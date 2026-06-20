#include "client/includes/font_manager.h"

#define FONT_PATH "client/assets/Fonts/"

FontManager::FontManager():
        console_font(FONT_PATH "CinzelBold.ttf", 14),
        console_input_font(FONT_PATH "DejaVuSans-Bold.ttf", 14),
        bubble_font(FONT_PATH "Timeless-Bold.ttf", 13),
        level_font(FONT_PATH "DejaVuSans-Bold.ttf", 13),
        name_font(FONT_PATH "Augusta.ttf", 21),
        hud_font(FONT_PATH "OldLondon.ttf", 35) {}

SDL2pp::Font& FontManager::get_console_font() { return console_font; }

SDL2pp::Font& FontManager::get_console_input_font() { return console_input_font; }

SDL2pp::Font& FontManager::get_bubble_font() { return bubble_font; }

SDL2pp::Font& FontManager::get_level_font() { return level_font; }

SDL2pp::Font& FontManager::get_name_font() { return name_font; }

SDL2pp::Font& FontManager::get_hud_font() { return hud_font; }
