#ifndef __OPCODE__
#define __OPCODE__

#include <cstdint>

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

#endif
