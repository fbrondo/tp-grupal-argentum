#include "common/includes/protocol.h"

#include <cstring>

#include "gtest/gtest.h"

// Verifica que los opcodes del cliente tienen los valores correctos del protocolo
TEST(ProtocolOpcodes, ClientOpcodeValues) {
    EXPECT_EQ(LOGIN, 1);
    EXPECT_EQ(MOVE, 2);
    EXPECT_EQ(ATTACK, 3);
    EXPECT_EQ(DISCONNECT, 12);
    EXPECT_EQ(SIGNUP, 13);
}

// Verifica que los opcodes del servidor tienen los valores correctos del protocolo
TEST(ProtocolOpcodes, ServerOpcodeValues) {
    EXPECT_EQ(LOGIN_RESPONSE, 50);
    EXPECT_EQ(SNAPSHOT, 51);
    EXPECT_EQ(PLAYER_STATS, 53);
    EXPECT_EQ(INVENTORY_UPDATE, 54);
    EXPECT_EQ(EQUIPMENT_UPDATE, 64);
}

// MsgLogin debe ocupar exactamente 61 bytes con pack(1)
TEST(ProtocolSizes, MsgLoginSize) { EXPECT_EQ(sizeof(MsgLogin), 61u); }

// MsgMove debe ocupar exactamente 2 bytes
TEST(ProtocolSizes, MsgMoveSize) { EXPECT_EQ(sizeof(MsgMove), 2u); }

// MsgAttack debe ocupar exactamente 5 bytes (1 opcode + 4 target_id)
TEST(ProtocolSizes, MsgAttackSize) { EXPECT_EQ(sizeof(MsgAttack), 5u); }

// MsgInteract debe ocupar exactamente 6 bytes (1 + 4 + 1)
TEST(ProtocolSizes, MsgInteractSize) { EXPECT_EQ(sizeof(MsgInteract), 6u); }

// MsgTrade debe ocupar exactamente 8 bytes (1 + 4 + 1 + 2)
TEST(ProtocolSizes, MsgTradeSize) { EXPECT_EQ(sizeof(MsgTrade), 8u); }

// MsgPlayerStats debe ocupar exactamente 34 bytes (1 opcode + 8 campos uint32 + 1 level)
TEST(ProtocolSizes, MsgPlayerStatsSize) { EXPECT_EQ(sizeof(MsgPlayerStats), 34u); }

// MsgSignup debe ocupar exactamente 67 bytes (1 + 30 + 30 + 6 de CharacterTraits)
TEST(ProtocolSizes, MsgSignupSize) { EXPECT_EQ(sizeof(MsgSignup), 67u); }

// MsgLogin inicializa el opcode correctamente y el nombre se puede escribir/leer
TEST(ProtocolFields, MsgLoginNameRoundTrip) {
    MsgLogin msg{};
    EXPECT_EQ(msg.opcode, LOGIN);
    std::strncpy(msg.name, "jugador1", MAX_NAME_SIZE);
    std::strncpy(msg.pass, "clave123", MAX_NAME_SIZE);
    EXPECT_STREQ(msg.name, "jugador1");
    EXPECT_STREQ(msg.pass, "clave123");
}

// MsgPlayerStats: todos los campos de stats se escriben y leen correctamente
TEST(ProtocolFields, MsgPlayerStatsRoundTrip) {
    MsgPlayerStats msg{};
    EXPECT_EQ(msg.opcode, PLAYER_STATS);
    msg.hp = 100;
    msg.max_hp = 200;
    msg.mana = 50;
    msg.max_mana = 150;
    msg.safe_gold = 1000;
    msg.excess_gold = 500;
    msg.exp = 2500;
    msg.exp_next_level = 5000;
    msg.level = 5;

    EXPECT_EQ(msg.hp, 100u);
    EXPECT_EQ(msg.max_hp, 200u);
    EXPECT_EQ(msg.mana, 50u);
    EXPECT_EQ(msg.max_mana, 150u);
    EXPECT_EQ(msg.safe_gold, 1000u);
    EXPECT_EQ(msg.excess_gold, 500u);
    EXPECT_EQ(msg.exp, 2500u);
    EXPECT_EQ(msg.exp_next_level, 5000u);
    EXPECT_EQ(msg.level, 5u);
}
