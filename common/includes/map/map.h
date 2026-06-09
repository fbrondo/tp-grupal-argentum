#pragma once
#include <array>
#include <string>
#include <vector>

#include "common/includes/map/layer.h"
#include "common/includes/map/tile.h"

constexpr int MAX_MAP_WIDTH = 1000;
constexpr int MAX_MAP_HEIGHT = 1000;

class Map {
public:
    Map(const Map&) = default;
    Map& operator=(const Map&) = default;
    
    Map(Map&&) = default;
    Map& operator=(Map&&) = default;

    Map(): width_(0), height_(0) {}
    Map(std::string name, int width, int height);

    Tile& tile_at(int x, int y, Layer layer);
    const Tile& tile_at(int x, int y, Layer layer) const;

    const std::string& name() const;
    int width() const;
    int height() const;

    std::vector<Tile>& layer_tiles(Layer layer);
    const std::vector<Tile>& layer_tiles(Layer layer) const;

private:
    std::string name_;
    int width_;
    int height_;
    std::array<std::vector<Tile>, layer_count> layers_;

    size_t index(int x, int y) const;
};
