#include "server/includes/game_config.h"
#include "common/includes/types.h"
#include <toml++/toml.hpp>
#include <stdexcept>
#include <string>

GameConfig::GameConfig(const Path config_dir_):config_dir(config_dir) {}

void GameConfig::loadPaths() {
    
    Path path = this->config_dir / "rutas.toml";
    toml::table config = toml::parse_file(path.string());
    
    this->path_conf.players_data = config["persistence_players"]["data_file"].value_or("players.dat");
    this->path_conf.player_indx = config["persistence_players"]["data_indx"].value_or("players.idx");
    this->path_conf.world_data = config["persistence_world"]["data_world"].value_or("wordl.dat");
    this->path_conf.map_path = config["map"]["path"].value_or("LANZAR EXCEPTCION");
}

const PathConfig GameConfig::getPaths() {
    return this->path_conf;
}

std::map<TypeItem, std::unique_ptr<Item>>&& GameConfig::loadItems() {
    std::map<TypeItem, std::unique_ptr<Item>> items;

    Path path = this->config_dir / "items.toml";
    toml::table config = toml::parse_file(path.string());
    auto* items_array = config.get_as<toml::array>("items");
    if(!items_array) {
        throw std::runtime_error("Error - items.toml: falta array [[items]]");
    }

    for (const auto& item_node: *items_array) {
        const toml::table& item = *item_node.as_table();

        auto name = item["name"].value_or(std::string{});
        auto descp = item["description"].value_or(std::string{});
        auto type = static_cast<TypeItem>(item["idItem"].value_or(0));
        auto body = static_cast<BodyPart>(item["body_part"].value_or(0));
        auto classif = static_cast<ItemClassification >(item["classif"].value_or(0));

        auto sell_price = static_cast<uint16_t>(item["selling_price"].value_or(0));
        auto purch_price = static_cast<uint16_t>(item["purchase_price"].value_or(0));

        auto min_dam =  static_cast<uint16_t>(item["minimal_damage"].value_or(0));
        auto max_dam = static_cast<uint16_t>(item["maximun_damage"].value_or(0));

        auto min_def =  static_cast<uint16_t>(item["minimal_defense"].value_or(0));
        auto max_def = static_cast<uint16_t>(item["maximun_defense"].value_or(0));

        auto mana_cost = static_cast<uint16_t>(item["mana_cost"].value_or(0));
        
        auto range = static_cast<uint16_t>(item["range"].value_or(0));

        if(type == ITEM_ATTACK) {
            if(descp == "MELEE_WEAPON") {
                items[type] = std::make_unique<Weapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam);
            } else if(descp == "MAGICAL") {
                items[type] = std::make_unique<MagicWeapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam, mana_cost, range);
            } else if(descp == "RANGED_WEAPON") {
                items[type] = std::make_unique<MagicWeapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam, range);
            }   
        } else if(type == ITEM_DEFENSIVE) {
            items[type] = std::make_unique<Defense>(type, body, classif, std::move(name), sell_price, purch_price, min_def, max_def);
        } else if(type == ITEM_HEALING) {
             if(descp == "POTION") {
                items[type] = std::make_unique<Item>(type, body, classif, std::move(name), sell_price, purch_price);
            } else if(descp == "MAGICAL") {
                items[type] = std::make_unique<ObjectMagic>(type, body, classif, std::move(name), sell_price, purch_price, mana_cost, range);
            }
        }
    }
    return std::move(items);
}
