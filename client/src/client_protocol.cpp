#include "client/includes/client_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

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
        std::string mssgErr = "ERROR IN sendLogin -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendMove(const uint8_t direction) const {
    MsgMove msg;
    msg.direction = direction;
    try {
        socket.sendall(&msg, sizeof(MsgMove));
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendMove -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendAttack(const uint32_t target_id) const {
    MsgAttack msg;
    msg.target_id = htonl(target_id);
    try {
        socket.sendall(&msg, sizeof(MsgAttack));
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendAttack -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
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

    if (!msg.empty()) {
        std::memcpy(buffer.data() + offset, msg.data(), msg.size());
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendChat -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}


void ClientProtocol::sendUseItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = USE_ITEM;
    msg.slot_index = slot_index;
    try {
        socket.sendall(&msg, sizeof(MsgSlotItem));
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendUseItem -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendDropItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = DROP_ITEM;
    msg.slot_index = slot_index;
    try {
        socket.sendall(&msg, sizeof(MsgSlotItem));
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendDropItem -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
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

    if (!cmd.empty()) {
        std::memcpy(buffer.data() + offset, cmd.data(), cmd.size());
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendCommand -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendInteract(const uint32_t npc_id) const {
    MsgInteract msg;
    msg.npc_id = htonl(npc_id);
    try {
        socket.sendall(&msg, sizeof(MsgInteract));
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendInteract -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendTakeItem() const {
    constexpr uint8_t opcode = TAKE_ITEM;
    try {
        socket.sendall(&opcode, 1);
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendTakeItem -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
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
        std::string mssgErr = "ERROR IN sendBuyItem -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
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
        std::string mssgErr = "ERROR IN sendSellItem -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ClientProtocol::sendDisconnect() const {
    constexpr uint8_t opcode = DISCONNECT;
    try {
        socket.sendall(&opcode, 1);
    } catch (const std::exception& e) {
        std::string mssgErr = "ERROR IN sendDisconnect -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

bool ClientProtocol::receiveMessage(EventClient& out_evento) const {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) {
        out_evento.type = TypeEventClient::DISCONNECTION;
        return false;
    }

    switch (opcode) {
        case SNAPSHOT: {
            out_evento.type = TypeEventClient::UPDATE_WORLD;
            out_evento.world.players.clear();
            out_evento.world.npcs.clear();
            out_evento.world.items.clear();

            // 1. Jugadores
            uint16_t p_count;
            socket.recvall(&p_count, 2);
            p_count = ntohs(p_count);
            for (int i = 0; i < p_count; ++i) {
                PlayerSnapshotData p;
                socket.recvall(&p, sizeof(PlayerSnapshotData));
                p.id = ntohl(p.id);
                p.hp_max = ntohs(p.hp_max);
                p.hp_actual = ntohs(p.hp_actual);
                p.body_id = ntohs(p.body_id);
                p.head_id = ntohs(p.head_id);
                p.weapon_id = ntohs(p.weapon_id);
                out_evento.world.players.push_back(p);
            }

            // 2. NPCs
            uint16_t n_count;
            socket.recvall(&n_count, 2);
            n_count = ntohs(n_count);
            for (int i = 0; i < n_count; ++i) {
                NpcSnapshotData n;
                socket.recvall(&n, sizeof(NpcSnapshotData));
                n.id = ntohl(n.id);
                n.type_id = ntohs(n.type_id);
                n.hp_actual = ntohs(n.hp_actual);
                out_evento.world.npcs.push_back(n);
            }

            // 3. Items
            uint16_t i_count;
            socket.recvall(&i_count, 2);
            i_count = ntohs(i_count);
            for (int i = 0; i < i_count; ++i) {
                ItemGroundSnapshotData it;
                socket.recvall(&it, sizeof(ItemGroundSnapshotData));
                it.item_id = ntohs(it.item_id);
                out_evento.world.items.push_back(it);
            }
            break;
        }
        case PLAYER_STATS: {
            out_evento.type = TypeEventClient::OWN_STATS;
            out_evento.stats.opcode = opcode;

            socket.recvall(&out_evento.stats.hp, 4);
            out_evento.stats.hp = ntohl(out_evento.stats.hp);

            socket.recvall(&out_evento.stats.mana, 4);
            out_evento.stats.mana = ntohl(out_evento.stats.mana);

            socket.recvall(&out_evento.stats.gold, 4);
            out_evento.stats.gold = ntohl(out_evento.stats.gold);

            socket.recvall(&out_evento.stats.exp, 4);
            out_evento.stats.exp = ntohl(out_evento.stats.exp);

            socket.recvall(&out_evento.stats.nivel, 1);
            break;
        }
        case LOGIN_RESPONSE: {
            out_evento.type = TypeEventClient::LOGIN_RESPONSE;
            uint8_t success;
            socket.recvall(&success, 1);

            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            out_evento.text_payload.resize(len);
            socket.recvall(out_evento.text_payload.data(), len);
            break;
        }
        case CHANGE_MAP: {
            out_evento.type = TypeEventClient::MAP_CHANGE;
            socket.recvall(&out_evento.map_id, 2);
            out_evento.map_id = ntohs(out_evento.map_id);
            break;
        }
        case CHAT_MSG:
        case ACTION_ERROR: {
            out_evento.type = (opcode == CHAT_MSG) ? TypeEventClient::CHAT_MSG :
                                                     TypeEventClient::ERROR_ACTION;
            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            out_evento.text_payload.resize(len);
            socket.recvall(out_evento.text_payload.data(), len);
            break;
        }
        default:
            return true;
    }
    return true;
}
