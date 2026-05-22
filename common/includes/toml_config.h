#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <toml++/toml.hpp>

class TomlConfig {
public:
    TomlConfig() = default;
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

    // Get an array of values by dot-separated key path.
    // Returns empty vector if the key is missing or not an array.
    template <typename T>
    std::vector<T> get_array(std::string_view key) const {
        auto* arr = table_.at_path(key).as_array();
        if (!arr)
            return {};
        std::vector<T> result;
        result.reserve(arr->size());
        for (size_t i = 0; i < arr->size(); ++i) {
            if (auto val = (*arr)[i].template value<T>())
                result.push_back(*val);
        }
        return result;
    }

    // Set a scalar value by dot-separated key path, creating intermediate tables as needed.
    template <typename T>
    void set(std::string_view key, T value) {
        set_at_path(table_, key, std::move(value));
    }

    // Set an array of values by dot-separated key path.
    template <typename T>
    void set_array(std::string_view key, const std::vector<T>& values) {
        toml::array arr;
        // cppcheck-suppress useStlAlgorithm
        for (const auto& v: values) arr.push_back(v);
        set_at_path(table_, key, std::move(arr));
    }

    bool has(std::string_view key) const;

    void save(const std::filesystem::path& filepath) const;

    const toml::table& data() const;

private:
    toml::table table_;

    template <typename T>
    static void set_at_path(toml::table& tbl, std::string_view path, T value) {
        auto dot = path.find('.');
        if (dot == std::string_view::npos) {
            tbl.insert_or_assign(std::string(path), std::move(value));
            return;
        }
        std::string section(path.substr(0, dot));
        if (!tbl.contains(section) || !tbl[section].is_table())
            tbl.insert_or_assign(section, toml::table{});
        set_at_path(*tbl[section].as_table(), path.substr(dot + 1), std::move(value));
    }
};
