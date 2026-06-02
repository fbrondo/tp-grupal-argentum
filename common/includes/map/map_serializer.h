#pragma once
#include <array>
#include <filesystem>

#include "common/includes/map/layer.h"
#include "common/includes/map/map.h"

class MapSerializer {
public:
    static Map load(const std::filesystem::path& filepath);
    static void save(const Map& map, const std::filesystem::path& filepath);

private:
    static constexpr std::array<const char*, layer_count> layer_names_ = {"background", "details",
                                                                          "object", "roof"};
    static constexpr std::array<Layer, layer_count> layers_ = {Layer::Background, Layer::Details,
                                                               Layer::Object, Layer::Roof};
};
