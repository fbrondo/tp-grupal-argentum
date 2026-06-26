#include "common/includes/protocol.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define private public
#include "common/includes/socket.h"
#undef private

#include "client/includes/client_protocol.h"
#include "common/includes/core/snapshot.h"
#include "common/includes/direction.h"
#include "common/includes/map/layer.h"
#include "common/includes/map/tile.h"
#include "gtest/gtest.h"

namespace {

template <typename T, size_t N>
std::array<uint8_t, N> bytesOf(const T& value) {
    static_assert(sizeof(T) == N);
    std::array<uint8_t, N> bytes{};
    std::memcpy(bytes.data(), &value, N);
    return bytes;
}

std::optional<std::pair<Socket, Socket>> makeSocketPair() {
    const int listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        return std::nullopt;
    }

    int reuse = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
        ::close(listener);
        return std::nullopt;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(listener);
        return std::nullopt;
    }
    if (listen(listener, 1) != 0) {
        ::close(listener);
        return std::nullopt;
    }

    socklen_t addr_len = sizeof(addr);
    if (getsockname(listener, reinterpret_cast<sockaddr*>(&addr), &addr_len) != 0) {
        ::close(listener);
        return std::nullopt;
    }

    const int client_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        ::close(listener);
        return std::nullopt;
    }
    if (connect(client_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(client_fd);
        ::close(listener);
        return std::nullopt;
    }

    const int server_fd = accept(listener, nullptr, nullptr);
    ::close(listener);
    if (server_fd < 0) {
        ::close(client_fd);
        return std::nullopt;
    }

    return std::optional<std::pair<Socket, Socket>>(std::in_place, Socket(client_fd),
                                                    Socket(server_fd));
}

#define MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP()                                       \
    auto socket_pair = makeSocketPair();                                          \
    if (!socket_pair) {                                                           \
        GTEST_SKIP() << "TCP sockets are not available in this test environment"; \
    }                                                                             \
    auto& [client_socket, peer_socket] = *socket_pair

void appendBytes(std::vector<uint8_t>& out, const void* data, size_t size) {
    const auto* begin = static_cast<const uint8_t*>(data);
    out.insert(out.end(), begin, begin + size);
}

void appendU8(std::vector<uint8_t>& out, uint8_t value) { out.push_back(value); }

void appendNetU16(std::vector<uint8_t>& out, uint16_t value) {
    const uint16_t net = htons(value);
    appendBytes(out, &net, sizeof(net));
}

void appendNetU32(std::vector<uint8_t>& out, uint32_t value) {
    const uint32_t net = htonl(value);
    appendBytes(out, &net, sizeof(net));
}

void appendText(std::vector<uint8_t>& out, const std::string& text) {
    appendNetU16(out, static_cast<uint16_t>(text.size()));
    appendBytes(out, text.data(), text.size());
}

std::vector<uint8_t> recvBytes(Socket& socket, size_t size) {
    std::vector<uint8_t> bytes(size);
    socket.recvall(bytes.data(), static_cast<unsigned int>(bytes.size()));
    return bytes;
}

}  // namespace

TEST(ProtocolConstants, MaxStringAndItemSizesArePartOfWireContract) {
    EXPECT_EQ(MAX_NAME_SIZE, 30u);
    EXPECT_EQ(MAX_ITEM_SIZE, 100u);
}

TEST(ProtocolOpcodes, ClientOpcodeValuesAreStable) {
    EXPECT_TRUE((std::is_same_v<std::underlying_type_t<ClientOpcode>, uint8_t>));

    EXPECT_EQ(LOGIN, 1);
    EXPECT_EQ(MOVE, 2);
    EXPECT_EQ(ATTACK, 3);
    EXPECT_EQ(CHAT, 4);
    EXPECT_EQ(USE_ITEM, 5);
    EXPECT_EQ(DROP_ITEM, 6);
    EXPECT_EQ(TAKE_ITEM, 7);
    EXPECT_EQ(COMMAND, 8);
    EXPECT_EQ(INTERACT, 9);
    EXPECT_EQ(BUY_ITEM, 10);
    EXPECT_EQ(SELL_ITEM, 11);
    EXPECT_EQ(DISCONNECT, 12);
    EXPECT_EQ(SIGNUP, 13);
    EXPECT_EQ(CHARACTER_CREATE, 15);
    EXPECT_EQ(DEPOSIT_ITEM, 16);
    EXPECT_EQ(WITHDRAW_ITEM, 17);
    EXPECT_EQ(DEPOSIT_GOLD, 18);
    EXPECT_EQ(WITHDRAW_GOLD, 19);
    EXPECT_EQ(LIST_ITEMS, 20);
    EXPECT_EQ(EQUIP_ITEM, 21);
    EXPECT_EQ(UNEQUIP_ITEM, 22);
    EXPECT_EQ(RESURRECT, 23);
}

TEST(ProtocolOpcodes, ServerOpcodeValuesAreStable) {
    EXPECT_TRUE((std::is_same_v<std::underlying_type_t<ServerOpcode>, uint8_t>));

    EXPECT_EQ(LOGIN_RESPONSE, 50);
    EXPECT_EQ(SNAPSHOT, 51);
    EXPECT_EQ(CHANGE_MAP, 52);
    EXPECT_EQ(PLAYER_STATS, 53);
    EXPECT_EQ(INVENTORY_UPDATE, 54);
    EXPECT_EQ(CHAT_MSG, 55);
    EXPECT_EQ(ACTION_ERROR, 56);
    EXPECT_EQ(SIGNUP_RESPONSE, 57);
    EXPECT_EQ(CHARACTER_CREATE_RESPONSE, 59);
    EXPECT_EQ(MAP_DATA, 61);
    EXPECT_EQ(TRADER_CATALOG, 62);
    EXPECT_EQ(BANK_CONTENT, 63);
    EXPECT_EQ(EQUIPMENT_UPDATE, 64);
}

TEST(ProtocolPrimitiveStructs, CharacterTraitsIsPacked) {
    EXPECT_EQ(sizeof(CharacterTraits), 6u);
    EXPECT_EQ(offsetof(CharacterTraits, head), 0u);
    EXPECT_EQ(offsetof(CharacterTraits, body), 2u);
    EXPECT_EQ(offsetof(CharacterTraits, race), 4u);
    EXPECT_EQ(offsetof(CharacterTraits, clase), 5u);

    CharacterTraits traits{0x1122, 0x3344, HUMAN, WIZARD};
    EXPECT_EQ(traits.head, 0x1122u);
    EXPECT_EQ(traits.body, 0x3344u);
    EXPECT_EQ(traits.race, HUMAN);
    EXPECT_EQ(traits.clase, WIZARD);
}

TEST(ProtocolPrimitiveStructs, PositionAndPlayerStatsArePacked) {
    EXPECT_EQ(sizeof(Position), 8u);
    EXPECT_EQ(offsetof(Position, x), 0u);
    EXPECT_EQ(offsetof(Position, y), 4u);

    EXPECT_EQ(sizeof(PlayerStats), 21u);
    EXPECT_EQ(offsetof(PlayerStats, level), 0u);
    EXPECT_EQ(offsetof(PlayerStats, xp), 1u);
    EXPECT_EQ(offsetof(PlayerStats, current_hp), 5u);
    EXPECT_EQ(offsetof(PlayerStats, max_hp), 7u);
    EXPECT_EQ(offsetof(PlayerStats, current_mana), 9u);
    EXPECT_EQ(offsetof(PlayerStats, max_mana), 11u);
    EXPECT_EQ(offsetof(PlayerStats, safe_gold), 13u);
    EXPECT_EQ(offsetof(PlayerStats, excess_gold), 17u);
}

TEST(ProtocolMessageSizes, ClientMessagesHaveExpectedPackedSizes) {
    EXPECT_EQ(sizeof(MsgLogin), 61u);
    EXPECT_EQ(sizeof(MsgMove), 2u);
    EXPECT_EQ(sizeof(MsgAttack), 5u);
    EXPECT_EQ(sizeof(MsgSlotItem), 5u);
    EXPECT_EQ(sizeof(MsgItemInfo), 5u);
    EXPECT_EQ(sizeof(MsgInteract), 6u);
    EXPECT_EQ(sizeof(MsgTrade), 8u);
    EXPECT_EQ(sizeof(MsgSignup), 67u);
}

TEST(ProtocolMessageSizes, ServerMessagesHaveExpectedPackedSizes) {
    EXPECT_EQ(sizeof(MsgPlayerStats), 34u);
    EXPECT_EQ(sizeof(MsgSlot), 4u);
}

TEST(ProtocolMessageLayout, LoginLayoutHasNoPadding) {
    EXPECT_EQ(offsetof(MsgLogin, opcode), 0u);
    EXPECT_EQ(offsetof(MsgLogin, name), 1u);
    EXPECT_EQ(offsetof(MsgLogin, pass), 31u);

    MsgLogin msg{};
    EXPECT_EQ(msg.opcode, LOGIN);
    std::strncpy(msg.name, "jugador1", sizeof(msg.name) - 1);
    std::strncpy(msg.pass, "clave123", sizeof(msg.pass) - 1);

    EXPECT_STREQ(msg.name, "jugador1");
    EXPECT_STREQ(msg.pass, "clave123");
}

TEST(ProtocolMessageLayout, MoveAndAttackLayoutsHaveNoPadding) {
    EXPECT_EQ(offsetof(MsgMove, opcode), 0u);
    EXPECT_EQ(offsetof(MsgMove, direction), 1u);
    EXPECT_EQ(offsetof(MsgAttack, opcode), 0u);
    EXPECT_EQ(offsetof(MsgAttack, target_id), 1u);

    MsgMove move{};
    move.direction = RIGHT;
    EXPECT_EQ(move.opcode, MOVE);
    EXPECT_EQ(move.direction, RIGHT);

    MsgAttack attack{};
    attack.target_id = 42;
    EXPECT_EQ(attack.opcode, ATTACK);
    EXPECT_EQ(attack.target_id, 42u);
}

TEST(ProtocolMessageLayout, ItemAndInteractionLayoutsHaveNoPadding) {
    EXPECT_EQ(offsetof(MsgSlotItem, opcode), 0u);
    EXPECT_EQ(offsetof(MsgSlotItem, instance_id), 1u);
    EXPECT_EQ(offsetof(MsgItemInfo, instance_id), 0u);
    EXPECT_EQ(offsetof(MsgItemInfo, item_type), 4u);
    EXPECT_EQ(offsetof(MsgInteract, opcode), 0u);
    EXPECT_EQ(offsetof(MsgInteract, npc_id), 1u);
    EXPECT_EQ(offsetof(MsgInteract, action), 5u);

    MsgSlotItem use_item{};
    use_item.opcode = USE_ITEM;
    use_item.instance_id = 7;
    EXPECT_EQ(use_item.opcode, USE_ITEM);
    EXPECT_EQ(use_item.instance_id, 7u);

    MsgInteract interact{};
    interact.npc_id = 99;
    interact.action = 1;
    EXPECT_EQ(interact.opcode, INTERACT);
    EXPECT_EQ(interact.npc_id, 99u);
    EXPECT_EQ(interact.action, 1u);
}

TEST(ProtocolMessageLayout, TradeLayoutHasNoPadding) {
    EXPECT_EQ(offsetof(MsgTrade, opcode), 0u);
    EXPECT_EQ(offsetof(MsgTrade, npc_id), 1u);
    EXPECT_EQ(offsetof(MsgTrade, item_id), 5u);
    EXPECT_EQ(offsetof(MsgTrade, quantity), 6u);

    MsgTrade trade{};
    trade.opcode = BUY_ITEM;
    trade.npc_id = 123;
    trade.item_id = SWORD;
    trade.quantity = 2;

    EXPECT_EQ(trade.opcode, BUY_ITEM);
    EXPECT_EQ(trade.npc_id, 123u);
    EXPECT_EQ(trade.item_id, SWORD);
    EXPECT_EQ(trade.quantity, 2u);
}

TEST(ProtocolMessageLayout, PlayerStatsLayoutHasNoPadding) {
    EXPECT_EQ(offsetof(MsgPlayerStats, opcode), 0u);
    EXPECT_EQ(offsetof(MsgPlayerStats, hp), 1u);
    EXPECT_EQ(offsetof(MsgPlayerStats, max_hp), 5u);
    EXPECT_EQ(offsetof(MsgPlayerStats, mana), 9u);
    EXPECT_EQ(offsetof(MsgPlayerStats, max_mana), 13u);
    EXPECT_EQ(offsetof(MsgPlayerStats, safe_gold), 17u);
    EXPECT_EQ(offsetof(MsgPlayerStats, excess_gold), 21u);
    EXPECT_EQ(offsetof(MsgPlayerStats, exp), 25u);
    EXPECT_EQ(offsetof(MsgPlayerStats, exp_next_level), 29u);
    EXPECT_EQ(offsetof(MsgPlayerStats, level), 33u);

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

TEST(ProtocolMessageLayout, SlotLayoutHasNoPadding) {
    EXPECT_EQ(offsetof(MsgSlot, slot_index), 0u);
    EXPECT_EQ(offsetof(MsgSlot, type_item), 1u);
    EXPECT_EQ(offsetof(MsgSlot, quantity), 2u);

    MsgSlot slot{};
    slot.slot_index = 3;
    slot.type_item = LIFE_POTION;
    slot.quantity = 25;

    EXPECT_EQ(slot.slot_index, 3u);
    EXPECT_EQ(slot.type_item, LIFE_POTION);
    EXPECT_EQ(slot.quantity, 25u);
}

TEST(ProtocolMessageLayout, SignupLayoutHasNoPadding) {
    EXPECT_EQ(offsetof(MsgSignup, opcode), 0u);
    EXPECT_EQ(offsetof(MsgSignup, user), 1u);
    EXPECT_EQ(offsetof(MsgSignup, password), 31u);
    EXPECT_EQ(offsetof(MsgSignup, traits), 61u);

    MsgSignup signup{};
    EXPECT_EQ(signup.opcode, SIGNUP);
    std::strncpy(signup.user, "nuevo", sizeof(signup.user) - 1);
    std::strncpy(signup.password, "secreta", sizeof(signup.password) - 1);
    signup.traits = CharacterTraits{1, 2, ELF, CLERIC};

    EXPECT_STREQ(signup.user, "nuevo");
    EXPECT_STREQ(signup.password, "secreta");
    EXPECT_EQ(signup.traits.head, 1u);
    EXPECT_EQ(signup.traits.body, 2u);
    EXPECT_EQ(signup.traits.race, ELF);
    EXPECT_EQ(signup.traits.clase, CLERIC);
}

TEST(ProtocolContainers, InventoryAndEquipmentMessagesDefaultOpcodesAreStable) {
    MsgInventoryUpdate inv{};
    MsgEquipmentUpdate equip{};

    EXPECT_EQ(inv.opcode, INVENTORY_UPDATE);
    EXPECT_TRUE(inv.inventory.empty());
    inv.inventory.push_back(MsgSlot{0, SWORD, 1});
    inv.inventory.push_back(MsgSlot{1, LIFE_POTION, 5});
    EXPECT_EQ(inv.inventory.size(), 2u);

    EXPECT_EQ(equip.opcode, EQUIPMENT_UPDATE);
    EXPECT_TRUE(equip.equipment.empty());
    equip.equipment.push_back(MsgSlot{0, IRON_HELMET, 1});
    EXPECT_EQ(equip.equipment.size(), 1u);
}

TEST(ProtocolSnapshotLayout, SnapshotElementSizesArePacked) {
    EXPECT_EQ(sizeof(PlayerSnapshotData), 77u);
    EXPECT_EQ(sizeof(CitizenNpcSnapshot), 44u);
    EXPECT_EQ(sizeof(NpcSnapshotData), 48u);
    EXPECT_EQ(sizeof(ItemGroundSnapshotData), 9u);
    EXPECT_EQ(sizeof(SoundEffectSnapshotData), 10u);
    EXPECT_EQ(sizeof(VisualEffectSnapshotData), 14u);
}

TEST(ProtocolSnapshotLayout, PlayerSnapshotFieldOffsetsAreStable) {
    EXPECT_EQ(offsetof(PlayerSnapshotData, name), 0u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, id), 30u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, position), 34u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, direction), 42u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, stats), 43u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, ch_traits), 64u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, weapon_id), 70u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, shield_id), 71u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, helmet_id), 72u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, armor_id), 73u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, flags), 74u);
    EXPECT_EQ(offsetof(PlayerSnapshotData, resurrection_time_left_ms), 75u);
}

TEST(ProtocolSnapshotLayout, OtherSnapshotFieldOffsetsAreStable) {
    EXPECT_EQ(offsetof(NpcSnapshotData, name), 0u);
    EXPECT_EQ(offsetof(NpcSnapshotData, id), 30u);
    EXPECT_EQ(offsetof(NpcSnapshotData, type_id), 34u);
    EXPECT_EQ(offsetof(NpcSnapshotData, position), 35u);
    EXPECT_EQ(offsetof(NpcSnapshotData, direction), 43u);
    EXPECT_EQ(offsetof(NpcSnapshotData, current_hp), 44u);
    EXPECT_EQ(offsetof(NpcSnapshotData, max_hp), 46u);

    EXPECT_EQ(offsetof(ItemGroundSnapshotData, item_id), 0u);
    EXPECT_EQ(offsetof(ItemGroundSnapshotData, position), 1u);

    EXPECT_EQ(offsetof(SoundEffectSnapshotData, effect_id), 0u);
    EXPECT_EQ(offsetof(SoundEffectSnapshotData, pos_x), 2u);
    EXPECT_EQ(offsetof(SoundEffectSnapshotData, pos_y), 6u);

    EXPECT_EQ(offsetof(VisualEffectSnapshotData, effect_id), 0u);
    EXPECT_EQ(offsetof(VisualEffectSnapshotData, recipient_id), 2u);
    EXPECT_EQ(offsetof(VisualEffectSnapshotData, pos_x), 6u);
    EXPECT_EQ(offsetof(VisualEffectSnapshotData, pos_y), 10u);
}

TEST(ProtocolNetworkOrder, Uint16ValuesAreWrittenInNetworkByteOrder) {
    const uint16_t host_value = 0x1234;
    const uint16_t network_value = htons(host_value);
    const auto bytes = bytesOf<uint16_t, sizeof(uint16_t)>(network_value);

    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[1], 0x34);
    EXPECT_EQ(ntohs(network_value), host_value);
}

TEST(ProtocolNetworkOrder, Uint32ValuesAreWrittenInNetworkByteOrder) {
    const uint32_t host_value = 0x11223344;
    const uint32_t network_value = htonl(host_value);
    const auto bytes = bytesOf<uint32_t, sizeof(uint32_t)>(network_value);

    EXPECT_EQ(bytes[0], 0x11);
    EXPECT_EQ(bytes[1], 0x22);
    EXPECT_EQ(bytes[2], 0x33);
    EXPECT_EQ(bytes[3], 0x44);
    EXPECT_EQ(ntohl(network_value), host_value);
}

TEST(ProtocolNetworkOrder, AttackWireBytesAreOpcodeThenNetworkOrderedId) {
    std::array<uint8_t, sizeof(MsgAttack)> wire{};
    size_t offset = 0;

    const uint8_t opcode = ATTACK;
    std::memcpy(wire.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint32_t target_id = htonl(0x01020304);
    std::memcpy(wire.data() + offset, &target_id, sizeof(target_id));

    EXPECT_EQ(wire, (std::array<uint8_t, 5>{ATTACK, 0x01, 0x02, 0x03, 0x04}));
}

TEST(ProtocolNetworkOrder, TradeWireBytesAreOpcodeNpcItemAndQuantity) {
    std::array<uint8_t, sizeof(MsgTrade)> wire{};
    size_t offset = 0;

    const uint8_t opcode = SELL_ITEM;
    std::memcpy(wire.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint32_t npc_id = htonl(0x11223344);
    std::memcpy(wire.data() + offset, &npc_id, sizeof(npc_id));
    offset += sizeof(npc_id);

    const uint8_t item_id = SWORD;
    std::memcpy(wire.data() + offset, &item_id, sizeof(item_id));
    offset += sizeof(item_id);

    const uint16_t quantity = htons(0x0506);
    std::memcpy(wire.data() + offset, &quantity, sizeof(quantity));

    EXPECT_EQ(wire, (std::array<uint8_t, 8>{SELL_ITEM, 0x11, 0x22, 0x33, 0x44, SWORD, 0x05, 0x06}));
}

TEST(ProtocolFlags, PlayerFlagsAreIndependentBits) {
    EXPECT_EQ(PLAYER_FLAG_GHOST, 1u);
    EXPECT_EQ(PLAYER_FLAG_MEDITATING, 2u);
    EXPECT_EQ((PLAYER_FLAG_GHOST & PLAYER_FLAG_MEDITATING), 0u);

    uint8_t flags = 0;
    flags |= PLAYER_FLAG_GHOST;
    EXPECT_TRUE(flags & PLAYER_FLAG_GHOST);
    EXPECT_FALSE(flags & PLAYER_FLAG_MEDITATING);
    flags |= PLAYER_FLAG_MEDITATING;
    EXPECT_TRUE(flags & PLAYER_FLAG_GHOST);
    EXPECT_TRUE(flags & PLAYER_FLAG_MEDITATING);
}

TEST(ClientProtocolWireSend, LoginWritesOpcodeAndFixedLengthCredentials) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendLogin("jugador", "clave");

    MsgLogin msg{};
    peer_socket.recvall(&msg, sizeof(msg));
    EXPECT_EQ(msg.opcode, LOGIN);
    EXPECT_STREQ(msg.name, "jugador");
    EXPECT_STREQ(msg.pass, "clave");
    EXPECT_EQ(msg.name[MAX_NAME_SIZE - 1], '\0');
    EXPECT_EQ(msg.pass[MAX_NAME_SIZE - 1], '\0');
}

TEST(ClientProtocolWireSend, MoveWritesOpcodeAndDirection) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendMove(LEFT);

    const auto bytes = recvBytes(peer_socket, sizeof(MsgMove));
    EXPECT_EQ(bytes, (std::vector<uint8_t>{MOVE, LEFT}));
}

TEST(ClientProtocolWireSend, AttackWritesNetworkOrderedTargetId) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendAttack(0x01020304);

    const auto bytes = recvBytes(peer_socket, sizeof(MsgAttack));
    EXPECT_EQ(bytes, (std::vector<uint8_t>{ATTACK, 0x01, 0x02, 0x03, 0x04}));
}

TEST(ClientProtocolWireSend, ChatWritesLengthPrefixedText) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendChat("hola");

    const auto bytes = recvBytes(peer_socket, 1 + 2 + 4);
    EXPECT_EQ(bytes, (std::vector<uint8_t>{CHAT, 0x00, 0x04, 'h', 'o', 'l', 'a'}));
}

TEST(ClientProtocolWireSend, CommandWritesLengthPrefixedText) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendCommand("/meditar");

    const auto bytes = recvBytes(peer_socket, 1 + 2 + 8);
    EXPECT_EQ(bytes,
              (std::vector<uint8_t>{COMMAND, 0x00, 0x08, '/', 'm', 'e', 'd', 'i', 't', 'a', 'r'}));
}

TEST(ClientProtocolWireSend, UseAndDropItemWriteNetworkOrderedSlot) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendUseItem(7);
    EXPECT_EQ(recvBytes(peer_socket, sizeof(MsgSlotItem)),
              (std::vector<uint8_t>{USE_ITEM, 0x00, 0x00, 0x00, 0x07}));

    protocol.sendDropItem(8);
    EXPECT_EQ(recvBytes(peer_socket, sizeof(MsgSlotItem)),
              (std::vector<uint8_t>{DROP_ITEM, 0x00, 0x00, 0x00, 0x08}));
}

TEST(ClientProtocolWireSend, InteractWritesNpcAndAction) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendInteract(0x01020304, 1);

    const auto bytes = recvBytes(peer_socket, sizeof(MsgInteract));
    EXPECT_EQ(bytes, (std::vector<uint8_t>{INTERACT, 0x01, 0x02, 0x03, 0x04, 0x01}));
}

TEST(ClientProtocolWireSend, TakeDisconnectAndResurrectAreSingleByteMessages) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendTakeItem();
    EXPECT_EQ(recvBytes(peer_socket, 1), (std::vector<uint8_t>{TAKE_ITEM}));

    protocol.sendDisconnect();
    EXPECT_EQ(recvBytes(peer_socket, 1), (std::vector<uint8_t>{DISCONNECT}));

    protocol.sendResurrect();
    EXPECT_EQ(recvBytes(peer_socket, 1), (std::vector<uint8_t>{RESURRECT}));
}

TEST(ClientProtocolWireSend, BuyAndSellWriteTradePayload) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendBuyItem(0x01020304, SWORD, 0x0506);
    EXPECT_EQ(recvBytes(peer_socket, sizeof(MsgTrade)),
              (std::vector<uint8_t>{BUY_ITEM, 0x01, 0x02, 0x03, 0x04, SWORD, 0x05, 0x06}));

    protocol.sendSellItem(0x11223344, AXE, 0x0708);
    EXPECT_EQ(recvBytes(peer_socket, sizeof(MsgTrade)),
              (std::vector<uint8_t>{SELL_ITEM, 0x11, 0x22, 0x33, 0x44, AXE, 0x07, 0x08}));
}

TEST(ClientProtocolWireSend, SignupWritesNetworkOrderedTraits) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendSignup("nuevo", "clave", CharacterTraits{0x0102, 0x0304, ELF, CLERIC});

    const auto bytes = recvBytes(peer_socket, sizeof(MsgSignup));
    ASSERT_EQ(bytes.size(), 67u);
    EXPECT_EQ(bytes[0], SIGNUP);
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(bytes.data() + 1)), "nuevo");
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(bytes.data() + 31)), "clave");
    EXPECT_EQ(bytes[61], 0x01);
    EXPECT_EQ(bytes[62], 0x02);
    EXPECT_EQ(bytes[63], 0x03);
    EXPECT_EQ(bytes[64], 0x04);
    EXPECT_EQ(bytes[65], ELF);
    EXPECT_EQ(bytes[66], CLERIC);
}

TEST(ClientProtocolWireSend, BankAndEquipmentCommandsWriteExpectedPayloads) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    protocol.sendListItems(0x01020304);
    EXPECT_EQ(recvBytes(peer_socket, 5),
              (std::vector<uint8_t>{LIST_ITEMS, 0x01, 0x02, 0x03, 0x04}));

    protocol.sendDepositItem(0x01020304, 0x0506);
    EXPECT_EQ(recvBytes(peer_socket, 7),
              (std::vector<uint8_t>{DEPOSIT_ITEM, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06}));

    protocol.sendWithdrawItem(0x01020304, 0x0708);
    EXPECT_EQ(recvBytes(peer_socket, 7),
              (std::vector<uint8_t>{WITHDRAW_ITEM, 0x01, 0x02, 0x03, 0x04, 0x07, 0x08}));

    protocol.sendDepositGold(0x01020304, 0x05060708);
    EXPECT_EQ(recvBytes(peer_socket, 9),
              (std::vector<uint8_t>{DEPOSIT_GOLD, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}));

    protocol.sendWithdrawGold(0x01020304, 0x090A0B0C);
    EXPECT_EQ(recvBytes(peer_socket, 9), (std::vector<uint8_t>{WITHDRAW_GOLD, 0x01, 0x02, 0x03,
                                                               0x04, 0x09, 0x0A, 0x0B, 0x0C}));

    protocol.sendEquipItem(0x01020304);
    EXPECT_EQ(recvBytes(peer_socket, 5),
              (std::vector<uint8_t>{EQUIP_ITEM, 0x01, 0x02, 0x03, 0x04}));

    protocol.sendUnequipItem(0x01020304);
    EXPECT_EQ(recvBytes(peer_socket, 5),
              (std::vector<uint8_t>{UNEQUIP_ITEM, 0x01, 0x02, 0x03, 0x04}));
}

TEST(ClientProtocolWireReceive, ReceivesLoginResponse) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, LOGIN_RESPONSE);
    appendU8(wire, 1);
    appendText(wire, "ok");
    appendNetU32(wire, 0x01020304);
    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::LOGIN_RESPONSE);
    EXPECT_TRUE(event.login_success);
    EXPECT_EQ(event.text_payload, "ok");
    EXPECT_EQ(event.player_id, 0x01020304u);
}

TEST(ClientProtocolWireReceive, ReceivesChatAndActionErrorText) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> chat;
    appendU8(chat, CHAT_MSG);
    appendText(chat, "hola mundo");
    peer_socket.sendall(chat.data(), static_cast<unsigned int>(chat.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::CHAT_MSG);
    EXPECT_EQ(event.text_payload, "hola mundo");

    std::vector<uint8_t> error;
    appendU8(error, ACTION_ERROR);
    appendText(error, "no se puede");
    peer_socket.sendall(error.data(), static_cast<unsigned int>(error.size()));

    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::ERROR_ACTION);
    EXPECT_EQ(event.text_payload, "no se puede");
}

TEST(ClientProtocolWireReceive, ReceivesChangeMap) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, CHANGE_MAP);
    appendNetU16(wire, 42);
    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::MAP_CHANGE);
    EXPECT_EQ(event.map_id, 42u);
}

TEST(ClientProtocolWireReceive, ReceivesPlayerStats) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, PLAYER_STATS);
    appendNetU32(wire, 10);
    appendNetU32(wire, 20);
    appendNetU32(wire, 30);
    appendNetU32(wire, 40);
    appendNetU32(wire, 50);
    appendNetU32(wire, 60);
    appendNetU32(wire, 70);
    appendNetU32(wire, 80);
    appendU8(wire, 9);
    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::OWN_STATS);
    EXPECT_EQ(event.stats.hp, 10u);
    EXPECT_EQ(event.stats.max_hp, 20u);
    EXPECT_EQ(event.stats.mana, 30u);
    EXPECT_EQ(event.stats.max_mana, 40u);
    EXPECT_EQ(event.stats.safe_gold, 50u);
    EXPECT_EQ(event.stats.excess_gold, 60u);
    EXPECT_EQ(event.stats.exp, 70u);
    EXPECT_EQ(event.stats.exp_next_level, 80u);
    EXPECT_EQ(event.stats.level, 9u);
}

TEST(ClientProtocolWireReceive, ReceivesInventoryAndEquipmentSlots) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> inv;
    appendU8(inv, INVENTORY_UPDATE);
    appendNetU16(inv, 2);
    MsgSlot slot0{0, SWORD, htons(3)};
    MsgSlot slot1{1, LIFE_POTION, htons(7)};
    appendBytes(inv, &slot0, sizeof(slot0));
    appendBytes(inv, &slot1, sizeof(slot1));
    peer_socket.sendall(inv.data(), static_cast<unsigned int>(inv.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    ASSERT_EQ(event.type, TypeEventClient::INVENTORY_UPDATE);
    ASSERT_EQ(event.inventory.size(), 2u);
    EXPECT_EQ(event.inventory[0].slot_index, 0u);
    EXPECT_EQ(event.inventory[0].type_item, SWORD);
    EXPECT_EQ(event.inventory[0].quantity, 3u);
    EXPECT_EQ(event.inventory[1].slot_index, 1u);
    EXPECT_EQ(event.inventory[1].type_item, LIFE_POTION);
    EXPECT_EQ(event.inventory[1].quantity, 7u);

    std::vector<uint8_t> equip;
    appendU8(equip, EQUIPMENT_UPDATE);
    appendNetU16(equip, 1);
    MsgSlot helmet{2, IRON_HELMET, htons(1)};
    appendBytes(equip, &helmet, sizeof(helmet));
    peer_socket.sendall(equip.data(), static_cast<unsigned int>(equip.size()));

    ASSERT_TRUE(protocol.receiveMessage(event));
    ASSERT_EQ(event.type, TypeEventClient::EQUIPMENT_UPDATE);
    ASSERT_EQ(event.equipment.size(), 1u);
    EXPECT_EQ(event.equipment[0].slot_index, 2u);
    EXPECT_EQ(event.equipment[0].type_item, IRON_HELMET);
    EXPECT_EQ(event.equipment[0].quantity, 1u);
}

TEST(ClientProtocolWireReceive, ReceivesTraderCatalog) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, TRADER_CATALOG);
    appendNetU16(wire, 2);
    appendU8(wire, SWORD);
    appendNetU32(wire, 10);
    appendNetU32(wire, 5);
    appendU8(wire, LIFE_POTION);
    appendNetU32(wire, 20);
    appendNetU32(wire, 8);
    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::OPEN_MERCHANT);
    ASSERT_EQ(event.merchant_data.catalog.size(), 2u);
    EXPECT_EQ(event.merchant_data.catalog.at(SWORD).purchase_price, 10u);
    EXPECT_EQ(event.merchant_data.catalog.at(SWORD).selling_price, 5u);
    EXPECT_EQ(event.merchant_data.catalog.at(LIFE_POTION).purchase_price, 20u);
    EXPECT_EQ(event.merchant_data.catalog.at(LIFE_POTION).selling_price, 8u);
}

TEST(ClientProtocolWireReceive, ReceivesBankContent) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, BANK_CONTENT);
    appendNetU32(wire, 1234);
    appendNetU16(wire, 2);
    appendU8(wire, SWORD);
    appendNetU32(wire, 3);
    appendU8(wire, MANA_POTION);
    appendNetU32(wire, 4);
    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::OPEN_BANK);
    EXPECT_EQ(event.bank_data.gold, 1234u);
    ASSERT_EQ(event.bank_data.items.size(), 2u);
    EXPECT_EQ(event.bank_data.items.at(SWORD), 3u);
    EXPECT_EQ(event.bank_data.items.at(MANA_POTION), 4u);
}

TEST(ClientProtocolWireReceive, ReceivesSnapshotWithAllCollections) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, SNAPSHOT);

    appendNetU16(wire, 1);
    PlayerSnapshotData player{};
    std::strncpy(player.name, "jugador", sizeof(player.name) - 1);
    player.id = htonl(100);
    player.position.x = htonl(10);
    player.position.y = htonl(20);
    player.direction = RIGHT;
    player.stats.level = 4;
    player.stats.xp = htonl(500);
    player.stats.current_hp = htons(30);
    player.stats.max_hp = htons(40);
    player.stats.current_mana = htons(50);
    player.stats.max_mana = htons(60);
    player.stats.safe_gold = 700;    // These are not converted by receivePlayers today.
    player.stats.excess_gold = 800;  // Keep host values to document current contract.
    player.ch_traits.head = htons(1);
    player.ch_traits.body = htons(2);
    player.ch_traits.race = HUMAN;
    player.ch_traits.clase = WIZARD;
    player.weapon_id = SWORD;
    player.flags = PLAYER_FLAG_GHOST | PLAYER_FLAG_MEDITATING;
    player.resurrection_time_left_ms = htons(900);
    appendBytes(wire, &player, sizeof(player));

    appendNetU16(wire, 1);
    NpcSnapshotData npc{};
    std::strncpy(npc.name, "npc", sizeof(npc.name) - 1);
    npc.id = htonl(200);
    npc.type_id = MERCHANT;
    npc.position.x = htonl(30);
    npc.position.y = htonl(40);
    npc.direction = LEFT;
    npc.current_hp = htons(70);
    npc.max_hp = htons(80);
    appendBytes(wire, &npc, sizeof(npc));

    appendNetU16(wire, 1);
    ItemGroundSnapshotData item{};
    item.item_id = LIFE_POTION;
    item.position.x = htonl(50);
    item.position.y = htonl(60);
    appendBytes(wire, &item, sizeof(item));

    appendNetU16(wire, 1);
    SoundEffectSnapshotData sound{};
    sound.effect_id =
            static_cast<SoundEffectID>(htons(static_cast<uint16_t>(SoundEffectID::CURAR)));
    sound.pos_x = htonl(70);
    sound.pos_y = htonl(80);
    appendBytes(wire, &sound, sizeof(sound));

    appendNetU16(wire, 1);
    VisualEffectSnapshotData visual{};
    visual.effect_id =
            static_cast<VisualEffectID>(htons(static_cast<uint16_t>(VisualEffectID::LIGHTNING)));
    visual.recipient_id = htonl(100);
    visual.pos_x = htonl(90);
    visual.pos_y = htonl(91);
    appendBytes(wire, &visual, sizeof(visual));

    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    ASSERT_EQ(event.type, TypeEventClient::UPDATE_WORLD);

    ASSERT_EQ(event.world.players.size(), 1u);
    const auto& got_player = event.world.players[0];
    EXPECT_STREQ(got_player.name, "jugador");
    EXPECT_EQ(got_player.id, 100u);
    EXPECT_EQ(got_player.position.x, 10u);
    EXPECT_EQ(got_player.position.y, 20u);
    EXPECT_EQ(got_player.direction, RIGHT);
    EXPECT_EQ(got_player.stats.level, 4u);
    EXPECT_EQ(got_player.stats.xp, 500u);
    EXPECT_EQ(got_player.stats.current_hp, 30u);
    EXPECT_EQ(got_player.stats.max_hp, 40u);
    EXPECT_EQ(got_player.stats.current_mana, 50u);
    EXPECT_EQ(got_player.stats.max_mana, 60u);
    EXPECT_EQ(got_player.ch_traits.head, 1u);
    EXPECT_EQ(got_player.ch_traits.body, 2u);
    EXPECT_EQ(got_player.weapon_id, SWORD);
    EXPECT_TRUE(got_player.flags & PLAYER_FLAG_GHOST);
    EXPECT_TRUE(got_player.flags & PLAYER_FLAG_MEDITATING);
    EXPECT_EQ(got_player.resurrection_time_left_ms, 900u);

    ASSERT_EQ(event.world.npcs.size(), 1u);
    EXPECT_STREQ(event.world.npcs[0].name, "npc");
    EXPECT_EQ(event.world.npcs[0].id, 200u);
    EXPECT_EQ(event.world.npcs[0].type_id, MERCHANT);
    EXPECT_EQ(event.world.npcs[0].position.x, 30u);
    EXPECT_EQ(event.world.npcs[0].position.y, 40u);
    EXPECT_EQ(event.world.npcs[0].current_hp, 70u);
    EXPECT_EQ(event.world.npcs[0].max_hp, 80u);

    ASSERT_EQ(event.world.items_on_floor.size(), 1u);
    EXPECT_EQ(event.world.items_on_floor[0].item_id, LIFE_POTION);
    EXPECT_EQ(event.world.items_on_floor[0].position.x, 50u);
    EXPECT_EQ(event.world.items_on_floor[0].position.y, 60u);

    ASSERT_EQ(event.world.sound_effects.size(), 1u);
    EXPECT_EQ(event.world.sound_effects[0].effect_id, SoundEffectID::CURAR);
    EXPECT_EQ(event.world.sound_effects[0].pos_x, 70u);
    EXPECT_EQ(event.world.sound_effects[0].pos_y, 80u);

    ASSERT_EQ(event.world.visual_effects.size(), 1u);
    EXPECT_EQ(event.world.visual_effects[0].effect_id, VisualEffectID::LIGHTNING);
    EXPECT_EQ(event.world.visual_effects[0].recipient_id, 100u);
    EXPECT_EQ(event.world.visual_effects[0].pos_x, 90u);
    EXPECT_EQ(event.world.visual_effects[0].pos_y, 91u);
}

TEST(ClientProtocolWireReceive, ReceivesMapDataAndCitizens) {
    MAKE_PROTOCOL_SOCKET_PAIR_OR_SKIP();
    ClientProtocol protocol(client_socket);

    std::vector<uint8_t> wire;
    appendU8(wire, MAP_DATA);
    appendNetU32(wire, 1);
    appendNetU32(wire, 1);

    for (uint8_t i = 0; i < layer_count; ++i) {
        appendNetU32(wire, 100 + i);
        appendU8(wire, i % 2 == 0 ? 1 : 0);
        appendU8(wire, static_cast<uint8_t>(Region::City));
    }

    appendNetU16(wire, 1);
    CitizenNpcSnapshot citizen{};
    std::strncpy(citizen.name, "banquero", sizeof(citizen.name) - 1);
    citizen.id = htonl(300);
    citizen.type = BANKER;
    citizen.position.x = htonl(5);
    citizen.position.y = htonl(6);
    citizen.direction = DOWN;
    appendBytes(wire, &citizen, sizeof(citizen));

    peer_socket.sendall(wire.data(), static_cast<unsigned int>(wire.size()));

    EventClient event;
    ASSERT_TRUE(protocol.receiveMessage(event));
    EXPECT_EQ(event.type, TypeEventClient::MAP_DATA);
    EXPECT_EQ(event.map_data.width(), 1);
    EXPECT_EQ(event.map_data.height(), 1);
    EXPECT_EQ(event.map_data.tile_at(0, 0, Layer::Background).sprite_id, 100);
    EXPECT_TRUE(event.map_data.tile_at(0, 0, Layer::Background).walkable);
    EXPECT_EQ(event.map_data.tile_at(0, 0, Layer::Details).sprite_id, 101);
    EXPECT_FALSE(event.map_data.tile_at(0, 0, Layer::Details).walkable);
    EXPECT_EQ(event.map_data.tile_at(0, 0, Layer::Object).region, Region::City);
    EXPECT_EQ(event.map_data.tile_at(0, 0, Layer::Roof).sprite_id, 103);

    ASSERT_EQ(event.citizens.size(), 1u);
    EXPECT_STREQ(event.citizens[0].name, "banquero");
    EXPECT_EQ(event.citizens[0].id, 300u);
    EXPECT_EQ(event.citizens[0].type, BANKER);
    EXPECT_EQ(event.citizens[0].position.x, 5u);
    EXPECT_EQ(event.citizens[0].position.y, 6u);
    EXPECT_EQ(event.citizens[0].direction, DOWN);
}
