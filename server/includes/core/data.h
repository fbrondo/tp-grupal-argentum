#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <string>

constexpr size_t MAX_DATA = 30;
constexpr size_t MAX_INVENTORY = 50;
constexpr size_t MAX_EQUIPMENT = 4;

#pragma pack(push, 1)
struct ItemInstanceData {
    uint32_t id;
    uint16_t type_item;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct PlayerData {
    char username[MAX_DATA];
    char password[MAX_DATA];
    // posición
    int32_t x;
    int32_t y;
    // personaje
    uint8_t race;
    uint8_t clase;
    // atributos
    uint8_t level;
    uint16_t hp;
    uint16_t mana;

    /* inventario*/
    uint32_t golden;
    uint32_t number_of_items_inv;
    ItemInstanceData inventory[MAX_INVENTORY];

    /*Equipo*/
    uint32_t number_of_items_equip;
    ItemInstanceData equipment[MAX_EQUIPMENT];
};
#pragma pack(pop)


#endif
