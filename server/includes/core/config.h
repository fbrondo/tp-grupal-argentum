#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "common/includes/types.h"
#include "server/includes/core/clase.h"
#include "server/includes/core/item.h"
#include "server/includes/core/race.h"
#include "server/includes/core/region.h"

using Path = std::filesystem::path;

struct PathsConfig {
    Path game;
    Path clases;
    Path races;
    Path items;
    Path npcs;
    Path creatures;
    Path regions;
};
struct FileData {
    Path players; /*datos de los usuarios*/
    Path indx_players;
    Path world; /*Donde guardo el estado del mundo*/
    Path map;   /*Donde esta guardado el map que me envia el editor*/
};
struct PlayerStateInit {
    uint8_t level;
    uint32_t golden_init;
    uint32_t max_inventory;
};
struct ClanConfig {
    uint32_t max_members;
    // ClanConfig() = default;
};
struct NpcSafeZone {
    TypeNPC type;
    std::string name;
    std::vector<TypeItem> ids_items_store;
};
struct CreatureConfig {
    TypeNPC type;
    std::string name;
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
    PlayerStateInit player_init;
    ClanConfig clan;
    TimesConfig times;
    std::map<TypeRace, Race> races;
    std::map<TypeClase, Clase> clases;
    std::map<std::string, NpcSafeZone> npcs;
    std::map<std::string, CreatureConfig> creatures;
    std::map<Region, std::unique_ptr<RegionWorld>> regions;
    std::map<TypeItem, std::unique_ptr<Item>> items;

    GameConfig(GameConfig&&) = default;
    GameConfig& operator=(GameConfig&&) = default;
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;
    GameConfig() = default;

    // GameConfig(PlayerStateInit player_init_, ClanConfig clan_, TimesConfig times_):
    // player_init(player_init_),
    // clan(clan_),
    // times(times_) {}
};
#endif
