#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <vector>

#include "common/includes/core/character_traits.h"
#include "server/includes/core/creature_attributes.h"
#include "server/includes/core/map.h"
constexpr size_t MAX_DATA = 30;

#pragma pack(push, 1)
struct ItemInstanceData {
    uint8_t type_item;
    Position position;
    // int32_t x;
    // int32_t y;
};

struct SlotData {
    uint8_t type_item;
    uint32_t index;
    uint32_t quantity;
    SlotData(): type_item(0), index(0), quantity(0) {}
    SlotData(uint8_t type_item_, uint32_t index_, uint32_t quantity_):
            type_item(type_item_), index(index_), quantity(quantity_) {}
};

struct PlayerData {
    char username[MAX_DATA];
    char password[MAX_DATA];
    /*Pose del personaje*/
    Position position;
    // int32_t x;
    // int32_t y;
    uint8_t direction;
    /*Personaje*/
    CharacterTraits charact_traits;
    /*Atributos*/
    uint16_t exp;
    uint8_t level;
    uint16_t hp;
    uint16_t mana;
    /* inventario*/
    uint32_t golden;
    std::vector<SlotData> inventory;
    std::vector<size_t> equipment; /*guarda el index del slot que esta euipado*/
};

struct CitizenNpcData {
    char name[MAX_DATA];
    uint8_t type;  // TypeNPC
    Position position;
    uint8_t direction;
};

struct CreatureData {
    uint8_t type;  // TypeNPC
    char name[MAX_DATA];
    // uint32_t x;
    // uint32_t y;
    Position position;
    uint8_t direction;
    NpcAttributes attributes;
    // uint16_t hp;
    // uint16_t max_hp;
    // uint16_t level;
    // uint16_t range_attack;
};
struct TreasureStateData {
    uint32_t amount;
    Position position;
    // uint32_t pos_x;
    // uint32_t pos_y;
};
struct GoldBagsData {
    uint32_t amount;
    Position position;
    // uint32_t pos_x;
    // uint32_t pos_y;
};
struct WorldStateData {
    std::vector<CitizenNpcData> citizen_npcs;
    std::vector<CreatureData> creatures;
    std::vector<TreasureStateData> treasures;
    std::vector<GoldBagsData> gold_bags;
    std::vector<ItemInstanceData> items;
};
#pragma pack(pop)
#endif
