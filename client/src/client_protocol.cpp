#include "client/includes/client_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "common/includes/map/layer.h"

ClientProtocol::ClientProtocol(Socket& s): socket(s) {}

void ClientProtocol::sendLogin(const std::string& name, const std::string& pass) const {
    MsgLogin msg;
    std::memset(msg.name, 0, sizeof(msg.name));
    std::memset(msg.pass, 0, sizeof(msg.pass));
    std::strncpy(msg.name, name.c_str(), sizeof(msg.name) - 1);
    std::strncpy(msg.pass, pass.c_str(), sizeof(msg.pass) - 1);
    try {
        socket.sendall(&msg, sizeof(MsgLogin));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendLogin -- ") + e.what());
    }
}

void ClientProtocol::sendMove(const uint8_t direction) const {
    MsgMove msg;
    msg.direction = direction;
    try {
        socket.sendall(&msg, sizeof(MsgMove));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendMove -- ") + e.what());
    }
}

void ClientProtocol::sendAttack(const uint32_t target_id) const {
    MsgAttack msg;
    msg.target_id = htonl(target_id);
    try {
        socket.sendall(&msg, sizeof(MsgAttack));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendAttack -- ") + e.what());
    }
}

void ClientProtocol::sendChat(const std::string& msg) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint16_t) + msg.size();
    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = CHAT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t len = htons(static_cast<uint16_t>(msg.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!msg.empty())
        std::memcpy(buffer.data() + offset, msg.data(), msg.size());

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendChat -- ") + e.what());
    }
}

void ClientProtocol::sendUseItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = USE_ITEM;
    msg.slot_index = slot_index;
    try {
        socket.sendall(&msg, sizeof(MsgSlotItem));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendUseItem -- ") + e.what());
    }
}

void ClientProtocol::sendDropItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = DROP_ITEM;
    msg.slot_index = slot_index;
    try {
        socket.sendall(&msg, sizeof(MsgSlotItem));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendDropItem -- ") + e.what());
    }
}

void ClientProtocol::sendCommand(const std::string& cmd) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint16_t) + cmd.size();
    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = COMMAND;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t len = htons(static_cast<uint16_t>(cmd.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!cmd.empty())
        std::memcpy(buffer.data() + offset, cmd.data(), cmd.size());

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendCommand -- ") + e.what());
    }
}

void ClientProtocol::sendInteract(const uint32_t npc_id) const {
    MsgInteract msg;
    msg.npc_id = htonl(npc_id);
    try {
        socket.sendall(&msg, sizeof(MsgInteract));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendInteract -- ") + e.what());
    }
}

void ClientProtocol::sendTakeItem() const {
    constexpr uint8_t opcode = TAKE_ITEM;
    try {
        socket.sendall(&opcode, 1);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendTakeItem -- ") + e.what());
    }
}

void ClientProtocol::sendBuyItem(const uint32_t npc_id, const uint16_t item_id,
                                 const uint16_t quantity) const {
    MsgTrade msg;
    msg.opcode = BUY_ITEM;
    msg.npc_id = htonl(npc_id);
    msg.item_id = htons(item_id);
    msg.quantity = htons(quantity);
    try {
        socket.sendall(&msg, sizeof(MsgTrade));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendBuyItem -- ") + e.what());
    }
}

void ClientProtocol::sendSellItem(const uint32_t npc_id, const uint16_t item_id,
                                  const uint16_t quantity) const {
    MsgTrade msg;
    msg.opcode = SELL_ITEM;
    msg.npc_id = htonl(npc_id);
    msg.item_id = htons(item_id);
    msg.quantity = htons(quantity);
    try {
        socket.sendall(&msg, sizeof(MsgTrade));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendSellItem -- ") + e.what());
    }
}

void ClientProtocol::sendDisconnect() const {
    constexpr uint8_t opcode = DISCONNECT;
    try {
        socket.sendall(&opcode, 1);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendDisconnect -- ") + e.what());
    }
}

void ClientProtocol::sendSignup(const std::string& user, const std::string& password, const CharacterTraits& traits) const {
    MsgSignup msg{};
    std::strncpy(msg.user, user.c_str(), sizeof(msg.user) - 1);
    std::strncpy(msg.password, password.c_str(), sizeof(msg.password) - 1);
    msg.traits = traits;
    try {
        socket.sendall(&msg, sizeof(MsgSignup));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendSignup -- ") + e.what());
    }
}

void ClientProtocol::sendCharacterCreate(const std::string& name, uint8_t race,
                                         uint8_t clase) const {
    MsgCharacterCreate msg{};
    std::strncpy(msg.name, name.c_str(), sizeof(msg.name) - 1);
    msg.race = race;
    msg.clase = clase;
    try {
        socket.sendall(&msg, sizeof(MsgCharacterCreate));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendCharacterCreate -- ") + e.what());
    }
}

bool ClientProtocol::recvResponse(uint8_t expected_opcode, std::string& out_message) const {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0)
        return false;
    if (opcode != expected_opcode)
        return false;

    uint8_t success;
    socket.recvall(&success, 1);

    uint16_t len;
    socket.recvall(&len, 2);
    len = ntohs(len);

    out_message.resize(len);
    if (len > 0)
        socket.recvall(out_message.data(), len);

    return success == 1;
}

bool ClientProtocol::receiveMessage(EventClient& out_event) const {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) {
        out_event.type = TypeEventClient::DISCONNECTION;
        return false;
    }

    switch (opcode) {
        case SNAPSHOT: {
            out_event.type = TypeEventClient::UPDATE_WORLD;
            out_event.world.players.clear();
            out_event.world.npcs.clear();
            out_event.world.items_on_floor.clear();

            uint16_t p_count;
            socket.recvall(&p_count, 2);
            p_count = ntohs(p_count);
            for (uint16_t i = 0; i < p_count; ++i) {
                PlayerSnapshotData p;
                socket.recvall(&p, sizeof(PlayerSnapshotData));
                p.id = ntohl(p.id);
                p.max_hp = ntohs(p.max_hp);
                p.hp = ntohs(p.hp);
                p.body_id = ntohs(p.body_id);
                p.head_id = ntohs(p.head_id);
                p.weapon_id = ntohs(p.weapon_id);
                out_event.world.players.push_back(p);
            }

            uint16_t n_count;
            socket.recvall(&n_count, 2);
            n_count = ntohs(n_count);
            for (uint16_t i = 0; i < n_count; ++i) {
                NpcSnapshotData n;
                socket.recvall(&n, sizeof(NpcSnapshotData));
                n.id = ntohl(n.id);
                n.type_id = ntohs(n.type_id);
                n.hp_actual = ntohs(n.hp_actual);
                out_event.world.npcs.push_back(n);
            }

            uint16_t i_count;
            socket.recvall(&i_count, 2);
            i_count = ntohs(i_count);
            for (uint16_t i = 0; i < i_count; ++i) {
                ItemGroundSnapshotData it;
                socket.recvall(&it, sizeof(ItemGroundSnapshotData));
                it.item_id = ntohs(it.item_id);
                out_event.world.items_on_floor.push_back(it);
            }
            break;
        }
        case PLAYER_STATS: {
            out_event.type = TypeEventClient::OWN_STATS;
            out_event.stats.opcode = opcode;

            socket.recvall(&out_event.stats.hp, 4);
            out_event.stats.hp = ntohl(out_event.stats.hp);

            socket.recvall(&out_event.stats.mana, 4);
            out_event.stats.mana = ntohl(out_event.stats.mana);

            socket.recvall(&out_event.stats.gold, 4);
            out_event.stats.gold = ntohl(out_event.stats.gold);

            socket.recvall(&out_event.stats.exp, 4);
            out_event.stats.exp = ntohl(out_event.stats.exp);

            socket.recvall(&out_event.stats.level, 1);
            break;
        }
        case LOGIN_RESPONSE: {
            out_event.type = TypeEventClient::LOGIN_RESPONSE;
            uint8_t success;
            socket.recvall(&success, 1);

            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            out_event.text_payload.resize(len);
            socket.recvall(out_event.text_payload.data(), len);
            break;
        }
        case CHANGE_MAP: {
            out_event.type = TypeEventClient::MAP_CHANGE;
            socket.recvall(&out_event.map_id, 2);
            out_event.map_id = ntohs(out_event.map_id);
            break;
        }
        case CHAT_MSG:
        case ACTION_ERROR: {
            out_event.type = (opcode == CHAT_MSG) ? TypeEventClient::CHAT_MSG :
                                                    TypeEventClient::ERROR_ACTION;
            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            out_event.text_payload.resize(len);
            socket.recvall(out_event.text_payload.data(), len);
            break;
        }
        case MAP_DATA: {
            uint32_t w_net, h_net;
            socket.recvall(&w_net, 4);
            socket.recvall(&h_net, 4);
            const size_t total_tiles =
                    ntohl(w_net) * ntohl(h_net) * static_cast<size_t>(layer_count);
            for (size_t i = 0; i < total_tiles; ++i) {
                int32_t sprite_id_net;
                uint8_t walkable_byte;
                socket.recvall(&sprite_id_net, 4);
                socket.recvall(&walkable_byte, 1);
            }
            break;
        }
        default:
            return true;
    }
    return true;
}
