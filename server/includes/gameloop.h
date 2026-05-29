#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>


#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "core/item.h"
#include "core/race.h"
#include "core/clase.h"
#include "persistence_manager.h"
#include "response_builder.h"
#include "definitions.h"
#include "monitor_queues.h"
#include "player.h"
#include "world.h"

#include "../includes/commands/command.h"
#include "../includes/commands/command_move.h"
#include "../includes/commands/command_attack.h"
#include "../includes/commands/command_buy_item.h"
#include "../includes/commands/command_chat.h"
#include "../includes/commands/command_drop_item.h"
#include "../includes/commands/command_interact.h"
#include "../includes/commands/command_login.h"
#include "../includes/commands/command_move.h"
#include "../includes/commands/command_sell_item.h"
#include "../includes/commands/command_take_item.h"
#include "../includes/commands/command_use_item.h"
#include "../includes/commands/command_create_character.h"

class MoveCommand;

class Gameloop: public Thread {

private:
    std::map<Id,Player> players;
    std::unordered_map<TypeRace, Race> info_races;
    std::unordered_map<TypeClase, Clase> info_clases;

    std::map<TypeItem, std::unique_ptr<Item>> config_items;
    std::map<Id, std::unique_ptr<NPC>> config_NPC;

    MonitorQueues& monitor;
    QueueCmd& commands_queue;
    ResponseBuilder resp;
    PersistenceManager persistence; /*Esto tal vez deberia pasarse por referencia*/
    World world_game;

    Player&& initPlayer(const TypeRace& race, const TypeClase& clase);
    void registerNewPlayer(CreateCharacterCommand* register_cmd);
    void executeMovePlayer(MoveCommand* move_cmd);
    void execuetRequest();

public:
    explicit Gameloop(MonitorQueues& monitor, QueueCmd& cmmds_queue);
    void run() override;
};

#endif
