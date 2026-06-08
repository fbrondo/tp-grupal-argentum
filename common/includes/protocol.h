#pragma once
#include <cstdint>
#include <vector>

#include "common//includes/core/character_traits.h"

constexpr size_t MAX_NAME_SIZE = 30;

enum ClientOpcode : uint8_t {
    LOGIN = 1,
    MOVE = 2,
    ATTACK = 3,
    CHAT = 4,
    USE_ITEM = 5,
    DROP_ITEM = 6,
    TAKE_ITEM = 7,
    COMMAND = 8,  // Meditar, Curar, Resucitar
    INTERACT = 9,
    BUY_ITEM = 10,
    SELL_ITEM = 11,
    DISCONNECT = 12,
    SIGNUP = 13,
    CHARACTER_CREATE = 15,
    DEPOSIT_ITEM = 16,
    WITHDRAW_ITEM = 17,
    DEPOSIT_GOLD = 18,
    WITHDRAW_GOLD = 19,
    LIST_ITEMS = 20,
    EQUIP_ITEM = 21,
    UNEQUIP_ITEM = 22,
    RESURRECT = 23
};

enum ServerOpcode : uint8_t {
    LOGIN_RESPONSE = 50,
    SNAPSHOT = 51,
    CHANGE_MAP = 52,
    PLAYER_STATS = 53,
    INVENTORY_UPDATE = 54,
    CHAT_MSG = 55,
    ACTION_ERROR = 56,
    SIGNUP_RESPONSE = 57,
    CHARACTER_CREATE_RESPONSE = 59,
    MAP_DATA = 61,
    TRADER_CATALOG = 62,
    BANK_CONTENT = 63
};

#pragma pack(push, 1)

// Client message structs
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
    uint8_t opcode;  // USE_ITEM or DROP_ITEM
    uint32_t instance_id;
};

struct MsgItemInfo {
    uint32_t instance_id;
    uint8_t item_type;
};

struct MsgInteract {
    uint8_t opcode = INTERACT;
    uint32_t npc_id;
};

struct MsgTrade {
    uint8_t opcode;  // BUY_ITEM or SELL_ITEM
    uint32_t npc_id;
    uint16_t item_id;
    uint16_t quantity;
};

struct MsgPlayerStats {
    uint8_t opcode = PLAYER_STATS;
    uint32_t hp;
    uint32_t mana;
    uint32_t gold;
    uint32_t exp;
    uint8_t level;
};

struct MsgInventoryUpdate {
    uint8_t opcode = INVENTORY_UPDATE;
    uint8_t slot_index;
    uint16_t item_id;
    uint16_t quantity;
    uint8_t is_equipped;
};

struct MsgSignup {
    uint8_t opcode = SIGNUP;
    char user[MAX_NAME_SIZE];
    char password[MAX_NAME_SIZE];
    CharacterTraits traits;
};

// struct MsgCharacterCreate {
//     uint8_t opcode = CHARACTER_CREATE;
//     char name[MAX_NAME_SIZE];
//     uint8_t race;
//     uint8_t clase;
// };

#pragma pack(pop)
