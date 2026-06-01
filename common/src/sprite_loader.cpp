#include "common/includes/sprite_loader.h"

#include <stdexcept>
#include <string>

#include "common/includes/toml_config.h"

std::unordered_map<int, SpriteDefinition> SpriteLoader::load(
        const std::filesystem::path& filepath) {
    TomlConfig cfg(filepath);

    auto ids = cfg.get_array<int>("sprites.ids");
    auto png_ids = cfg.get_array<int>("sprites.png_ids");
    auto x = cfg.get_array<int>("sprites.x");
    auto y = cfg.get_array<int>("sprites.y");
    auto widths = cfg.get_array<int>("sprites.widths");
    auto heights = cfg.get_array<int>("sprites.heights");

    // TODO: Move this to a validation function
    const size_t sprites_len = ids.size();
    if (png_ids.size() != sprites_len || x.size() != sprites_len || y.size() != sprites_len ||
        widths.size() != sprites_len || heights.size() != sprites_len) {
        throw std::runtime_error(std::string("sprite array size mismatch in: ") +
                                 filepath.string());
    }

    std::unordered_map<int, SpriteDefinition> result;
    result.reserve(sprites_len);
    for (size_t i = 0; i < sprites_len; ++i) {
        result[ids[i]] = {png_ids[i], x[i], y[i], widths[i], heights[i]};
    }

    return result;
}
