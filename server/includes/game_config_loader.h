#ifndef GAME_CONFIG_LOADER_H
#define GAME_CONFIG_LOADER_H

#include <filesystem>
#include <map> 
#include <memory>
#include <toml++/toml.hpp>
#include "server/includes/core/race.h"
#include "server/includes/core/item.h"
#include "server/includes/core/config.h"
#include "server/includes/definitions.h"

using Table = toml::table;
using Table_array = toml::array;

class GameConfigLoader {
private:
    static constexpr const char* FILE_PATHS = "paths.toml";
    const Path config_dir;
    PathsConfig paths;
    FileData data;

    void loadPaths();
    //void loadFilesData();
    Statistics loadTableStatistics(Table_array* stats_array);
    Path loadPath(const Table& config, const std::string& section_key, const std::string& fiel_key) const;


public:
    GameConfigLoader(const GameConfigLoader& other) = delete;
    GameConfigLoader& operator=(const GameConfigLoader& other) = delete;

    GameConfigLoader(Path config_dir_);

    const FileData getFilesData();
    void loadRaces(std::unordered_map<TypeRace, Race>& info_races);
    void loadClases(std::unordered_map<TypeClase, Clase>& info_clases);
    void loadItems(std::map<TypeItem, std::unique_ptr<Item>>& info_items);
    GameConfig getdGameConfiguration();
};



#endif
