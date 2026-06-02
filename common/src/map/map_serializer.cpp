#include "common/includes/map/map_serializer.h"
#include <iostream>
#include <stdexcept>
#include <string>

#include "common/includes/map/tile.h"
#include "common/includes/toml_config.h"

static std::string regionToString(Region region) {
    switch (region) {
        case Region::Cavern:
            return "cavern";
        case Region::Dungeon:
            return "dungeon";
        case Region::Forest:
            return "forest";
        case Region::Desert:
            return "desert";
        case Region::Field:
            return "field";
        case Region::City:
            return "city";
        case Region::Town:
            return "town";
    }
    return "field";
}

static Region regionFromString(const std::string& region_str) {
    if (region_str == "cavern")
        return Region::Cavern;
    if (region_str == "dungeon")
        return Region::Dungeon;
    if (region_str == "forest")
        return Region::Forest;
    if (region_str == "desert")
        return Region::Desert;
    if (region_str == "city")
        return Region::City;
    if (region_str == "town")
        return Region::Town;
    return Region::Field;
}

Map MapSerializer::load(const std::filesystem::path& filepath) {
    std::cout << "--- Serializando Map: "<< filepath <<" ---" << std::endl;
    TomlConfig cfg(filepath);

    std::string name = cfg.get_or<std::string>("metadata.name", "");
    int width = cfg.get_or<int>("metadata.width", 0);
    int height = cfg.get_or<int>("metadata.height", 0);

    Map map(std::move(name), width, height);
    size_t expected = static_cast<size_t>(width * height);

    for (size_t i = 0; i < layer_count; ++i) {
        std::string prefix = std::string("layer.") + layer_names_[i];

        auto sprite_ids = cfg.get_array<int>(prefix + ".sprite_ids");
        auto walkable = cfg.get_array<bool>(prefix + ".walkable");
        auto regions = cfg.get_array<std::string>(prefix + ".regions");

        if (sprite_ids.size() != expected || walkable.size() != expected) {
            throw std::runtime_error(std::string("tile count mismatch for layer: ") +
                                     layer_names_[i]);
        }

        auto& tiles = map.layer_tiles(layers_[i]);
        for (size_t j = 0; j < expected; ++j) {
            tiles[j].sprite_id = sprite_ids[j];
            tiles[j].walkable = walkable[j];
            tiles[j].region = (j < regions.size()) ? regionFromString(regions[j]) : Region::Field;
        }
    }
    std::cout << "--- Termino serializacion Map: "<< filepath <<" ---" << std::endl;
    return map;
}

void MapSerializer::save(const Map& map, const std::filesystem::path& filepath) {
    TomlConfig cfg;

    cfg.set<std::string>("metadata.name", map.name());
    cfg.set<int>("metadata.width", map.width());
    cfg.set<int>("metadata.height", map.height());

    for (size_t i = 0; i < layer_count; ++i) {
        const auto& tiles = map.layer_tiles(layers_[i]);
        std::string prefix = std::string("layer.") + layer_names_[i];

        std::vector<int> sprite_ids;
        std::vector<bool> walkable;
        std::vector<std::string> regions;
        sprite_ids.reserve(tiles.size());
        walkable.reserve(tiles.size());
        regions.reserve(tiles.size());

        for (const auto& tile: tiles) {
            sprite_ids.push_back(tile.sprite_id);
            walkable.push_back(tile.walkable);
            regions.push_back(regionToString(tile.region));
        }

        cfg.set_array<int>(prefix + ".sprite_ids", sprite_ids);
        cfg.set_array<bool>(prefix + ".walkable", walkable);
        cfg.set_array<std::string>(prefix + ".regions", regions);
    }

    cfg.save(filepath);
}
