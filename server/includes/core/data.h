#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <vector>

#include "common/includes/core/character_traits.h"
constexpr size_t MAX_DATA = 30;

#pragma pack(push, 1)
struct ItemInstanceData {
    uint8_t type_item;
    int32_t x;
    int32_t y;
};

struct PlayerData {
    char username[MAX_DATA];
    char password[MAX_DATA];
    /*Pose del personaje*/
    int32_t x;
    int32_t y;
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
    std::vector<ItemInstanceData> inventory;
    std::vector<ItemInstanceData> equipment;
};

struct CitizenNpcData {
    uint8_t  type;  // TypeNPC
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t direction;
};

struct CreatureData {
    uint8_t  type;  // TypeNPC
    uint32_t x;
    uint32_t y;
    uint8_t direction;
    uint16_t hp;
    uint16_t max_hp;
    uint16_t level;
    uint16_t range_attack;
};
struct TreasureStateData {
    uint32_t pos_x;
    uint32_t pos_y;
};
struct GoldBagsData {
    uint32_t amount;
    uint32_t pos_x;
    uint32_t pos_y;
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
