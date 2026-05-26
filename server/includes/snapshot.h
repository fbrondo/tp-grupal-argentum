#ifndef SNAPSHOT_H
#define SNAPSHOT_H
//#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PlayerSnapshotData {
    uint32_t id;

    // Posicion en el mapa
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t direction;  // Hacia donde mira

    uint16_t hp;
    uint16_t max_hp;
    uint16_t mana;
    uint16_t max_mana;

    uint16_t body_id;
    uint16_t head_id;
    uint16_t weapon_id;

    uint8_t raza;
    uint8_t clase;
    uint8_t flags;  // Estados especiales (por ej: bit 0 = invisible, bit 1 = meditando, etc.)
};

struct NpcSnapshotData {
    uint32_t id;
    uint16_t type_id;
    float x;
    float y;
    uint16_t hp_actual;
    bool is_alive;
};

struct ItemGroundSnapshotData {
    uint16_t item_id;
    float x;
    float y;
};

// No estoy seguro aun como manejar los efectos visuales
struct VisualEffect {
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t effect_type;  // Sonido / Animación de explosión / etc
};


struct Snapshot {
    uint32_t turn_number;  // Turno actual

    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items_on_floor;

    std::vector<VisualEffect> effects;
};

#endif
