#pragma once

#include <cstdint>
#include <string>

#include "common/includes/types.h"

const char* item_name(TypeItem type);
std::string to_lower(const std::string& s);
uint8_t resolve_item_id(const std::string& name);
