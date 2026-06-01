#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include "server/includes/definitions.h"

struct InitialPlayerConfig {
    uint8_t level;
    uint32_t golden_init;
    uint16_t max_inventory;
};

struct ClanConfig {
    uint8_t max_members;
};

struct PathConfig {
    Path players_data; /*datos de los usuarios*/
    Path players_indx;
    Path world_data; /*Donde guardo el estado del mundo*/
    Path map_path; /*Donde esta guardado el map que me envia el editor*/
};

#endif
