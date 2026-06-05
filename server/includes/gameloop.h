#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "server/includes/commands/command.h"
#include "server/includes/commands/command_attack.h"
#include "server/includes/commands/command_buy_item.h"
#include "server/includes/commands/command_chat.h"
#include "server/includes/commands/command_drop_item.h"
#include "server/includes/commands/command_interact.h"
#include "server/includes/commands/command_login.h"
#include "server/includes/commands/command_move.h"
#include "server/includes/commands/command_sell_item.h"
#include "server/includes/commands/command_signup.h"
#include "server/includes/commands/command_take_item.h"
#include "server/includes/commands/command_use_item.h"
#include "server/includes/core/clase.h"
#include "server/includes/core/config.h"
#include "server/includes/core/item.h"
#include "server/includes/core/race.h"
#include "server/includes/definitions.h"
#include "server/includes/game_config_loader.h"
#include "server/includes/monitor_queues.h"
#include "server/includes/persistence_manager.h"
#include "server/includes/player.h"
#include "server/includes/response_builder.h"
#include "server/includes/world.h"

class Gameloop: public Thread {

private:
    Id nex_npc_instance{0};
    Id next_item_instance_id{1};
    MonitorQueues& monitor;
    QueueCmd& commands_queue;

    const FileData files_data;
    const GameConfig conf;
    World world;
    PersistenceManager persistence;
    /*Esto tal vez deberia pasarse por referencia*/
    ResponseBuilder resp;

    std::unordered_map<TypeRace, Race> info_races;
    std::unordered_map<TypeClase, Clase> info_clases;

    std::map<Id, std::unique_ptr<Player>> players;
    std::map<TypeItem, std::unique_ptr<Item>> info_items;
    std::map<Id, std::unique_ptr<NPC>> info_NPC;

    void handleSignup(SignupCommand* cmd);
    void handleLogin(LoginCommand* cmd);

    void executeBroacastSnapshot();

    void executeMovePlayer(MoveCommand* move_cmd);

    /*Metodos del Comando Attack*/
    bool isItPossibleToAttack(const Id& player_id, const Id& victim, Weapon& weapon);
    CombatEntity* inSearchOfTheVictimAttack(const Id& id_search);
    std::vector<Defense*> getInfoAboutThePlayerDefensiveEquipment(const Id& player_id);
    void executeAttackPlayer(AttackCommand* attack_cmd);

    void execuetRequest();

public:
    explicit Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue);
    void run() override;
};

#endif
