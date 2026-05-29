#include "gameloop.h"

#include <memory>
#include <string>
#include <utility>

#include "../includes/responses/response.h"
#include "../includes/responses/response_login.h"
#include "../includes/snapshot.h"

Gameloop::Gameloop(MonitorQueues& monitor, QueueCmd& cmmds_queue): monitor(monitor), commands_queue(cmmds_queue), persistence("data/"){
    
}
/*Levantamos un jugador - Deberia servir tanto para
    - Uno que se registra
    - Uno que ya estaba registrado y solo estamos cargando sus datos.
*/
Player&& Gameloop::initPlayer(const TypeRace& race, const TypeClase& clase) {
    /*El oro:50, el nivel 1 y la posicion no seran harcodeados
        - El oro y nivel inicial los obtengo de mis archivos game.toml
        - La position me la a dar el mundo (seguramente en una zona segura)
    */
    Player new_player(this->info_races[race], this->info_clases[clase], 50, 1);
    return std::move(new_player);
}
void Gameloop::registerNewPlayer(CreateCharacterCommand* register_cmd) {
    auto [id, username, pass, race, clase] = register_cmd->getRegistrationInfo();
    std::unique_ptr<ResponseLogin> response_register;

    /*Comprobamos que el username no coincide con el de ningun jugador ya registrado*/
    if(this->persistence.exists(username)) {
        response_register = std::make_unique<ResponseLogin>(false, "Ese nombre no esta disponible. Por favor elige otro.");
    } else { /*No hay conciendencia, registro al nuevo jugador*/
        this->players.insert({id, this->initPlayer(race, clase)});
        register_cmd->execute(this->world_game);
        response_register = std::make_unique<ResponseLogin>(true);
    }
    this->monitor.queueTheServerResponse(id, std::move(response_register));
}
void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world_game.isWalkable(player_id, direction)) {
        cmd->execute(this->world_game);
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
