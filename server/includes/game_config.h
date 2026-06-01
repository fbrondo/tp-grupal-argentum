#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <filesystem>
#include <map> 
#include <memory>
#include <toml++/toml.hpp>

#include "server/includes/core/item.h"
#include "server/includes/core/config.h"
#include "server/includes/definitions.h"

using Table = toml::table;
class GameConfig {
private:
    static constexpr const char* RUTAS_FILE = "rutas.toml";

    const Path config_dir;
    PathConfig path_conf;
    Path loadPath(const Table& config, const std::string& setion_key, const std::string& fiel_key) const;
    void loadPaths();

public:
    GameConfig(const GameConfig& other) = delete;
    GameConfig& operator=(const GameConfig& other) = delete;
    
    GameConfig(Path config_dir_);

    PathConfig getPaths();
    std::map<TypeItem, std::unique_ptr<Item>> loadItems();
};



#endif
