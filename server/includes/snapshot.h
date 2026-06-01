#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common/includes/types.h"

#pragma pack(push, 1)

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
    uint32_t pos_x;
    uint32_t pos_y;
    uint16_t hp_actual;
    uint8_t is_alive;  // bool como uint8_t para serialización segura
};

struct ItemGroundSnapshotData {
    uint16_t item_id;
    uint32_t pos_x;
    uint32_t pos_y;
};

// No estoy seguro aun como manejar los efectos visuales
struct VisualEffect {
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t effect_type;
};

struct SoundEffectSnapshotData {
    SoundEffectID effect_id;
    uint32_t pos_x;  // Coordenada X donde ocurrio (para audio posicional)
    uint32_t pos_y;  // Coordenada Y
};

#pragma pack(pop)

struct Snapshot {
    uint32_t turn_number;  // Turno actual

    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items_on_floor;

    std::vector<VisualEffect> effects;
    std::vector<SoundEffectSnapshotData> sound_effects;
};
