#include "client/includes/core/asset_catalog_loader.h"

#include <iostream>
#include <string>

#include <toml++/toml.hpp>

std::vector<AssetEntry> load_asset_catalog(const std::filesystem::path& toml_path) {
    std::vector<AssetEntry> result;

    toml::table table;
    try {
        table = toml::parse_file(toml_path.string());
    } catch (const toml::parse_error& err) {
        std::cerr << "[AssetCatalog] Error al parsear " << toml_path << ": " << err.what()
                  << std::endl;
        return result;
    }

    for (auto& [section_key, section_val]: table) {
        if (!section_val.is_table())
            continue;

        const auto& section = *section_val.as_table();

        std::string dir;
        if (auto* d = section["dir"].as_string())
            dir = d->get();

        std::string prefix;
        if (auto* p = section["prefix"].as_string())
            prefix = p->get();

        int section_fw = section["frame_width"].value_or(0);
        int section_fh = section["frame_height"].value_or(0);
        int section_fpr = section["frames_per_row"].value_or(0);

        auto* entries = section["entries"].as_array();
        if (!entries)
            continue;

        for (auto& entry_val: *entries) {
            if (!entry_val.is_table())
                continue;

            const auto& entry = *entry_val.as_table();
            AssetEntry asset;
            if (auto* id_str = entry["id"].as_string()) {
                asset.string_id = id_str->get();
                asset.id = 0;
            } else {
                asset.id = static_cast<uint16_t>(entry["id"].value_or(0));
            }
            asset.prefix = prefix;
            asset.filepath = dir + "/" + std::string(entry["file"].value_or(""));
            asset.frame_width = entry["frame_width"].value_or(section_fw);
            asset.frame_height = entry["frame_height"].value_or(section_fh);
            asset.frames_per_row = entry["frames_per_row"].value_or(section_fpr);

            result.push_back(std::move(asset));
        }
    }

    return result;
}
