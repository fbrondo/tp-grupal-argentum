#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>
#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "server/includes/core/config.h"
#include "server/includes/commands/command.h"
#include "server/includes/commands/command_move.h"
#include "server/includes/commands/command_attack.h"
#include "server/includes/commands/command_buy_item.h"
#include "server/includes/commands/command_chat.h"
#include "server/includes/commands/command_drop_item.h"
#include "server/includes/commands/command_interact.h"
#include "server/includes/commands/command_login.h"
#include "server/includes/commands/command_move.h"
#include "server/includes/commands/command_sell_item.h"
#include "server/includes/commands/command_take_item.h"
#include "server/includes/commands/command_use_item.h"
#include "server/includes/commands/command_create_character.h"
#include "server/includes/core/item.h"
#include "server/includes/core/race.h"
#include "server/includes/core/clase.h"
#include "server/includes/core/config.h"
#include "server/includes/persistence_manager.h"
#include "server/includes/response_builder.h"
#include "server/includes/definitions.h"
#include "server/includes/monitor_queues.h"
#include "server/includes/player.h"
#include "server/includes/world.h"
#include "server/includes/game_config_loader.h"

class Gameloop: public Thread {

private:
    MonitorQueues& monitor;
    QueueCmd& commands_queue;

    const FileData files_data;
    const GameConfig game_conf;
    World world_game;
    PersistenceManager persistence;
    /*Esto tal vez deberia pasarse por referencia*/
    ResponseBuilder resp;

    std::unordered_map<TypeRace, Race> info_races;
    std::unordered_map<TypeClase, Clase> info_clases;

    std::map<Id,Player> players;
    std::map<TypeItem, std::unique_ptr<Item>> info_items;
    std::map<Id, std::unique_ptr<NPC>> info_NPC;

    Player initPlayer(const TypeRace& race, const TypeClase& clase, Inventory&& inv, uint8_t level);
    void registerNewPlayer(CreateCharacterCommand* register_cmd);
    void executeMovePlayer(MoveCommand* move_cmd);
    void execuetRequest();

public:
    explicit Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue);
    void run() override;
};

#endif
