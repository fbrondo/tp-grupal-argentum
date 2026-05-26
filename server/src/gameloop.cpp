#include "gameloop.h"
//#include "../includes/commands/command.h"
#include <memory>
#include <string>
Gameloop::Gameloop(MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor), commands_queue(cmmds_queue) {}

class Command;
class MoveCommand;
class LoginCommand;
void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    /*Que el metodo encapsule en una tupla y haga el get de todo de una vez*/
    const Id id_player = cmd.getIdPlayer(); /*Hay que implementar*/
    const uint8_t direction = cmd.getDirectionOfMovement();
    if (this->world_game.isWalkable(id_player, direction)) {}
}
void Gameloop::execuetRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        if (MoveCommand* move_cmd = dynamic_cast<MoveCommand*>(cmd.get())) {}
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
