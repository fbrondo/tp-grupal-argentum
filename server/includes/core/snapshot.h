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
    /*equipo*/
    uint8_t weapon_id;
    uint8_t shield_id; /*Debe ser none si no esta equipado*/
    uint8_t armor_id;
    uint8_t head;

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

struct GoldPileGroundSnapshotData {
    uint32_t id;
    uint32_t amount;
    uint32_t pos_x;
    uint32_t pos_y;
};

struct SoundEffectSnapshotData {
    SoundEffectID effect_id;
    uint32_t pos_x;  // Coordenada X donde ocurrio (para audio posicional)
    uint32_t pos_y;  // Coordenada Y
};
struct Snapshot {
    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items_on_floor;
    std::vector<GoldPileGroundSnapshotData> gold_piles;
    std::vector<SoundEffectSnapshotData> sound_effects;
};
#pragma pack(pop)
