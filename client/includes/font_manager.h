#pragma once

#include <SDL2pp/Font.hh>

class FontManager {
private:
    SDL2pp::Font console_font;
    SDL2pp::Font console_input_font;
    SDL2pp::Font bubble_font;
    SDL2pp::Font level_font;
    SDL2pp::Font name_font;
    SDL2pp::Font hud_font;

public:
    FontManager();

    SDL2pp::Font& get_console_font();
    SDL2pp::Font& get_console_input_font();
    SDL2pp::Font& get_bubble_font();
    SDL2pp::Font& get_level_font();
    SDL2pp::Font& get_name_font();
    SDL2pp::Font& get_hud_font();
};
