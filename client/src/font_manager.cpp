#include "client/includes/font_manager.h"

#include <string>

#include "client/includes/core/constants.h"

FontManager::FontManager():
        console_font(std::string(FONT_PATH) + "Timeless.ttf", 14),
        console_input_font(std::string(FONT_PATH) + "Timeless-Bold.ttf", 14),
        bubble_font(std::string(FONT_PATH) + "Timeless-Bold.ttf", 13),
        level_font(std::string(FONT_PATH) + "DejaVuSans-Bold.ttf", 13),
        name_font(std::string(FONT_PATH) + "Augusta.ttf", 21),
        hud_font(std::string(FONT_PATH) + "OldLondon.ttf", 35),
        stats_font(std::string(FONT_PATH) + "CinzelBold.ttf", 14),
        npc_name_font(std::string(FONT_PATH) + "Augusta.ttf", 18) {}

SDL2pp::Font& FontManager::get_console_font() { return console_font; }

SDL2pp::Font& FontManager::get_console_input_font() { return console_input_font; }

SDL2pp::Font& FontManager::get_bubble_font() { return bubble_font; }

SDL2pp::Font& FontManager::get_level_font() { return level_font; }

SDL2pp::Font& FontManager::get_name_font() { return name_font; }

SDL2pp::Font& FontManager::get_hud_font() { return hud_font; }

SDL2pp::Font& FontManager::get_stats_font() { return stats_font; }

SDL2pp::Font& FontManager::get_npc_name_font() { return npc_name_font; }
