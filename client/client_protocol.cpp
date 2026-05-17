#include "client_protocol.h"
#include <arpa/inet.h>
#include <cstring>

ClientProtocol::ClientProtocol(Socket& s) : socket(s) {}

void ClientProtocol::sendLogin(const std::string& name, const std::string& pass) {
    MsgLogin msg;
    std::memset(msg.name, 0, 30);
    std::memset(msg.pass, 0, 30);
    std::strncpy(msg.name, name.c_str(), 29);
    std::strncpy(msg.pass, pass.c_str(), 29);
    socket.sendall(&msg, sizeof(MsgLogin));
}

void ClientProtocol::sendMove(uint8_t direccion) {
    MsgMove msg;
    msg.direction = direccion;
    socket.sendall(&msg, sizeof(MsgMove));
}

void ClientProtocol::sendAttack(uint32_t target_id) {
    MsgAttack msg;
    msg.target_id = htonl(target_id);
    socket.sendall(&msg, sizeof(MsgAttack));
}

void ClientProtocol::sendChat(const std::string& msg) {
    uint8_t opcode = ClientOpcode::CHAT;
    uint16_t len = htons(static_cast<uint16_t>(msg.size()));
    socket.sendall(&opcode, 1);
    socket.sendall(&len, 2);
    socket.sendall(msg.data(), msg.size());
}

void ClientProtocol::sendUseItem(uint8_t slot_index) {
    MsgSlotItem msg;
    msg.opcode = ClientOpcode::USE_ITEM;
    msg.slot_index = slot_index;
    socket.sendall(&msg, sizeof(MsgSlotItem));
}

void ClientProtocol::sendDropItem(uint8_t slot_index) {
    MsgSlotItem msg;
    msg.opcode = ClientOpcode::DROP_ITEM;
    msg.slot_index = slot_index;
    socket.sendall(&msg, sizeof(MsgSlotItem));
}

void ClientProtocol::sendCommand(const std::string& cmd) {
    uint8_t opcode = ClientOpcode::COMMAND;
    uint16_t len = htons(static_cast<uint16_t>(cmd.size()));
    
    socket.sendall(&opcode, 1);
    socket.sendall(&len, 2);
    socket.sendall(cmd.data(), cmd.size());
}

void ClientProtocol::sendInteract(uint32_t npc_id) {
    MsgInteract msg;
    msg.npc_id = htonl(npc_id);
    socket.sendall(&msg, sizeof(MsgInteract));
}

void ClientProtocol::sendTakeItem() {
    uint8_t opcode = ClientOpcode::TAKE_ITEM;
    socket.sendall(&opcode, 1);
}

bool ClientProtocol::recieveMessage(EventClient& out_evento) {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) {
        out_evento.type = TypeEventClient::DESCONEXION;
        return false;
    }

    switch (opcode) {
        case ServerOpcode::SNAPSHOT: {
            out_evento.type = TypeEventClient::ACTUALIZACION_MUNDO;
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
        case ServerOpcode::PLAYER_STATS: {
            out_evento.type = TypeEventClient::ESTADISTICAS_PROPIAS;
            out_evento.stats.opcode = opcode;

            socket.recvall(&out_evento.stats.hp, 4);
            out_evento.stats.hp = ntohl(out_evento.stats.hp);

            socket.recvall(&out_evento.stats.mana, 4);
            out_evento.stats.mana = ntohl(out_evento.stats.mana);

            socket.recvall(&out_evento.stats.oro, 4);
            out_evento.stats.oro = ntohl(out_evento.stats.oro);

            socket.recvall(&out_evento.stats.exp, 4);
            out_evento.stats.exp = ntohl(out_evento.stats.exp);

            socket.recvall(&out_evento.stats.nivel, 1);
            break;
        }
        case ServerOpcode::LOGIN_RESPONSE: {
            out_evento.type = TypeEventClient::RESPUESTA_LOGIN;
            uint8_t success;
            socket.recvall(&success, 1);
            
            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            out_evento.text_payload.resize(len);
            socket.recvall(out_evento.text_payload.data(), len);
            break;
        }
        case ServerOpcode::CHANGE_MAP: {
            out_evento.type = TypeEventClient::CAMBIO_MAPA;
            socket.recvall(&out_evento.map_id, 2);
            out_evento.map_id = ntohs(out_evento.map_id);
            break;
        }
        case ServerOpcode::CHAT_MSG:
        case ServerOpcode::ACTION_ERROR: {
            out_evento.type = (opcode == ServerOpcode::CHAT_MSG) ? 
                              TypeEventClient::MENSAJE_CHAT : TypeEventClient::ERROR_ACCION;
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