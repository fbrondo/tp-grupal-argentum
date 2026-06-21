#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common/includes/core/character_traits.h"
#include "common/includes/core/player_stats.h"
#include "common/includes/core/position.h"
#include "common/includes/protocol.h"
#include "common/includes/types.h"

constexpr uint8_t PLAYER_FLAG_GHOST = 1 << 0;

#pragma pack(push, 1)
struct PlayerSnapshotData {
    char name[MAX_NAME_SIZE];
    uint32_t id;
    // Posicion en el mapa
    Position position;
    // uint32_t pos_x;
    // uint32_t pos_y;
    uint8_t direction;  // Hacia donde mira

    PlayerStats stats;
    CharacterTraits ch_traits;
    // uint16_t body_id;
    // uint16_t head_id;
    // uint8_t raza;
    // uint8_t clase;
    /*equipo*/
    uint8_t weapon_id;
    uint8_t shield_id; /*Debe ser none si no esta equipado*/
    uint8_t helmet_id;
    uint8_t armor_id;

    uint8_t flags;  // Estados especiales (por ej: bit 0 = invisible, bit 1 = meditando, etc.)
    uint16_t resurrection_time_left_ms;
};
struct CitizenNpcSnapshot {
    char name[MAX_NAME_SIZE];
    uint32_t id;
    uint8_t type;
    Position position;
    uint8_t direction;
};

struct NpcSnapshotData {
    char name[MAX_NAME_SIZE];
    uint32_t id;
    uint8_t type_id;
    Position position;
    // uint32_t pos_x;
    // uint32_t pos_y;
    uint8_t direction;
    uint16_t current_hp;
    uint16_t max_hp;
};

struct ItemGroundSnapshotData {
    uint8_t item_id;
    Position position;
    // uint32_t pos_x;
    // uint32_t pos_y;
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

struct VisualEffectSnapshotData {
    VisualEffectID effect_id;
    uint32_t recipient_id;  // 0 para broadcast; de otro modo, id del jugador destinatario
    uint32_t pos_x;
    uint32_t pos_y;
};

struct Snapshot {
    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items_on_floor;
    // std::vector<GoldPileGroundSnapshotData> gold_piles;
    std::vector<SoundEffectSnapshotData> sound_effects;
    std::vector<VisualEffectSnapshotData> visual_effects;
};
#pragma pack(pop)
