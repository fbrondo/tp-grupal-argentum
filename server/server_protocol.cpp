#include "server_protocol.h"
#include "cola_comandos.h"
#include <arpa/inet.h>
#include <cstring>

ServerProtocolo::ServerProtocolo(Socket& s) : socket(s) {}

void ServerProtocolo::sendSnapshot(const WorldState& state) {
    uint8_t opcode = ServerOpcode::SNAPSHOT;
    socket.sendall(&opcode, 1);

    // 1. Enviar Jugadores
    uint16_t p_count = htons(static_cast<uint16_t>(state.players.size()));
    socket.sendall(&p_count, 2);
    for (auto p : state.players) {
        p.id = htonl(p.id);
        p.hp_max = htons(p.hp_max);
        p.hp_actual = htons(p.hp_actual);
        p.body_id = htons(p.body_id);
        p.head_id = htons(p.head_id);
        p.weapon_id = htons(p.weapon_id);
        socket.sendall(&p, sizeof(PlayerSnapshotData));
    }

    // 2. Enviar NPCs
    uint16_t n_count = htons(static_cast<uint16_t>(state.npcs.size()));
    socket.sendall(&n_count, 2);
    for (auto n : state.npcs) {
        n.id = htonl(n.id);
        n.type_id = htons(n.type_id);
        n.hp_actual = htons(n.hp_actual);
        socket.sendall(&n, sizeof(NpcSnapshotData));
    }

    // 3. Enviar Items tirados
    uint16_t i_count = htons(static_cast<uint16_t>(state.items.size()));
    socket.sendall(&i_count, 2);
    for (auto i : state.items) {
        i.item_id = htons(i.item_id);
        socket.sendall(&i, sizeof(ItemGroundSnapshotData));
    }
}

void ServerProtocolo::sendPlayerStats(const MsgPlayerStats& stats) {
    MsgPlayerStats temp_stats = stats;
    temp_stats.hp = htonl(stats.hp);
    temp_stats.mana = htonl(stats.mana);
    temp_stats.oro = htonl(stats.oro);
    temp_stats.exp = htonl(stats.exp);
    socket.sendall(&temp_stats, sizeof(MsgPlayerStats));
}

void ServerProtocolo::sendInventoryUpdate(const MsgInventoryUpdate& inv) {
    MsgInventoryUpdate temp_inv = inv;
    temp_inv.item_id = htons(inv.item_id);
    temp_inv.quantity = htons(inv.quantity);
    socket.sendall(&temp_inv, sizeof(MsgInventoryUpdate));
}

void ServerProtocolo::sendLoginResponse(bool success, const std::string& message) {
    uint8_t opcode = ServerOpcode::LOGIN_RESPONSE;
    socket.sendall(&opcode, 1);
    
    uint8_t res = success ? 1 : 0;
    socket.sendall(&res, 1);

    uint16_t len = htons(static_cast<uint16_t>(message.size()));
    socket.sendall(&len, 2);
    socket.sendall(message.data(), message.size());
}

void ServerProtocolo::sendChangeMap(uint16_t map_id) {
    uint8_t opcode = ServerOpcode::CHANGE_MAP;
    socket.sendall(&opcode, 1);

    uint16_t net_map_id = htons(map_id);
    socket.sendall(&net_map_id, 2);
}

void ServerProtocolo::sendChatMsg(const std::string& message) {
    uint8_t opcode = ServerOpcode::CHAT_MSG;
    socket.sendall(&opcode, 1);

    uint16_t len = htons(static_cast<uint16_t>(message.size()));
    socket.sendall(&len, 2);
    socket.sendall(message.data(), message.size());
}

void ServerProtocolo::sendActionError(const std::string& error_msg) {
    uint8_t opcode = ServerOpcode::ACTION_ERROR;
    socket.sendall(&opcode, 1);

    uint16_t len = htons(static_cast<uint16_t>(error_msg.size()));
    socket.sendall(&len, 2);
    socket.sendall(error_msg.data(), error_msg.size());
}

//Asumo que tendremos una estructura Comando y Accion para agregar eventos a la cola
bool ServerProtocolo::readCommand(uint32_t player_id, ColaComandos& cola) {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) return false;

    switch (opcode) {
        case ClientOpcode::LOGIN: {
            MsgLogin login;
            socket.recvall(login.name, 30);
            socket.recvall(login.pass, 30);
            cola.push(Comando(player_id, Accion::LOGIN, std::string(login.name), std::string(login.pass)));
            break;
        }
        case ClientOpcode::MOVE: {
            uint8_t dir;
            socket.recvall(&dir, 1);
            cola.push(Comando(player_id, Accion::MOVER, dir));
            break;
        }
        case ClientOpcode::ATTACK: {
            uint32_t target_id;
            socket.recvall(&target_id, 4);
            cola.push(Comando(player_id, Accion::ATACAR, ntohl(target_id)));
            break;
        }
        case ClientOpcode::CHAT: 
        case ClientOpcode::COMMAND: {
            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            std::string texto(len, '\0');
            socket.recvall(texto.data(), len);
            
            Accion tipo = (opcode == ClientOpcode::CHAT) ? Accion::CHAT : Accion::EJECUTAR_COMANDO;
            cola.push(Comando(player_id, tipo, texto));
            break;
        }
        case ClientOpcode::USE_ITEM:
        case ClientOpcode::DROP_ITEM: {
            uint8_t slot;
            socket.recvall(&slot, 1);
            Accion tipo = (opcode == ClientOpcode::USE_ITEM) ? Accion::EQUIPAR : Accion::TIRAR;
            cola.push(Comando(player_id, tipo, slot));
            break;
        }
        case ClientOpcode::INTERACT: {
            uint32_t npc_id;
            socket.recvall(&npc_id, 4);
            cola.push(Comando(player_id, Accion::INTERACTUAR, ntohl(npc_id)));
            break;
        }
        case ClientOpcode::TAKE_ITEM: {
            cola.push(Comando(player_id, Accion::AGARRAR_ITEM));
            break;
        }
        default:
            return false; 
    }
    return true;
}