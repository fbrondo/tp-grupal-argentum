#pragma once

enum class Region {
    Cavern,
    Dungeon,
    Forest,
    Desert,
    Field,
    City,
    Town,
};

struct Tile {
    int sprite_id{0};
    bool walkable{true};
    Region region{Region::Field};
};
