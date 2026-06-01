#pragma once
#include <cstdint>

enum class Layer : uint8_t {
    Background = 0,
    Details = 1,
    Object = 2,
    Roof = 3,
};

constexpr uint8_t layer_count = 4;
