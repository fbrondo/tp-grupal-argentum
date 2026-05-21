#include "../includes/toml_config.h"

#include <string>

TomlConfig::TomlConfig(const std::filesystem::path& path):
        table_(toml::parse_file(path.string())) {}

bool TomlConfig::has(std::string_view key) const { return static_cast<bool>(table_.at_path(key)); }

const toml::table& TomlConfig::data() const { return table_; }
