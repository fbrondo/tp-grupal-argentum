#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <string>
#include <filesystem>
struct InitialPlayerConfig {
    uint8_t level;
    uint32_t golden_init;
    uint16_t max_inventory;
};

struct ClanConfig {
    uint8_t max_members;
};

struct PathConfig {
    std::filesystem::path players_data; /*datos de los usuarios*/
    std::filesystem::path player_indx;
    std::filesystem::path world_data; /*Donde guardo el estado del mundo*/
    std::filesystem::path map_path; /*Donde esta guardado el map que me envia el editor*/
};

#endif
