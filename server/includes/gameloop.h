#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>
#include <vector>
#include "common/includes/core/user.h"
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
#include "server/includes/core/region.h"
#include "server/includes/definitions.h"
#include "server/includes/game_config_loader.h"
#include "server/includes/monitor_queues.h"
#include "server/includes/persistence_manager.h"
#include "server/includes/player.h"
#include "server/includes/response_builder.h"
#include "server/includes/world.h"

class Gameloop: public Thread {

private:
    Id nex_trausure_id{0};
    Id nex_npc_id{0};
    Id next_item_id{0};

    MonitorQueues& monitor;
    QueueCmd& commands_queue;

    const FileData files_data;
    GameConfig conf;
    World world;
    PersistenceManager persistence;
    /*Esto tal vez deberia pasarse por referencia*/
    ResponseBuilder resp;

    /*Tal vez esto deberia pasarlo dentro de configuraciones*/
    // std::map<TypeRace, Race> info_races;
    // std::map<TypeClase, Clase> info_clases;
    // std::map<std::string, NpcConfig> info_npcs;
    // std::map<Region, std::unique_ptr<RegionWorld>> info_regions;
    // std::map<TypeItem, std::unique_ptr<Item>> info_items;

    std::map<Id, std::unique_ptr<Player>> players;
    std::map<Id, std::unique_ptr<NPC>> npcs;

    void initTreasures(const uint16_t& numbers_treasure, const Id& zona_id);
    void initCriatures(Region type_region, const Id& zona_id);
    void initNPCS();

    Character createCharacter(CharacterTraits traits);

    void handleLogin(LoginCommand* cmd);

    void executeBroacastSnapshot();

    /*Metodos del Comando Attack*/
    bool isItPossibleToAttack(const Id& player_id, const Id& victim, Weapon& weapon);
    CombatEntity* inSearchOfTheVictimAttack(const Id& id_search);
    std::vector<Defense*> getInfoAboutThePlayerDefensiveEquipment(const Id& player_id);
    void execuetRequest();

public:
    explicit Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue);

    void processHandleSignup(const Id& player_id, const User& user, const CharacterTraits& traits);
    void sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response);
    void executeAttackPlayer(AttackCommand* attack_cmd);
    void processMovePlayer(Id player_id, Direction dir);
    void process_buy_item(Id player_id, Id npc_id, Id item_id);
    void process_sell_item(Id player_id, Id npc_id, Id item_id);
    void processPlayerPickUp(Id player_id);
    void processPlayerDropItem(Id player_id, Id instance_id);
    void processPlayerWithdrawItem(Id player_id, Id instance_id);
    void processPlayerDepositItem(Id player_id, Id instance_id);
    void processPlayerDepositGold(Id player_id, uint32_t amount);
    void processPlayerWithdrawGold(Id player_id, uint32_t amount);
    void processPlayerMeditate(Id player_id);
    void processPlayerHeal(Id player_id);
    void processPlayerResurrect(Id player_id);
    void processListItems(Id player_id, Id npc_id);
    void run() override;
};

#endif
