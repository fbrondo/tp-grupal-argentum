#include "server/includes/server_protocol.h"

#include <cstring>

#include <arpa/inet.h>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

ServerProtocol::ServerProtocol(Socket& s): socket(s) {}

void ServerProtocol::sendSnapshot(const Snapshot& state) const {
    const size_t size_total = sizeof(uint8_t)     // Opcode
                              + sizeof(uint16_t)  // Cantidad de jugadores
                              + (state.players.size() * sizeof(PlayerSnapshotData)) +
                              sizeof(uint16_t)  // Cantidad de NPCs
                              + (state.npcs.size() * sizeof(NpcSnapshotData)) +
                              sizeof(uint16_t)  // Cantidad de Items
                              + (state.items_on_floor.size() * sizeof(ItemGroundSnapshotData));

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = SNAPSHOT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    // Jugadores
    const auto p_size = static_cast<uint16_t>(state.players.size());
    const uint16_t p_count_net = htons(p_size);
    std::memcpy(buffer.data() + offset, &p_count_net, sizeof(p_count_net));
    offset += sizeof(p_count_net);

    for (auto p: state.players) {
        p.id = htonl(p.id);
        p.pos_x = htonl(p.pos_x);
        p.pos_y = htonl(p.pos_y);
        p.max_hp = htons(p.max_hp);
        p.hp = htons(p.hp);
        p.body_id = htons(p.body_id);
        p.head_id = htons(p.head_id);
        p.weapon_id = htons(p.weapon_id);

        std::memcpy(buffer.data() + offset, &p, sizeof(PlayerSnapshotData));
        offset += sizeof(PlayerSnapshotData);
    }

    // NPCs
    const auto n_size = static_cast<uint16_t>(state.npcs.size());
    const uint16_t n_count_net = htons(n_size);
    std::memcpy(buffer.data() + offset, &n_count_net, sizeof(n_count_net));
    offset += sizeof(n_count_net);

    for (auto n: state.npcs) {
        n.id = htonl(n.id);
        n.pos_x = htonl(n.pos_x);
        n.pos_y = htonl(n.pos_y);
        n.type_id = htons(n.type_id);
        n.hp_actual = htons(n.hp_actual);

        std::memcpy(buffer.data() + offset, &n, sizeof(NpcSnapshotData));
        offset += sizeof(NpcSnapshotData);
    }

    // Items en el piso
    const auto i_size = static_cast<uint16_t>(state.items_on_floor.size());
    const uint16_t i_count_net = htons(i_size);
    std::memcpy(buffer.data() + offset, &i_count_net, sizeof(i_count_net));
    offset += sizeof(i_count_net);

    for (auto i: state.items_on_floor) {
        i.item_id = htons(i.item_id);
        i.pos_x = htonl(i.pos_x);
        i.pos_y = htonl(i.pos_y);

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

void ServerProtocol::sendPlayerStats(const MsgPlayerStats& stats) const {
    MsgPlayerStats temp_stats = stats;
    temp_stats.hp = htonl(stats.hp);
    temp_stats.mana = htonl(stats.mana);
    temp_stats.gold = htonl(stats.gold);
    temp_stats.exp = htonl(stats.exp);
    temp_stats.level = stats.level;
    try {
        socket.sendall(&temp_stats, sizeof(MsgPlayerStats));
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendPlayerStats -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::sendInventoryUpdate(const MsgInventoryUpdate& inv) const {
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

void ServerProtocol::sendLoginResponse(const bool success, const std::string& message) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) + message.size();

    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = LOGIN_RESPONSE;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint8_t res = success ? 1 : 0;
    std::memcpy(buffer.data() + offset, &res, sizeof(res));
    offset += sizeof(res);

    const uint16_t len = htons(static_cast<uint16_t>(message.size()));
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

void ServerProtocol::sendChangeMap(const uint16_t map_id) const {
    constexpr size_t total_size = sizeof(uint8_t) + sizeof(uint16_t);

    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = CHANGE_MAP;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t net_map_id = htons(map_id);
    std::memcpy(buffer.data() + offset, &net_map_id, sizeof(net_map_id));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendChangeMap -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::sendChatMsg(const std::string& message) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint16_t) + message.size();

    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = CHAT_MSG;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t len = htons(static_cast<uint16_t>(message.size()));
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

void ServerProtocol::sendActionError(const std::string& error_msg) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint16_t) + error_msg.size();

    std::vector<char> buffer(total_size);
    size_t offset = 0;

    constexpr uint8_t opcode = ACTION_ERROR;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t len = htons(static_cast<uint16_t>(error_msg.size()));
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

void ServerProtocol::sendMap(const Map& map) {
    // Opcode (1B) + Width (4B) + Height (4B) + (Cantidad de Tiles * Tamaño de 1 Tile)
    // Cada Tile tiene sprite_id (4B) + walkable (1B) = 5B.
    const size_t total_tiles = map.width() * map.height() * layer_count;
    const size_t size_total =
            sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + (total_tiles * 5);

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = MAP_DATA;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t w_net = htonl(static_cast<uint32_t>(map.width()));
    std::memcpy(buffer.data() + offset, &w_net, sizeof(w_net));
    offset += sizeof(w_net);

    uint32_t h_net = htonl(static_cast<uint32_t>(map.height()));
    std::memcpy(buffer.data() + offset, &h_net, sizeof(h_net));
    offset += sizeof(h_net);

    std::array<Layer, layer_count> layers = {Layer::Background, Layer::Details, Layer::Object,
                                             Layer::Roof};

    for (Layer layer: layers) {
        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                const Tile& tile = map.tile_at(x, y, layer);

                // Convertimos y copiamos el sprite_id (4 bytes)
                int32_t sprite_id_net = htonl(tile.sprite_id);
                std::memcpy(buffer.data() + offset, &sprite_id_net, sizeof(sprite_id_net));
                offset += sizeof(sprite_id_net);

                // Convertimos y copiamos el walkable (1 byte)
                uint8_t walkable_byte = tile.walkable ? 1 : 0;
                std::memcpy(buffer.data() + offset, &walkable_byte, sizeof(walkable_byte));
                offset += sizeof(walkable_byte);
            }
        }
    }
    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en sendMap -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

bool ServerProtocol::readCommand(uint32_t player_id, Queue<std::unique_ptr<Command>>& queue) {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0)
        return false;

    switch (opcode) {
        case LOGIN: {
            MsgLogin login;
            socket.recvall(login.name, sizeof(login.name));
            socket.recvall(login.pass, sizeof(login.pass));
            login.name[sizeof(login.name) - 1] = '\0';
            login.pass[sizeof(login.pass) - 1] = '\0';
            auto cmd = std::make_unique<LoginCommand>(player_id, std::string(login.name),
                                                      std::string(login.pass));

            queue.push(std::move(cmd));
            break;
        }
        case MOVE: {
            uint8_t dir;
            socket.recvall(&dir, 1);
            auto cmd = std::make_unique<MoveCommand>(player_id, dir);
            queue.push(std::move(cmd));
            break;
        }
        case ATTACK: {
            uint32_t target_id;
            socket.recvall(&target_id, 4);
            auto cmd = std::make_unique<AttackCommand>(player_id, ntohl(target_id));
            queue.push(std::move(cmd));
            break;
        }
        case CHAT:
        case COMMAND: {
            uint16_t len;
            socket.recvall(&len, 2);
            len = ntohs(len);
            std::string texto(len, '\0');
            socket.recvall(texto.data(), len);
            auto cmd = std::make_unique<ChatCommand>(player_id, std::move(texto));
            queue.push(std::move(cmd));
            break;
        }
        case USE_ITEM:
        case DROP_ITEM: {
            uint8_t slot;
            socket.recvall(&slot, 1);
            std::unique_ptr<Command> cmd;
            if (opcode == ClientOpcode::USE_ITEM) {
                cmd = std::make_unique<UseItemCommand>(player_id, slot);
            } else {
                cmd = std::make_unique<DropItemCommand>(player_id, slot);
            }
            queue.push(std::move(cmd));
            break;
        }
        case INTERACT: {
            uint32_t npc_id;
            socket.recvall(&npc_id, 4);
            auto cmd = std::make_unique<InteractCommand>(player_id, ntohl(npc_id));
            queue.push(std::move(cmd));
            break;
        }
        case TAKE_ITEM: {
            auto cmd = std::make_unique<TakeItemCommand>(player_id);
            queue.push(std::move(cmd));
            break;
        }
        case BUY_ITEM:
        case SELL_ITEM: {
            uint32_t npc_id;
            uint16_t item_id;
            uint16_t quantity;

            socket.recvall(&npc_id, 4);
            socket.recvall(&item_id, 2);
            socket.recvall(&quantity, 2);

            npc_id = ntohl(npc_id);
            item_id = ntohs(item_id);
            quantity = ntohs(quantity);
            std::unique_ptr<Command> cmd;
            if (opcode == BUY_ITEM) {
                cmd = std::make_unique<BuyItemCommand>(player_id, npc_id, item_id, quantity);
            } else {
                cmd = std::make_unique<SellItemCommand>(player_id, npc_id, item_id, quantity);
            }
            queue.push(std::move(cmd));
            break;
        }
        case DISCONNECT: {
            auto cmd = std::make_unique<DisconnectCommand>(player_id);
            queue.push(std::move(cmd));

            // Devolvemos false para que el cliente deje de recibir
            return false;
        }
        default:
            return false;
    }
    return true;
}

void ServerProtocol::shutdown_peer() {
    try {
        socket.shutdown(1);
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en shutdown_peer -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}

void ServerProtocol::close_peer() const {
    try {
        socket.close();
    } catch (const std::exception& e) {
        std::string mssgErr = "Error en close_peer -- ";
        mssgErr += e.what();
        throw std::runtime_error(mssgErr);
    }
}
