#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "server/includes/core/data.h"
#include "server/includes/core/snapshot.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/responses/response_signup.h"
#include "server/includes/responses/response_snapshot.h"
#include "server/print.h"

#define INVALID_REGISTER "Username already taken."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."

Gameloop::Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor),
        commands_queue(cmmds_queue),
        files_data(loader_conf.getFilesData()),
        game_conf(loader_conf.getdGameConfiguration()),
        world(this->files_data.map),
        persistence(this->files_data) {
    loader_conf.loadRaces(this->info_races);
    loader_conf.loadClases(this->info_clases);
    loader_conf.loadItems(this->info_items);
}

void Gameloop::handleSignup(SignupCommand* cmd) {
    auto [id, username, pass, traits] = cmd->getSignupInfo();
    Print::printNewPlayerArrived(id, username, pass, static_cast<TypeRace>(traits.race),
                                 static_cast<TypeClase>(traits.clase));
    if (this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseSignup>(false, INVALID_REGISTER));
        return;
    }
    /*Creacion del jugador*/
    cmd->execute(this->world); /*Ubico al jugador en el mundo*/
    User user(std::move(username), std::move(pass));
    Character character(this->info_races.at(static_cast<TypeRace>(traits.race)),
                        this->info_clases.at(static_cast<TypeClase>(traits.clase)), traits.head,
                        traits.body);
    Pose pose(this->world.positionPlayerInTheWorld(id), UP);
    Player new_player(std::move(user), std::move(pose), std::move(character),
                      this->game_conf.player_init);
    PlayerData player_data = new_player.getPlayerData();
    this->persistence.savePlayer(player_data);
    this->players.emplace(id, std::move(new_player));

    /*Informacion que se envia al usuario*/
    this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true));
    this->executeBroacastSnapshot();
}

void Gameloop::handleLogin(LoginCommand* cmd) {
    auto [id, username, password] = cmd->getLoginInfo();
    if (!this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseLogin>(false, "User not found."));
        return;
    }
    PlayerData data = this->persistence.loadPlayer(username);
    if (std::string(data.password) != password) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseLogin>(false, "Wrong password."));
        return;
    }
    if (data.level == 0) {
        this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true, "none"));
        return;
    }
    std::ostringstream payload;
    payload << data.username << " " << static_cast<int>(data.race) << " "
            << static_cast<int>(data.clase) << " " << static_cast<int>(data.level);
    this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true, payload.str()));
}

void Gameloop::executeBroacastSnapshot() {
    Snapshot snap = this->resp.buildSnapshot(this->players, this->world);
    std::shared_ptr<ResponseSnapshot> resp_snap =
            std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}

void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world.isWalkable(player_id, direction)) {
        cmd->execute(this->world);
        this->executeBroacastSnapshot();
    }
}

void Gameloop::execuetRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        std::cerr << "[Gameloop] Processing command for player " << cmd->getIdPlayer() << std::endl;
        if (SignupCommand* signup_cmd = dynamic_cast<SignupCommand*>(cmd.get())) {
            std::cerr << "[Gameloop] -> SignupCommand" << std::endl;
            this->handleSignup(signup_cmd);
        } else if (LoginCommand* login_cmd = dynamic_cast<LoginCommand*>(cmd.get())) {
            std::cerr << "[Gameloop] -> LoginCommand" << std::endl;
            this->handleLogin(login_cmd);
        } else if (MoveCommand* move_cmd = dynamic_cast<MoveCommand*>(cmd.get())) {
            this->executeMovePlayer(move_cmd);
        } else {
            std::cerr << "[Gameloop] -> UnknownCommand (discarded)" << std::endl;
        }
    }
}

void Gameloop::run() {
    std::cerr << "[Gameloop] run() started" << std::endl;
    try {
        while (should_keep_running()) {
            this->execuetRequest();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (const ClosedQueue&) {
        std::cerr << "[Gameloop] run() exited: ClosedQueue" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Gameloop] run() exited: " << e.what() << std::endl;
    }
    std::cerr << "[Gameloop] run() finished" << std::endl;
}
