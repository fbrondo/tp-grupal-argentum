#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <vector>

#include "common/includes/core/character_traits.h"
#include "common/includes/core/position.h"
#include "server/includes/core/creature_attributes.h"
#include "server/includes/core/map.h"
constexpr size_t MAX_DATA = 30;

#pragma pack(push, 1)

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
    uint8_t direction;
    /*Personaje*/
    CharacterTraits charact_traits;
    /*Atributos*/
    uint8_t level;
    uint32_t xp;
    uint16_t hp;
    uint16_t mana;
    uint32_t golden;
    std::vector<SlotData> inventory;
    std::vector<SlotData> equipment; /*guarda el index del slot que esta euipado*/
    /*BANCO*/
    uint32_t golden_dep;
    std::vector<SlotData> box;
};

struct CitizenNpcData {
    char name[MAX_DATA];
    uint8_t type;
    uint32_t zone_id;
    Position position;
    uint8_t direction;
};

struct CreatureData {
    char name[MAX_DATA];
    uint8_t type;
    uint32_t zone_id;
    Position position;
    uint8_t direction;
    NpcAttributes attributes;
};
struct ItemInstanceData {
    uint8_t type_item;
    Position position;
};

struct TreasureStateData {
    uint32_t zone_id;
    uint32_t amount;
    Position position;
    std::vector<uint8_t> types_items;
};
struct GoldBagsData {
    uint32_t amount;
    Position position;
};
struct WorldStateData {
    std::vector<CitizenNpcData> citizen;
    std::vector<CreatureData> creatures;
    std::vector<TreasureStateData> treasures; /*Guardo*/
    std::vector<GoldBagsData> gold_bags;      /*Guardo*/
    std::vector<ItemInstanceData> items;      /*Guardo*/
};
#pragma pack(pop)
#endif
