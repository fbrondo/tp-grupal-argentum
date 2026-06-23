
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct AssetEntry {
    uint16_t id;
    std::string string_id;
    std::string prefix;
    std::string filepath;
    int frame_width;
    int frame_height;
    int frames_per_row;
};

std::vector<AssetEntry> load_asset_catalog(const std::filesystem::path& toml_path);
