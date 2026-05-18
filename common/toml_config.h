#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <utility>

#include <toml++/toml.hpp>

class TomlConfig {
public:
    explicit TomlConfig(const std::filesystem::path& path);

    // Get a value by dot-separated key path (e.g. "section.key").
    // Returns std::nullopt if the key is missing or the type doesn't match.
    template <typename T>
    std::optional<T> get(std::string_view key) const {
        return table_.at_path(key).value<T>();
    }

    // Get a value or return a default value if the key is missing or the type doesn't match.
    template <typename T>
    T get_or(std::string_view key, T default_value) const {
        return table_.at_path(key).value_or(std::move(default_value));
    }

    bool has(std::string_view key) const;

    const toml::table& data() const;

private:
    toml::table table_;
};
