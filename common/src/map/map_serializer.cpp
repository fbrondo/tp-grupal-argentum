#include "common/includes/map/map_serializer.h"

#include <stdexcept>
#include <string>

#include "common/includes/toml_config.h"

Map MapSerializer::load(const std::filesystem::path& filepath) {
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

        if (sprite_ids.size() != expected || walkable.size() != expected) {
            throw std::runtime_error(std::string("tile count mismatch for layer: ") +
                                     layer_names_[i]);
        }

        auto& tiles = map.layer_tiles(layers_[i]);
        for (size_t j = 0; j < expected; ++j) {
            tiles[j].sprite_id = sprite_ids[j];
            tiles[j].walkable = walkable[j];
        }
    }

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
        sprite_ids.reserve(tiles.size());
        walkable.reserve(tiles.size());

        for (const auto& tile: tiles) {
            sprite_ids.push_back(tile.sprite_id);
            walkable.push_back(tile.walkable);
        }

        cfg.set_array<int>(prefix + ".sprite_ids", sprite_ids);
        cfg.set_array<bool>(prefix + ".walkable", walkable);
    }

    cfg.save(filepath);
}
