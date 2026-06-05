#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <unordered_map>

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
    Path map;   /*Donde esta guardado el map que me envia el editor*/
};
struct PlayerStateInitConfig {
    uint8_t level;
    uint32_t golden_init;
    uint32_t max_inventory;
};

struct ClanConfig {
    uint32_t max_members;
};

struct NPCConfig {
    TypeNPC type;
    std::string name;
    /*Solo las criaturas tienen las siguientes campos*/
    uint16_t attack_range;
    uint16_t hp_max_initial;
    uint16_t minimal_level;
    uint16_t maximun_level;
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
    std::unordered_map<TypeRace, Race> races;
    std::unordered_map<TypeClase, Clase> clases;
    std::unordered_map<TypeNPC, NPCConfig> npcs;
    GameConfig() = default;
};

#endif
