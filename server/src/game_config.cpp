#include "server/includes/game_config.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <toml++/toml.hpp>
#include "common/includes/types.h"



GameConfig::GameConfig(Path config_dir_): config_dir(std::move(config_dir_)) {
    this->loadPaths();
}
Path GameConfig::loadPath(const Table& config, const std::string& setion_key, const std::string& field_key) const {
    try {
        std::string file_name = config[setion_key][field_key].value<std::string>().value();
        Path file_path(file_name);
        Path p = this->config_dir / file_path ;
        std::cout << "--- Cargando ruta: "<< p <<" ---" << std::endl;
        return p;
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en loadPath -- "; mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}
void GameConfig::loadPaths() {
    try {
        Path path = this->config_dir / RUTAS_FILE;
        std::cout << "--- Cargando rutas.tomls: "<< path <<" ---" << std::endl;
        const Table config = toml::parse_file(path.string());
        this->path_conf.players_data = this->loadPath(config, "persistence", "data_file");
        this->path_conf.players_indx = this->loadPath(config, "persistence", "data_indx");
        this->path_conf.world_data = this->loadPath(config, "persistence", "data_world");
        this->path_conf.map_path = this->loadPath(config, "persistence", "map");
    } catch (const toml::parse_error& err) {
        std::string mssgErr = "Error en loadPath -- No se pudo parsear el archivo TOML";
        mssgErr += err.description();
        throw std::runtime_error(mssgErr);
    }


    //this->path_conf.players_data = this->config_dir / config["persistence_players"]["data_file"].value<std::string>().value();
    //std::cout << "-- player_data: "<< path_conf.players_data << " --" << std::endl;
    //this->path_conf.players_indx =  this->config_dir / config["persistence_players"]["data_indx"].value<std::string>().value();
    //std::cout << "-- player_indx: "<< path_conf.players_indx << " --" << std::endl;
    //this->path_conf.world_data=  this->config_dir / config["persistence_world"]["data_world"].value<std::string>().value();
    //std::cout << "-- world_data: "<< path_conf.world_data << " --" << std::endl;
    //this->path_conf.map_path =  this->config_dir / config["map"]["path"].value<std::string>().value();
    //std::cout << "-- map_path: "<< path_conf.map_path << " --" << std::endl;
}

PathConfig GameConfig::getPaths() {
    return this->path_conf;
}

std::map<TypeItem, std::unique_ptr<Item>> GameConfig::loadItems() {
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

        if(classif == ITEM_ATTACK) {
            if(descp == "MELEE_WEAPON") {
                items[type] = std::make_unique<Weapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam);
            } else if(descp == "MAGICAL") {
                items[type] = std::make_unique<MagicWeapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam, mana_cost, range);
            } else if(descp == "RANGED_WEAPON") {
                items[type] = std::make_unique<RangedWeapon>(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam, range);
            }   
        } else if(classif == ITEM_DEFENSIVE) {
            items[type] = std::make_unique<Defense>(type, body, classif, std::move(name), sell_price, purch_price, min_def, max_def);
        } else if(classif == ITEM_HEALING) {
             if(descp == "POTION") {
                items[type] = std::make_unique<Item>(type, body, classif, std::move(name), sell_price, purch_price);
            } else if(descp == "MAGICAL") {
                items[type] = std::make_unique<ObjectMagic>(type, body, classif, std::move(name), sell_price, purch_price, mana_cost, range);
            }
        }
    }
    return items;
}
