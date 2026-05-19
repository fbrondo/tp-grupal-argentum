#include "server_protocol.h"
#include "command_queue.h"
#include <arpa/inet.h>
#include <cstring>

ServerProtocol::ServerProtocol(Socket& s) : socket(s) {}

void ServerProtocolo::sendSnapshot(const WorldState& state) {
    size_t size_total = sizeof(uint8_t)   // Opcode
                        + sizeof(uint16_t)  // Cantidad de jugadores
                        + (state.players.size() * sizeof(PlayerSnapshotData))
                        + sizeof(uint16_t)  // Cantidad de NPCs
                        + (state.npcs.size() * sizeof(NpcSnapshotData))
                        + sizeof(uint16_t)  // Cantidad de Items
                        + (state.items.size() * sizeof(ItemGroundSnapshotData));

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    uint8_t opcode = ServerOpcode::SNAPSHOT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    // Jugadores
    uint16_t p_size = static_cast<uint16_t>(state.players.size());
    uint16_t p_count_net = htons(p_size);
    std::memcpy(buffer.data() + offset, &p_count_net, sizeof(p_count_net));
    offset += sizeof(p_count_net);
    
    for (auto p : state.players) {
        p.id = htonl(p.id);
        p.hp_max = htons(p.hp_max);       
        p.hp_actual = htons(p.hp_actual);
        p.body_id = htons(p.body_id);
        p.head_id = htons(p.head_id);
        p.weapon_id = htons(p.weapon_id);

        std::memcpy(buffer.data() + offset, &p, sizeof(PlayerSnapshotData));
        offset += sizeof(PlayerSnapshotData);
    }

    // NPCs
    uint16_t n_size = static_cast<uint16_t>(state.npcs.size());
    uint16_t n_count_net = htons(n_size);
    std::memcpy(buffer.data() + offset, &n_count_net, sizeof(n_count_net));
    offset += sizeof(n_count_net);
    
    for (auto n : state.npcs) {
        n.id = htonl(n.id);
        n.type_id = htons(n.type_id);
        n.hp_actual = htons(n.hp_actual);

        std::memcpy(buffer.data() + offset, &n, sizeof(NpcSnapshotData));
        offset += sizeof(NpcSnapshotData);
    }

    // Items en el piso
    uint16_t i_size = static_cast<uint16_t>(state.items.size());
    uint16_t i_count_net = htons(i_size);
    std::memcpy(buffer.data() + offset, &i_count_net, sizeof(i_count_net));
    offset += sizeof(i_count_net);
    
    for (auto i : state.items) {
        i.item_id = htons(i.item_id);

        std::memcpy(buffer.data() + offset, &i, sizeof(ItemGroundSnapshotData));
        offset += sizeof(ItemGroundSnapshotData);
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendSnapshot -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::sendPlayerStats(const MsgPlayerStats& stats) {
    MsgPlayerStats temp_stats = stats;
    temp_stats.hp = htonl(stats.hp);
    temp_stats.mana = htonl(stats.mana);
    temp_stats.gold = htonl(stats.gold);
    temp_stats.exp = htonl(stats.exp);
    try {
        socket.sendall(&temp_stats, sizeof(MsgPlayerStats));
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendPlayerStats -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::sendInventoryUpdate(const MsgInventoryUpdate& inv) {
    MsgInventoryUpdate temp_inv = inv;
    temp_inv.item_id = htons(inv.item_id);
    temp_inv.quantity = htons(inv.quantity);
    try {
        socket.sendall(&temp_inv, sizeof(MsgInventoryUpdate));
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendInventoryUpdate -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::sendLoginResponse(bool success, const std::string& message) {
    size_t tamaño_total = sizeof(uint8_t) 
                        + sizeof(uint8_t) 
                        + sizeof(uint16_t) 
                        + message.size();

    std::vector<char> buffer(tamaño_total);
    size_t offset = 0;

    uint8_t opcode = ServerOpcode::LOGIN_RESPONSE;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);
    
    uint8_t res = success ? 1 : 0;
    std::memcpy(buffer.data() + offset, &res, sizeof(res));
    offset += sizeof(res);

    uint16_t len = htons(static_cast<uint16_t>(message.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!message.empty()) {
        std::memcpy(buffer.data() + offset, message.data(), message.size());
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendLoginResponse -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocolo::sendChangeMap(uint16_t map_id) {
    size_t tamaño_total = sizeof(uint8_t) + sizeof(uint16_t);

    std::vector<char> buffer(tamaño_total);
    size_t offset = 0;

    uint8_t opcode = ServerOpcode::CHANGE_MAP;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint16_t net_map_id = htons(map_id);
    std::memcpy(buffer.data() + offset, &net_map_id, sizeof(net_map_id));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendChangeMap -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocolo::sendChatMsg(const std::string& message) {
    size_t tamaño_total = sizeof(uint8_t) + sizeof(uint16_t) + message.size();

    std::vector<char> buffer(tamaño_total);
    size_t offset = 0;

    uint8_t opcode = ServerOpcode::CHAT_MSG;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint16_t len = htons(static_cast<uint16_t>(message.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!message.empty()) {
        std::memcpy(buffer.data() + offset, message.data(), message.size());
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendChatMsg -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocolo::sendActionError(const std::string& error_msg) {
    size_t tamaño_total = sizeof(uint8_t) + sizeof(uint16_t) + error_msg.size();

    std::vector<char> buffer(tamaño_total);
    size_t offset = 0;

    uint8_t opcode = ServerOpcode::ACTION_ERROR;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint16_t len = htons(static_cast<uint16_t>(error_msg.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!error_msg.empty()) {
        std::memcpy(buffer.data() + offset, error_msg.data(), error_msg.size());
    }

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendActionError -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

//Asumo que tendremos una estructura Comando y Accion para agregar eventos a la queue
bool ServerProtocol::readCommand(uint32_t player_id, CommandQueue& queue) {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) return false;

    switch (opcode) {
        case ClientOpcode::LOGIN: {
            MsgLogin login;
            socket.recvall(login.name, sizeof(login.name));
            socket.recvall(login.pass, sizeof(login.pass));
            login.name[sizeof(login.name) - 1] = '\0';
            login.pass[sizeof(login.pass) - 1] = '\0';
            queue.push(Comando(player_id, Accion::LOGIN, std::string(login.name), std::string(login.pass)));
            break;
        }
        case ClientOpcode::MOVE: {
            uint8_t dir;
            socket.recvall(&dir, 1);
            queue.push(Comando(player_id, Accion::MOVER, dir));
            break;
        }
        case ClientOpcode::ATTACK: {
            uint32_t target_id;
            socket.recvall(&target_id, 4);
            queue.push(Comando(player_id, Accion::ATACAR, ntohl(target_id)));
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
            queue.push(Comando(player_id, tipo, texto));
            break;
        }
        case ClientOpcode::USE_ITEM:
        case ClientOpcode::DROP_ITEM: {
            uint8_t slot;
            socket.recvall(&slot, 1);
            Accion tipo = (opcode == ClientOpcode::USE_ITEM) ? Accion::EQUIPAR : Accion::TIRAR;
            queue.push(Comando(player_id, tipo, slot));
            break;
        }
        case ClientOpcode::INTERACT: {
            uint32_t npc_id;
            socket.recvall(&npc_id, 4);
            queue.push(Comando(player_id, Accion::INTERACTUAR, ntohl(npc_id)));
            break;
        }
        case ClientOpcode::TAKE_ITEM: {
            queue.push(Comando(player_id, Accion::AGARRAR_ITEM));
            break;
        }
        case ClientOpcode::BUY_ITEM:
        case ClientOpcode::SELL_ITEM: {
            uint32_t npc_id;
            uint16_t item_id;
            uint16_t quantity;

            socket.recvall(&npc_id, 4);
            socket.recvall(&item_id, 2);
            socket.recvall(&quantity, 2);

            npc_id = ntohl(npc_id);
            item_id = ntohs(item_id);
            quantity = ntohs(quantity);

            Accion tipo = (opcode == ClientOpcode::BUY_ITEM) ? Accion::COMPRAR : Accion::VENDER;
            
            queue.push(Comando(player_id, tipo, npc_id, item_id, quantity));
            break;
        }
        default:
            return false; 
    }
    return true;
}