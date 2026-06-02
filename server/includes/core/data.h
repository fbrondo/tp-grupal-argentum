#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <string>

constexpr size_t MAX_DATA = 30;
#pragma pack(push, 1)
struct PlayerData {
    char     username[MAX_DATA];
    char     password[MAX_DATA];
    /* posición*/
    int32_t  x;
    int32_t  y;
    /*Caracteristicas del personaje*/
    uint8_t  race;
    uint8_t  clase;

    /* Atributos del jugador*/
    uint8_t  level;
    uint16_t hp;
    uint16_t mana;

    /* inventario*/
    uint32_t golden;
    uint32_t number_of_items;

    /*Equipo*/
    uint32_t number_of_items_equiment;

};
#pragma pack(pop)


#endif
