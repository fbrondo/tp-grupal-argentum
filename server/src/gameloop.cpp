#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "server/includes/core/snapshot.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/responses/response_snapshot.h"
#include "server/print.h"


#define INVALID_REGISTER "Ese username no esta disponible. Por favor elige otro."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."

Gameloop::Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue): monitor(monitor), commands_queue(cmmds_queue),
    files_data(loader_conf.getFilesData()),
    game_conf(loader_conf.getdGameConfiguration()),
    world_game(this->files_data.map),
    persistence(this->files_data) {
    loader_conf.loadRaces(this->info_races) ;
    loader_conf.loadClases(this->info_clases);
    loader_conf.loadItems(this->info_items);
}

void Gameloop::executeBroacastSnapshot() {
    Snapshot snap = this->resp.buildSnapshot(this->players, this->world_game);
    std::unique_ptr<ResponseSnapshot> resp_snap = std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}

void Gameloop::initNewPlayer(Id player_id, const TypeRace& race, const TypeClase& clase) {
    Player new_player(this->info_races.at(race), this->info_clases.at(clase), this->game_conf.player_init); //this->initPlayer(race, clase, std::move(inv), 1);
    this->players.emplace(player_id, std::move(new_player));
}

void Gameloop::registerNewPlayer(CreateCharacterCommand* cmd) {
    auto [player_id, username, pass, race, clase] = cmd->getRegistrationInfo();
    Print::printNewPlayerArrived(player_id, username, pass, race, clase);
    if(this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseLogin>(false, INVALID_REGISTER));
        return;
    }
    this->initNewPlayer(player_id, race, clase);
    cmd->execute(this->world_game);
    this->monitor.queueTheServerResponse(player_id,std::make_unique<ResponseLogin>(true));
    this->executeBroacastSnapshot();
}

void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world_game.isWalkable(player_id, direction)) {
        cmd->execute(this->world_game);
        this->executeBroacastSnapshot();
    }
}

void Gameloop::execuetRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        if (CreateCharacterCommand* register_cmd = dynamic_cast<CreateCharacterCommand*>(cmd.get())) {
            this->registerNewPlayer(register_cmd);
        } else if (MoveCommand* move_cmd = dynamic_cast<MoveCommand*>(cmd.get())) {
            this->executeMovePlayer(move_cmd);
        }
    }
}

void Gameloop::run() {
    try {
        while (should_keep_running()) {
            this->execuetRequest();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (const ClosedQueue&) {

    } catch (const std::exception& e) {
        std::cerr << "Error en Game::run --" << e.what() << std::endl;
    }
}
