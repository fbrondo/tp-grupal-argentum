#include "server/includes/game_config_loader.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <toml++/toml.hpp>

#include "common/includes/core/Statistics.h"
#include "common/includes/types.h"
#include "server/includes/game_config_loader.h"
#include "server/print.h"

GameConfigLoader::GameConfigLoader(Path config_dir_): config_dir(std::move(config_dir_)) {
    this->loadPaths();
}

Path GameConfigLoader::loadPath(const Table& config, const std::string& section_key,
                                const std::string& field_key) const {
    try {
        Path path =
                this->config_dir / (config[section_key][field_key].value<std::string>().value());
        return path;
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en loadPath -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void GameConfigLoader::loadNpcs(std::unordered_map<TypeNPC, NPCConfig>& npcs) {
    Table config = toml::parse_file(paths.npcs.string());
    Table_array* npcs_array = config["npcs"].as_array();
    for (const auto& npc_node: *npcs_array) {
        const Table& npc = *npc_node.as_table();
        NPCConfig npc_config;
        npc_config.type =  static_cast<TypeNPC>(npc["id_type"].value_or(0));
        npc_config.name = npc["name"].value_or<std::string>("");
        npc_config.attack_range  = static_cast<uint16_t>(npc["attack_range"].value_or(0));
        npc_config.hp_max_initial= static_cast<uint16_t>(npc["hp_max_initial"].value_or(0));
        npc_config.minimal_level = static_cast<uint16_t>(npc["minimal_level"].value_or(0));
        npc_config.maximun_level = static_cast<uint16_t>(npc["maximun_level"].value_or(0));
        npcs.emplace(npc_config.type, npc_config);
    }
}

void GameConfigLoader::loadPaths() {
    try {
        Path path = this->config_dir / FILE_PATHS;
        const Table config = toml::parse_file(path.string());
        this->paths.game = this->loadPath(config, "configuration", "game");
        this->paths.clases = this->loadPath(config, "configuration", "clases");
        this->paths.races = this->loadPath(config, "configuration", "races");
        this->paths.items = this->loadPath(config, "configuration", "items");
        this->paths.npcs = this->loadPath(config, "configuration", "npcs");
        this->paths.regions = this->loadPath(config, "configuration", "regions");

        this->data.players = this->loadPath(config, "data", "players");
        this->data.indx_players = this->loadPath(config, "data", "indx_players");
        this->data.world = this->loadPath(config, "data", "world");
        this->data.map = this->loadPath(config, "data", "map");

        Print::printLoadPathsAndFiles(path, this->paths, this->data);
    } catch (const toml::parse_error& err) {
        std::string mssgErr = "Error en loadPaths -- No se pudo parsear el archivo TOML";
        mssgErr += err.description();
        throw std::runtime_error(mssgErr);
    }
}

Statistics GameConfigLoader::loadTableStatistics(Table_array* stats_array) {
    Statistics stats;
    if (stats_array && !stats_array->empty()) {
        Table& stats_table = *(*stats_array)[0].as_table();
        stats.intelligence = static_cast<uint16_t>(
                stats_table["intelligence"].value_or(0));  // Ojo con la "s" en tu archivo
        stats.constitution = static_cast<uint16_t>(
                stats_table["constitution"].value_or(0));  // Ojo con la "i" intermedia
        stats.strength = static_cast<uint16_t>(stats_table["strength"].value_or(0));
        stats.agility = static_cast<uint16_t>(stats_table["agility"].value_or(0));
    }
    return stats;
}

void GameConfigLoader::loadRaces(std::unordered_map<TypeRace, Race>& info_races) {
    try {
        Table config = toml::parse_file(paths.races.string());
        Table_array* races_array = config["races"].as_array();
        if (!races_array) {
            throw std::runtime_error(" Error: falta [[races]] en el archivo TOML.");
        }
        for (auto&& node: *races_array) {
            Table& race_table = *node.as_table();
            auto type = static_cast<TypeRace>(race_table["id_type"].value_or(0));
            auto name = race_table["name"].value_or(std::string{});
            auto life_factor = static_cast<uint16_t>(race_table["life_factor"].value_or(1));
            auto recovery_factor = static_cast<uint16_t>(race_table["recovery_factor"].value_or(1));
            auto mana_factor = static_cast<uint16_t>(race_table["mana_factor"].value_or(0));

            Table_array* stats_array = race_table["statistics"].as_array();
            Statistics stats = this->loadTableStatistics(stats_array);
            Race race = Race(type, std::move(name), life_factor, recovery_factor, mana_factor,
                             std::move(stats));
            info_races.emplace(type, std::move(race));
        }
    } catch (const toml::parse_error& err) {
        std::string mssgErr = "Error en loadRaces -- No se pudo parsear el archivo TOML";
        mssgErr += err.description();
        throw std::runtime_error(mssgErr);
    }
}

void GameConfigLoader::loadClases(std::unordered_map<TypeClase, Clase>& info_clases) {
    try {
        Table config = toml::parse_file(paths.clases.string());
        Table_array* clases_array = config["clases"].as_array();
        if (!clases_array) {
            throw std::runtime_error("No se encontro el arreglo [[clases]] en el archivo TOML.");
        }
        for (auto&& node: *clases_array) {
            Table& clase_table = *node.as_table();
            auto type = static_cast<TypeClase>(clase_table["id_type"].value_or(0));
            auto name = clase_table["name"].value_or(std::string{});
            auto hp_factor = static_cast<uint16_t>(clase_table["life_factor"].value_or(1));
            auto meditation_factor =
                    static_cast<uint16_t>(clase_table["metitation_factor"].value_or(1));
            auto mana_factor = static_cast<uint16_t>(clase_table["mana_factor"].value_or(0));

            Table_array* stats_array = clase_table["statistics"].as_array();
            Statistics stats = this->loadTableStatistics(stats_array);
            Clase clase = Clase(type, std::move(name), hp_factor, meditation_factor, mana_factor,
                                std::move(stats));

            info_clases.emplace(type, std::move(clase));
        }
    } catch (const toml::parse_error& err) {
        std::string mssgErr = "Error en loadClases -- No se pudo parsear el archivo TOML";
        mssgErr += err.description();
        throw std::runtime_error(mssgErr);
    }
}

const FileData GameConfigLoader::getFilesData() { return this->data; }

void GameConfigLoader::loadItems(std::map<TypeItem, std::unique_ptr<Item>>& info_items) {
    Table config = toml::parse_file(paths.items.string());
    Table_array* items_array = config.get_as<toml::array>("items");
    if (!items_array) {
        throw std::runtime_error("Error: falta array [[items]] en el archivo TOML.");
    }

    for (const auto& item_node: *items_array) {
        const Table& item = *item_node.as_table();

        auto name = item["name"].value_or(std::string{});
        auto descp = item["description"].value_or(std::string{});
        auto type = static_cast<TypeItem>(item["id_type"].value_or(0));
        auto body = static_cast<BodyPart>(item["body_part"].value_or(0));
        auto classif = static_cast<ItemClassification>(item["classif"].value_or(0));

        auto sell_price = static_cast<uint16_t>(item["selling_price"].value_or(0));
        auto purch_price = static_cast<uint16_t>(item["purchase_price"].value_or(0));

        auto min_dam = static_cast<uint16_t>(item["minimal_damage"].value_or(0));
        auto max_dam = static_cast<uint16_t>(item["maximun_damage"].value_or(0));

        auto min_def = static_cast<uint16_t>(item["minimal_defense"].value_or(0));
        auto max_def = static_cast<uint16_t>(item["maximun_defense"].value_or(0));

        auto mana_cost = static_cast<uint16_t>(item["mana_cost"].value_or(0));
        auto range = static_cast<uint16_t>(item["range"].value_or(0));

        if (classif == ITEM_ATTACK) {
            if (descp == "MELEE_WEAPON") {
                info_items[type] =
                        std::make_unique<Weapon>(type, body, classif, std::move(name), sell_price,
                                                 purch_price, min_dam, max_dam, range);
            } else if (descp == "MAGICAL") {
                info_items[type] = std::make_unique<MagicWeapon>(
                        type, body, classif, std::move(name), sell_price, purch_price, min_dam,
                        max_dam, mana_cost, range);
            }
        } else if (classif == ITEM_DEFENSIVE) {
            info_items[type] = std::make_unique<Defense>(type, body, classif, std::move(name),
                                                         sell_price, purch_price, min_def, max_def);
        } else if (classif == ITEM_HEALING) {
            if (descp == "POTION") {
                info_items[type] = std::make_unique<ShopItem>(type, body, classif, std::move(name), sell_price, purch_price);
            } else if (descp == "MAGICAL") {
                info_items[type] =
                        std::make_unique<ObjectMagic>(type, body, classif, std::move(name),
                                                      sell_price, purch_price, mana_cost, range);
            }
        }
    }
}


void GameConfigLoader::loadRegions(std::map<Region, std::unique_ptr<RegionWorld>> &info_regions) {
    Table config = toml::parse_file(paths.regions.string());
    Table_array* regions_array = config["regions"].as_array();
    for (const auto& region_node: *regions_array) {
        const Table& region = *region_node.as_table();

        bool is_safe = region["is_safe_zone"].value_or(false);
        Region type = static_cast<Region>(region["id_type"].value_or(0));
        if (is_safe) {
            auto r = std::make_unique<SafeRegion>();
            r->type = type;
            r->merchants = region["merchants"].value_or(0);
            r->priests   = region["priest"].value_or(0);
            r->bankers   = region["bankers"].value_or(0);
            info_regions.emplace(type, std::move(r));
        } else {
            auto r = std::make_unique<WildRegion>();
            r->type = type;
            r->max_creatures = region["max_criatures"].value_or(0);
            if (region["min_treasure"].value<int>())
                r->min_treasure = region["min_treasure"].value_or(0);
            if (region["max_treasure"].value<int>())
                r->max_treasure = region["max_treasure"].value_or(0);
            info_regions.emplace(type, std::move(r));
        }
        auto& r = info_regions.at(type);
        if (auto* npcs = region["id_types_npcs"].as_array()) {
            npcs->for_each([&r](auto& npc) {
                if (npc.is_string()) {
                    r->npc_types.push_back(npc.as_string()->get());
                }
            });
        }
    }
}

GameConfig GameConfigLoader::getdGameConfiguration() {
    try {
        Table config = toml::parse_file(paths.game.string());

        /*Estado inicial del jugador*/
        auto player_init = config["player_state_init"];
        PlayerStateInitConfig state_init;
        state_init.level = static_cast<uint8_t>(player_init["level"].value_or(0));
        state_init.golden_init = static_cast<uint32_t>(player_init["golden_init"].value_or(0));
        state_init.max_inventory = static_cast<uint32_t>(player_init["max_inventory"].value_or(0));

        /*Condiciones de un clan*/
        auto clan_info = config["clan"];
        ClanConfig clan_conf;
        clan_conf.max_members = static_cast<uint32_t>(clan_info["max_members"].value_or(10));

        /*Tiempos del juego*/
        auto times_info = config["times"];
        TimesConfig times;
        times.server_update_frecuency = static_cast<uint32_t>(times_info["server_update_frecuency"].value_or(30));
        times.update_player_atributes = static_cast<uint32_t>(times_info["update_player_atributes"].value_or(500));
        times.spawn_npcs = static_cast<uint32_t>(times_info["spawn_npcs"].value_or(3000));
        times.disappear_dropped_item = static_cast<uint32_t>(times_info["disappear_dropped_item"].value_or(60000));
        times.pesistence_data = static_cast<uint32_t>(times_info["pesist_data"].value_or(60000));
        times.npc_attack_cooldown = static_cast<uint32_t>(times_info["npc_attack_cooldown"].value_or(1000));

        /*Informacion de los npcs*/
        std::unordered_map<TypeNPC, NPCConfig> npcs;
        this->loadNpcs(npcs);

        GameConfig game_config;
        game_config.player_init = state_init;
        game_config.clan = clan_conf;
        game_config.times = times;
        game_config.npcs = std::move(npcs);
        return game_config;

    } catch (const toml::parse_error& err) {
        std::string mssgErr =
                "Error en loadGameConfiguration -- No se pudo parsear el archivo TOML";
        mssgErr += err.description();
        throw std::runtime_error(mssgErr);
    }
}
