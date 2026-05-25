#pragma once

#include <filesystem>
#include <unordered_map>

#include "common/includes/sprite_definition.h"

class SpriteLoader {
public:
    // Loads sprite definitions from a TOML file.
    static std::unordered_map<int, SpriteDefinition> load(const std::filesystem::path& filepath);
};
