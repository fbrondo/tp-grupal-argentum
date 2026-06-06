#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <string>

#include "common/includes/core/character_traits.h"
constexpr size_t MAX_DATA = 30;
constexpr size_t MAX_INVENTORY = 50;
constexpr size_t MAX_EQUIPMENT = 4;

#pragma pack(push, 1)
struct ItemInstanceData {
    //uint32_t id;
    uint8_t type_item;
    uint8_t classification; /* DEFENSIVE, ATTACK, HEALING*/
    uint8_t body_part_use;
    int32_t x;
    int32_t y;
};

struct PlayerData {
    char username[MAX_DATA];
    char password[MAX_DATA];
    // Pose del personaje
    int32_t x;
    int32_t y;
    uint8_t direction;
    // personaje
    CharacterTraits charact_traits;
    // atributos
    uint8_t level;
    uint16_t hp;
    uint16_t mana;
    /* inventario*/
    uint32_t golden;
    std::vector<ItemInstanceData> inventory;
    std::vector<ItemInstanceData> equipment;
};

struct NpcStateData {
    uint8_t  type;// TypeNPC
    uint32_t pos_x;
    uint32_t pos_y;
};
struct TreasureStateData {
    uint32_t pos_x;
    uint32_t pos_y;
};
struct WorldStateData {
    std::vector<NpcStateData>  npcs;
    std::vector<TreasureStateData> treasures;
    std::vector<ItemInstanceData> items;
};
#pragma pack(pop)
#endif
