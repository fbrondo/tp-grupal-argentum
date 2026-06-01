#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "server/includes/core/snapshot.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/responses/response_snapshot.h"

Gameloop::Gameloop(GameConfig& game_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue) try :
    paths(game_conf.getPaths()),
    world_game(paths.map_path),
    monitor(monitor),
    commands_queue(cmmds_queue),
    persistence(paths) {
    std::cerr << "Gameloop construido OK" << std::endl;
} catch (const std::exception& e) {
    std::cerr << "EXCEPCION en constructor Gameloop: " << e.what() << std::endl;
    throw;
} catch (...) {
    std::cerr << "EXCEPCION DESCONOCIDA en constructor Gameloop" << std::endl;
    throw;
}
/*Levantamos un jugador - Deberia servir tanto para
    - Uno que se registra
    - Uno que ya estaba registrado y solo estamos cargando sus datos.
*/
Player Gameloop::initPlayer(const TypeRace& race, const TypeClase& clase, Inventory&& inv, uint8_t level) {
    Player new_player(std::move(inv), this->info_races.at(race), this->info_clases.at(clase), level);
    return new_player;
}
void Gameloop::registerNewPlayer(CreateCharacterCommand* register_cmd) {
    auto [id, username, pass, race, clase] = register_cmd->getRegistrationInfo();

    std::cout << " -- Client Arrived --" << std::endl;
    std::cout << " id: " << id << std::endl;
    std::cout << " username: " << username << std::endl;
    std::cout << " pass: " << pass << std::endl;
    std::cout << " race: " << std::to_string(race) << std::endl;
    std::cout << " clase: " << std::to_string(clase) << std::endl;

    std::unique_ptr<ResponseLogin> response_register;
    if(this->persistence.exists(username)) {  /*Comprobamos que el username no coincide con el de ningun jugador ya registrado*/
        response_register = std::make_unique<ResponseLogin>(false, "Ese nombre no esta disponible. Por favor elige otro.");
    } else {
        Inventory inv(50, 16);
        Player new_player = this->initPlayer(race, clase, std::move(inv), 1);
        std::cout << "Se inicializo" << std::endl;
        this->players.emplace(id, std::move(new_player));
        std::cout << "Se agrego" << std::endl;
        register_cmd->execute(this->world_game);
        std::cout << "Se registro exitosamente" << std::endl;
        response_register = std::make_unique<ResponseLogin>(true);
    }
    this->monitor.queueTheServerResponse(id, std::move(response_register));
}
void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world_game.isWalkable(player_id, direction)) {
        cmd->execute(this->world_game);
        Snapshot snap = this->resp.buildSnapshot(this->players, this->world_game);
        std::unique_ptr<ResponseSnapshot> response = std::make_unique<ResponseSnapshot>(std::move(snap));
        monitor.executeBroadcast(std::move(response));
    }
}

void Gameloop::execuetRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        if (CreateCharacterCommand* register_cmd = dynamic_cast<CreateCharacterCommand*>(cmd.get())) {
            this->registerNewPlayer(register_cmd);
        } 
        else if (MoveCommand* move_cmd = dynamic_cast<MoveCommand*>(cmd.get())) {
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
