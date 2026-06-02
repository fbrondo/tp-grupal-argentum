#pragma once

enum Region {
    Cavern = 1,
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
