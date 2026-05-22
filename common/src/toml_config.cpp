#include "common/includes/toml_config.h"

#include <fstream>
#include <stdexcept>
#include <string>

TomlConfig::TomlConfig(const std::filesystem::path& path):
        table_(toml::parse_file(path.string())) {}

bool TomlConfig::has(std::string_view key) const { return static_cast<bool>(table_.at_path(key)); }

void TomlConfig::save(const std::filesystem::path& filepath) const {
    std::ofstream ofs(filepath);
    if (!ofs)
        throw std::runtime_error("failed to open file for writing: " + filepath.string());
    ofs << table_;
}

const toml::table& TomlConfig::data() const { return table_; }
