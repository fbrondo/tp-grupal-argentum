#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "common/includes/core/user.h"
#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "npc/priest.h"
#include "server/includes/core/config.h"
#include "server/includes/core/creature_attributes.h"
#include "server/includes/core/inventory.h"
#include "server/includes/core/item.h"
#include "server/includes/definitions.h"
#include "server/includes/monitor_queues.h"
#include "server/includes/npc/banker.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/npc/creature.h"
#include "server/includes/npc/merchant.h"
#include "server/includes/persistence.h"
#include "server/includes/player.h"
#include "server/includes/world.h"

class Gameloop: public Thread {

private:
    std::mt19937 gen;
    Id next_item_id{0};
    Id next_npc_id{0};
    Id next_creature_id{0};
    Direction direction_default{UP};
    MonitorQueues& monitor;
    QueueCmd& commands_queue;

    GameConfig conf;
    World world;
    Persistence persistence;

    std::unique_ptr<Banker> banker;
    std::unique_ptr<Merchant> merchant;
    std::unique_ptr<Priest> priest;

    std::map<Id, CitizenNPC*> npcs;
    std::map<Id, std::unique_ptr<Player>> players;
    std::map<Id, std::unique_ptr<Creature>> creatures;
    std::map<Id, TypeItem> items;

    void initConfigurationCitizenNPC();
    void initCreatures(Region type_region, const Id& zona_id);
    void initNpcSafeZone(Region type_region, const Id& zona_id);
    void initNPCS();
    void initWorld();
    void loadWorld(const WorldStateData& data);

    void loadTreasures(const WorldStateData& world_data);
    void loadCreatures(const WorldStateData& world_data);
    void loadCitizenNPCs(const WorldStateData& world_data);
    void loadGoldBags(const WorldStateData& world_data);
    void loadItems(const WorldStateData& world_data);

    void treasurePlacemen(const Position& position);
    std::vector<ItemInstance> items_drop_creature();
    NpcAttributes creatingAttributesToCreature(const std::string& name_npc);
    void createCreature(TypeNPC type, Pose&& pose, const NpcAttributes& attrib);
    void createNpcCity(Pose&& pose, TypeNPC type);
    Character createCharacter(const CharacterTraits& traits) const;
    // Equipment createEquipment(const std::vector<ItemInstanceData>& equip) const;
    Inventory loadingInventory(const PlayerData& player);
    void loadingPlayerData(const Id& player_id, const PlayerData& player_data);
    void createNewPlayer(const Id& player_id, const User& user, const CharacterTraits& traits);


    void executeBroacastSnapshot();

    /*Metodos del Comando Attack*/
    bool isItPossibleToAttack(const Id& player_id, const Id& victim, Weapon& weapon);
    CombatEntity* inSearchOfTheVictimAttack(const Id& id_search) const;
    std::vector<Defense*> getPlayerDefensiveEquipment(const Id& player_id);
    void execuetRequest();

public:
    explicit Gameloop(GameConfig&& conf_, MonitorQueues& monitor, QueueCmd& cmmds_queue);

    void processHandleSignup(const Id& player_id, const User& user, const CharacterTraits& traits);
    void processHandleLogin(const Id& player_id, const User& user);
    void processDisconnect(Id player_id);
    void sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response);
    void executeAttackPlayer(const Id& player_id, const Id& victim_id);

    void processMovePlayer(Id player_id, Direction dir);
    void processBuyItem(Id player_id, Id npc_id, uint8_t type_item); /*enviabas un ID item_id*/
    void processSellItem(Id player_id, Id npc_id, Id id_item);
    void processPlayerPickUp(Id player_id);
    void processPlayerDropItem(Id player_id, Id instance_id);
    void processPlayerWithdrawItem(Id player_id, Id instance_id);
    void processPlayerDepositItem(Id player_id, Id instance_id);
    void processPlayerDepositGold(Id player_id, uint32_t amount);
    void processPlayerWithdrawGold(Id player_id, uint32_t amount);
    void processPlayerMeditate(Id player_id);
    void processPlayerHeal(Id player_id);
    // void processPlayerResurrect(Id player_id);
    void processListItems(Id player_id, Id npc_id);

    void updatePlayersAttributes();
    void updateStatePlayers();
    void updateStateWorld();
    void run() override;
    void stop() override;
    ~Gameloop();
};

#endif
