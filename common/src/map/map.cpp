#include "common/includes/map/map.h"

#include <stdexcept>

Map::Map(std::string name, int width, int height):
        name_(std::move(name)), width_(width), height_(height) {
    if (width_ <= 0 || height_ <= 0)
        throw std::invalid_argument("map dimensions must be positive");
    if (width_ > MAX_MAP_WIDTH || height_ > MAX_MAP_HEIGHT)
        throw std::invalid_argument("map dimensions exceed maximum allowed");
    for (auto& layer: layers_) {
        layer.resize(static_cast<size_t>(width_ * height_));
    }
}

Tile& Map::tile_at(int x, int y, Layer layer) {
    return layers_[static_cast<size_t>(layer)][index(x, y)];
}

const Tile& Map::tile_at(int x, int y, Layer layer) const {
    return layers_[static_cast<size_t>(layer)][index(x, y)];
}

const std::string& Map::name() const { return name_; }

int Map::width() const { return width_; }

int Map::height() const { return height_; }

std::vector<Tile>& Map::layer_tiles(Layer layer) { return layers_[static_cast<size_t>(layer)]; }

const std::vector<Tile>& Map::layer_tiles(Layer layer) const {
    return layers_[static_cast<size_t>(layer)];
}

size_t Map::index(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        throw std::out_of_range("tile coordinates out of map bounds");
    }
    return static_cast<size_t>(y * width_ + x);  // return index vector for (x, y) coord
}
