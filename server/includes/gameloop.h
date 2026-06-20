#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "common/includes/core/user.h"
#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "server/includes/clan_manager.h"
#include "server/includes/core/bank.h"
#include "server/includes/core/config.h"
#include "server/includes/core/item.h"
#include "server/includes/core/resurrect.h"
#include "server/includes/definitions.h"
#include "server/includes/inventory.h"
#include "server/includes/monitor_queues.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/npc/creature.h"
#include "server/includes/persistence.h"
#include "server/includes/player.h"
#include "server/includes/spawn_manager.h"
#include "server/includes/world.h"

class Gameloop: public Thread {

private:
    std::mt19937 gen;
    Id next_npc_id{0};

    MonitorQueues& monitor;
    QueueCmd& commands_queue;
    GameConfig conf;
    ClanManager clan_manager;
    World world;
    Persistence persistence;
    SpawnManager spawn;
    Bank bank;
    // std::unique_ptr<Banker> banker;
    // std::unique_ptr<Merchant> merchant;
    // std::unique_ptr<Priest> priest;

    std::map<Id, std::unique_ptr<Player>> players;
    std::map<Id, std::unique_ptr<CitizenNPC>> citizen_npcs;
    std::map<Id, std::unique_ptr<Creature>> creatures;

    std::vector<SoundEffectSnapshotData> sounds_of_current_tick;
    std::vector<VisualEffectSnapshotData> visual_effects_of_current_tick;
    struct ResurrectPending {
        uint32_t time_left_ms;
        Id healer_id;
    };
    std::map<Id, ResurrectPending> pending_resurrects;

    // void loadWorld(const WorldStateData& data);
    // void loadTreasures(const WorldStateData& world_data);
    // void loadCreatures(const WorldStateData& world_data);
    // void loadCitizenNPCs(const WorldStateData& world_data);
    // void loadGoldBags(const WorldStateData& world_data);
    // void loadItems(const WorldStateData& world_data);


    Character createCharacter(const CharacterTraits& traits) const;
    // Equipment createEquipment(const std::vector<ItemInstanceData>& equip) const;
    Inventory loadingInventory(const PlayerData& player);
    void loadingPlayerData(const Id& player_id, const PlayerData& player_data);
    void createNewPlayer(const User& user, const CharacterTraits& traits);


    void executeBroacastSnapshot();

    /*Metodos del Comando Attack*/
    bool isItPossibleToAttack(const Id& player_id, const CombatEntity& victim, Weapon& weapon);
    CombatEntity* inSearchOfTheVictimAttack(const Id& id_search) const;
    std::vector<Defense*> getPlayerDefensiveEquipment(const Id& player_id);
    void execuetRequest();
    std::optional<Id> findPlayerIdByUsername(const std::string& username) const;
    void sendClanOpResult(Id caller_id, const ClanOpResult& result);
    uint32_t calculateResurrectionDelayMs(const Position& from, const Position& to) const;
    void resurrectPlayerAtHealer(Id player_id, Id healer_id);
    Player* findNearestPlayer(const Creature& creature, Id& player_id);
    void moveCreatureTowards(Id creature_id, Creature& creature, const Position& target);
    void executeCreatureAttack(Creature& creature, Id player_id);
    void updateCreatures(uint32_t delta_ms);

public:
    explicit Gameloop(GameConfig&& conf_, MonitorQueues& monitor, QueueCmd& cmmds_queue);

    void processHandleSignup(const Id& player_id, const User& user, const CharacterTraits& traits);
    void processHandleLogin(const Id& player_id, const User& user);
    void sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response);
    void executeAttackPlayer(const Id& player_id, const Id& victim_id);

    void processMovePlayer(Id player_id, Direction dir);
    void processBuyItem(Id player_id, Id npc_id, TypeItem type_item); /*enviabas un ID item_id*/
    void processSellItem(Id player_id, Id npc_id, TypeItem type_item);
    void processPlayerDropItem(Id player_id, size_t index_slot);
    void processPlayerPickUp(Id player_id);

    void processPlayerWithdrawItem(Id player_id, Id npc_id, TypeItem type_item);
    void processPlayerDepositItem(Id player_id, Id npc_id, TypeItem type_item);
    void processPlayerDepositGold(Id player_id, uint32_t amount);
    void processPlayerWithdrawGold(Id player_id, uint32_t amount);
    void processPlayerEquipItem(Id player_id, size_t slot_id);

    void processPlayerDisconnet(Id player_id);
    // void processPlayerUnequipItem(Id player_id, size_t slot_id);
    // void processPlayerUseItem(Id player_id, Id instance_id);


    void processPlayerMeditate(Id player_id);
    void processPlayerHeal(Id player_id);
    void processPlayerResurrect(Id player_id);
    void processPlayerDebugKill(Id player_id);
    void processListItems(Id player_id, Id npc_id);


    void processClanFound(Id player_id, const std::string& clan_name);
    void processClanJoin(Id player_id, const std::string& clan_name);
    void processClanReview(Id player_id);
    void processClanAccept(Id player_id, const std::string& nick);
    void processClanReject(Id player_id, const std::string& nick);
    void processClanBan(Id player_id, const std::string& nick);
    void processClanKick(Id player_id, const std::string& nick);
    void processClanLeave(Id player_id);

    void respawnDeadNpcs();
    void updatePlayersAttributes();
    void updateStatePlayers();
    void updateStateWorld();
    void run() override;
    void stop() override;
    ~Gameloop();
};

#endif
