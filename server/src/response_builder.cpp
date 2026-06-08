#include "server/includes/response_builder.h"

#include "server/includes/core/instances.h"
Snapshot ResponseBuilder::buildSnapshot(
    const std::map<Id, std::unique_ptr<Player>>& players,
    const std::map<Id, NpcInstance>& npcs,
    const std::map<Id, NpcInstance>& creatures,
    const std::map<Id, ItemInstance>& items_on_floor,
    const std::map<Id, GoldBagInstance>& gold_on_floor) {
    Snapshot snap;

    for (const auto& [id, player] : players) {
        PlayerSnapshotData p_data{};
        
        p_data.id = static_cast<uint32_t>(id);
        
        Pose pose = player->getPose();
        p_data.pos_x = pose.position.x;
        p_data.pos_y = pose.position.y;
        p_data.direction = static_cast<uint8_t>(pose.direct);

        p_data.hp = player->getHp(); 
        p_data.max_hp = player->getHpMax();
        p_data.mana = player->getMana();
        p_data.max_mana = player->getManaMax();

        PlayerData p_info = player->getPlayerData();
        p_data.raza = p_info.charact_traits.race;
        p_data.clase = p_info.charact_traits.clase;
        p_data.head_id = p_info.charact_traits.head;
        p_data.body_id = p_info.charact_traits.body;

        // Equipamiento visual
        p_data.weapon_id = static_cast<uint8_t>(player->getHandItem());
        p_data.shield_id = 0; 
        p_data.armor_id = 0;  
        p_data.head = p_info.charact_traits.head; 

        p_data.flags = 0;
        if (player->isMeditating()) {
            p_data.flags |= (1 << 1); 
        }

        snap.players.push_back(p_data);
    }

    for (const auto& npc : npcs | std::views::values) {
        NpcSnapshotData npc_data{};
        npc_data.id = static_cast<uint32_t>(npc.id);
        npc_data.type_id = static_cast<uint16_t>(npc.type);
        npc_data.pos_x = npc.pose.position.x;
        npc_data.pos_y = npc.pose.position.y;
        npc_data.hp_actual = 100; 
        npc_data.is_alive = 1;

        snap.npcs.push_back(npc_data);
    }

    for (const auto& creature : creatures | std::views::values) {
        NpcSnapshotData npc_data{};
        npc_data.id = static_cast<uint32_t>(creature.id);
        npc_data.type_id = static_cast<uint16_t>(creature.type);
        npc_data.pos_x = creature.pose.position.x;
        npc_data.pos_y = creature.pose.position.y;
        npc_data.hp_actual = 100;
        npc_data.is_alive = 1;

        snap.npcs.push_back(npc_data);
    }

    for (const auto& item_floor : items_on_floor | std::views::values) {
        ItemGroundSnapshotData item_data{};
        item_data.item_id = static_cast<uint16_t>(item_floor.type);
        item_data.pos_x = item_floor.pos.x;
        item_data.pos_y = item_floor.pos.y;

        snap.items_on_floor.push_back(item_data);
    }

    for (const auto& gold_pile : gold_on_floor | std::views::values) {
        GoldPileGroundSnapshotData gold_data{};
        gold_data.amount = 50; 
        gold_data.pos_x = gold_pile.pos.x;
        gold_data.pos_y = gold_pile.pos.y;

        snap.gold_piles.push_back(gold_data);
    }

    return snap;
}

ResponseBuilder::~ResponseBuilder() = default;
