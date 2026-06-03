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

#define INVALID_REGISTER "Ese username no esta disponible. Por favor elige otro."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."

Gameloop::Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor),
        commands_queue(cmmds_queue),
        files_data(loader_conf.getFilesData()),
        game_conf(loader_conf.getdGameConfiguration()),
        world_game(this->files_data.map, this->info_items),
        persistence(this->files_data),
        formulas() {
    loader_conf.loadRaces(this->info_races);
    loader_conf.loadClases(this->info_clases);
    loader_conf.loadItems(this->info_items);
}

void Gameloop::handleSignup(SignupCommand* cmd) {
    auto [id, username, password] = cmd->getSignupInfo();
    if (this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseSignup>(false, "Username already taken."));
        return;
    }
    PlayerData data{};
    std::strncpy(data.username, username.c_str(), sizeof(data.username) - 1);
    std::strncpy(data.password, password.c_str(), sizeof(data.password) - 1);
    this->persistence.savePlayer(data);
    this->monitor.queueTheServerResponse(id, std::make_unique<ResponseSignup>(true));
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
    Snapshot snap = this->resp.buildSnapshot(this->players, this->world_game);
    std::unique_ptr<ResponseSnapshot> resp_snap =
            std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}

void Gameloop::initNewPlayer(Id player_id, const TypeRace& race, const TypeClase& clase) {
    Position pos_inicial{10, 10};

    Player new_player(player_id, std::move(pos_inicial), this->info_races.at(race),
                      this->info_clases.at(clase), this->game_conf.player_init, this->formulas);

    this->players.emplace(player_id, std::move(new_player));
}

void Gameloop::registerNewPlayer(CreateCharacterCommand* cmd) {
    auto [player_id, username, pass, race, clase] = cmd->getRegistrationInfo();
    Print::printNewPlayerArrived(player_id, username, pass, race, clase);
    if (this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                player_id, std::make_unique<ResponseLogin>(false, INVALID_REGISTER));
        return;
    }
    this->initNewPlayer(player_id, race, clase);
    cmd->execute(this->world_game);
    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseLogin>(true));
    this->executeBroacastSnapshot();
}

void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world_game.isWalkable(player_id, direction)) {
        cmd->execute(this->world_game);
        this->executeBroacastSnapshot();
    }
}

void Gameloop::executeAttackPlayer(AttackCommand* cmd) {
    auto [attacker_id, target_id] = cmd->getAttackInfo();

    if (this->players.count(attacker_id) == 0)
        return;
    Player& attacker = this->players.at(attacker_id);

    if (!attacker.isAlive())
        return;

    // FILTRO DE ARMA
    TypeItem weapon_type = attacker.getEquipment().getHandItem();
    if (weapon_type == TypeItem::NONE)
        return;

    // BUSCAMOS A LA VÍCTIMA
    CombatEntity* victim = nullptr;

    if (this->players.count(target_id) > 0) {
        victim = &this->players.at(target_id);
    } else if (this->info_NPC.count(target_id) > 0) {
        NPC* npc_generico = this->info_NPC.at(target_id).get();
        victim = dynamic_cast<CombatEntity*>(npc_generico);
        if (!victim) {
            return;
        }
    }

    if (!victim)
        return;

    // CHEQUEO DE DISTANCIA
    Position pos_attacker = attacker.getPosition();
    Position pos_target = victim->getPosition();

    int distance = std::abs(static_cast<int>(pos_attacker.x) - static_cast<int>(pos_target.x)) +
                   std::abs(static_cast<int>(pos_attacker.y) - static_cast<int>(pos_target.y));

    // CHEQUEO DE RANGO
    const Item& item_template = *(this->info_items.at(weapon_type));
    uint16_t range = item_template.getRange();

    if (distance > range)
        return;

    // CALCULO DE DAÑO
    bool is_critical = false;
    uint16_t damage = attacker.calculateDamage(is_critical, this->info_items);

    if (!is_critical && victim->dodgeAttack()) {
        // Registrar sonido de esquivado
        return;
    }

    victim->receiveDamage(damage, this->info_items);

    this->executeBroacastSnapshot();
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
        } else if (CreateCharacterCommand* register_cmd =
                           dynamic_cast<CreateCharacterCommand*>(cmd.get())) {
            std::cerr << "[Gameloop] -> CreateCharacterCommand" << std::endl;
            this->registerNewPlayer(register_cmd);
        } else if (MoveCommand* move_cmd = dynamic_cast<MoveCommand*>(cmd.get())) {
            this->executeMovePlayer(move_cmd);
        } else if (AttackCommand* attack_cmd = dynamic_cast<AttackCommand*>(cmd.get())) {
            this->executeAttackPlayer(attack_cmd);
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
