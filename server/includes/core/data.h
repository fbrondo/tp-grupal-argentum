#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <string>

struct PathsConfig {
    std::string player_data;
    std::string player_indx;
    std::string map_path;
};

#pragma pack(push, 1)  // evita padding
struct PlayerData {
    char     username[32];
    char     password[32];
    uint8_t  race;
    uint8_t  clase;
    uint16_t level;
    uint16_t hp;
    uint16_t mana;
    uint16_t golden;
    int32_t  pos_x;
    int32_t  pos_y;
    //inventario nombre de los items
    //equipo nombre de los items
};
#pragma pack(pop)


#endif
