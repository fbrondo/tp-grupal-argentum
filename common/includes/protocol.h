#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <vector>
constexpr size_t MAX_NAME_SIZE = 30;

enum ClientOpcode : uint8_t {
    LOGIN = 1,
    MOVE = 2,
    ATTACK = 3,
    CHAT = 4,
    USE_ITEM = 5,
    DROP_ITEM = 6,
    TAKE_ITEM = 7,
    COMMAND = 8,
    INTERACT = 9,
    BUY_ITEM = 10,
    SELL_ITEM = 11
};

enum ServerOpcode : uint8_t {
    LOGIN_RESPONSE = 50,
    SNAPSHOT = 51,
    CHANGE_MAP = 52,
    PLAYER_STATS = 53,
    INVENTORY_UPDATE = 54,
    CHAT_MSG = 55,
    ACTION_ERROR = 56
};

#pragma pack(push, 1)

// Structs del Cliente
struct MsgLogin {
    uint8_t opcode = LOGIN;
    char name[MAX_NAME_SIZE];
    char pass[MAX_NAME_SIZE];
};

struct MsgMove {
    uint8_t opcode = MOVE;
    uint8_t direction;
};

struct MsgAttack {
    uint8_t opcode = ATTACK;
    uint32_t target_id;
};

struct MsgSlotItem {
    uint8_t opcode;  // Puede ser USE_ITEM o DROP_ITEM
    uint8_t slot_index;
};

struct MsgInteract {
    uint8_t opcode = INTERACT;
    uint32_t npc_id;
};

struct MsgTrade {
    uint8_t opcode;   // BUY_ITEM o SELL_ITEM
    uint32_t npc_id;  // ID del comerciante
    uint16_t item_id;
    uint16_t quantity;
};

// Structs del Servidor
struct PlayerSnapshotData {
    uint32_t id;
    float x;
    float y;
    uint8_t dir;
    uint16_t hp_max;
    uint16_t hp_actual;
    uint16_t body_id;
    uint16_t head_id;
    uint16_t weapon_id;
    uint8_t flags;
};

struct NpcSnapshotData {
    uint32_t id;
    uint16_t type_id;
    float x;
    float y;
    uint16_t hp_actual;
};

struct ItemGroundSnapshotData {
    uint16_t item_id;
    float x;
    float y;
};

struct MsgPlayerStats {
    uint8_t opcode = PLAYER_STATS;
    uint32_t hp;
    uint32_t mana;
    uint32_t gold;
    uint32_t exp;
    uint8_t level;
    uint8_t nivel;
};

struct MsgInventoryUpdate {
    uint8_t opcode = INVENTORY_UPDATE;
    uint8_t slot_index;
    uint16_t item_id;
    uint16_t quantity;
    uint8_t is_equipped;
};

#endif
