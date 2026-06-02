#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>

using Path = std::filesystem::path;
struct PathsConfig {
    Path game;
    Path clases;
    Path races;
    Path items;
    Path npcs;
    Path regions;
};

struct FileData {
    Path players; /*datos de los usuarios*/
    Path indx_players;
    Path world; /*Donde guardo el estado del mundo*/
    Path map; /*Donde esta guardado el map que me envia el editor*/
};
struct PlayerStateInitConfig {
    uint8_t level;
    uint32_t golden_init;
    uint32_t max_inventory;
};

struct ClanConfig {
    uint32_t max_members;
};

struct TimesConfig {
    uint32_t server_update_frecuency;
    uint32_t update_player_atributes;
    uint32_t spawn_npcs;
    uint32_t disappear_dropped_item;
    uint32_t pesistence_data;
    uint32_t npc_attack_cooldown;
};

struct GameConfig {
    PlayerStateInitConfig player_init;
    ClanConfig clan;
    TimesConfig times;

    GameConfig(PlayerStateInitConfig p_init, ClanConfig clan, TimesConfig times):
    player_init(p_init), clan(clan), times(times) {}
};

#endif
