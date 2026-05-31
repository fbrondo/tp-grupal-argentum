#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <filesystem>
#include <map> 
#include <memory>

#include "server/includes/core/item.h"
#include "server/includes/core/config.h"
#include "server/includes/definitions.h"

class GameConfig {
    PathConfig path_conf;
private:
    const Path config_dir;
   
    void loadPaths();

public:
    GameConfig(const GameConfig& other) = delete;
    GameConfig& operator=(const GameConfig& other) = delete;
    
    GameConfig(const Path config_dir_);  
    
    const PathConfig getPaths();
    std::map<TypeItem, std::unique_ptr<Item>>&& loadItems();
};



#endif
