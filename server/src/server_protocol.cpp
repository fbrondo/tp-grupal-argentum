#include "../includes/server_protocol.h"

#include <cstring>

#include <arpa/inet.h>

#include "common/includes/direction.h"
#include "common/includes/map/layer.h"
#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

ServerProtocol::ServerProtocol(Socket& s): socket(s) {}

void ServerProtocol::sendSnapshot(const Snapshot& state) const {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint16_t) +
                              (state.players.size() * sizeof(PlayerSnapshotData)) +
                              sizeof(uint16_t) + (state.npcs.size() * sizeof(NpcSnapshotData)) +
                              sizeof(uint16_t) +
                              (state.items_on_floor.size() * sizeof(ItemGroundSnapshotData));

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = SNAPSHOT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t p_count_net = htons(static_cast<uint16_t>(state.players.size()));
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

    const uint16_t n_count_net = htons(static_cast<uint16_t>(state.npcs.size()));
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

    const uint16_t i_count_net = htons(static_cast<uint16_t>(state.items_on_floor.size()));
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
        throw std::runtime_error(std::string("ERROR IN sendSnapshot -- ") + e.what());
    }
}

void ServerProtocol::sendPlayerStats(const MsgPlayerStats& stats) const {
    MsgPlayerStats temp = stats;
    temp.hp = htonl(stats.hp);
    temp.mana = htonl(stats.mana);
    temp.gold = htonl(stats.gold);
    temp.exp = htonl(stats.exp);
    temp.level = stats.level;
    try {
        socket.sendall(&temp, sizeof(MsgPlayerStats));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendPlayerStats -- ") + e.what());
    }
}

void ServerProtocol::sendInventoryUpdate(const MsgInventoryUpdate& inv) const {
    MsgInventoryUpdate temp = inv;
    temp.item_id = htons(inv.item_id);
    temp.quantity = htons(inv.quantity);
    try {
        socket.sendall(&temp, sizeof(MsgInventoryUpdate));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendInventoryUpdate -- ") + e.what());
    }
}

void ServerProtocol::sendSimpleResponse(uint8_t opcode, bool success,
                                        const std::string& msg) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) + msg.size();

    std::vector<char> buffer(total_size);
    size_t offset = 0;

    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint8_t res = success ? 1 : 0;
    std::memcpy(buffer.data() + offset, &res, sizeof(res));
    offset += sizeof(res);

    const uint16_t len = htons(static_cast<uint16_t>(msg.size()));
    std::memcpy(buffer.data() + offset, &len, sizeof(len));
    offset += sizeof(len);

    if (!msg.empty())
        std::memcpy(buffer.data() + offset, msg.data(), msg.size());

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendSimpleResponse -- ") + e.what());
    }
}

void ServerProtocol::sendLoginResponse(bool success, const std::string& msg) const {
    sendSimpleResponse(LOGIN_RESPONSE, success, msg);
}

void ServerProtocol::sendSignupResponse(bool success, const std::string& msg) const {
    sendSimpleResponse(SIGNUP_RESPONSE, success, msg);
}

void ServerProtocol::sendCharacterCreateResponse(bool success, const std::string& msg) const {
    sendSimpleResponse(CHARACTER_CREATE_RESPONSE, success, msg);
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
        throw std::runtime_error(std::string("ERROR IN sendChangeMap -- ") + e.what());
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

    if (!message.empty())
        std::memcpy(buffer.data() + offset, message.data(), message.size());

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendChatMsg -- ") + e.what());
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

    if (!error_msg.empty())
        std::memcpy(buffer.data() + offset, error_msg.data(), error_msg.size());

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendActionError -- ") + e.what());
    }
}

void ServerProtocol::sendMap(const Map& map) {
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
                int32_t sprite_id_net = htonl(tile.sprite_id);
                std::memcpy(buffer.data() + offset, &sprite_id_net, sizeof(sprite_id_net));
                offset += sizeof(sprite_id_net);
                uint8_t walkable_byte = tile.walkable ? 1 : 0;
                std::memcpy(buffer.data() + offset, &walkable_byte, sizeof(walkable_byte));
                offset += sizeof(walkable_byte);
            }
        }
    }
    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendMap -- ") + e.what());
    }
}

void ServerProtocol::sendTraderCatalog(const std::map<TypeItem, uint32_t>& catalog) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint16_t) + (catalog.size() * 5);

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = TRADER_CATALOG;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint16_t net_total_items = htons(static_cast<uint16_t>(catalog.size()));
    std::memcpy(buffer.data() + offset, &net_total_items, sizeof(net_total_items));
    offset += sizeof(net_total_items);

    for (auto& [item_type, price]: catalog) {
        uint8_t type_byte = static_cast<uint8_t>(item_type);
        std::memcpy(buffer.data() + offset, &type_byte, sizeof(type_byte));
        offset += sizeof(type_byte);

        uint32_t net_price = htonl(price);
        std::memcpy(buffer.data() + offset, &net_price, sizeof(net_price));
        offset += sizeof(net_price);
    }
    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendTraderCatalog -- ") + e.what());
    }
}

void ServerProtocol::sendBankContent(const std::vector<MsgItemInfo>& items, uint32_t gold) {
    const size_t size_items = items.size() * sizeof(MsgItemInfo);
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint16_t) + size_items;

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = BANK_CONTENT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t net_gold = htonl(gold);
    std::memcpy(buffer.data() + offset, &net_gold, sizeof(net_gold));
    offset += sizeof(net_gold);

    uint16_t net_total_items = htons(static_cast<uint16_t>(items.size()));
    std::memcpy(buffer.data() + offset, &net_total_items, sizeof(net_total_items));
    offset += sizeof(net_total_items);

    if (!items.empty()) {
        std::memcpy(buffer.data() + offset, items.data(), size_items);
        offset += size_items;
    }
    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendBankContent -- ") + e.what());
    }
}

bool ServerProtocol::readCommand(Id player_id, QueueCmd& queue) {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0)
        return false;

    switch (opcode) {
        case LOGIN: {
            MsgLogin login;
            this->socket.recvall(login.name, sizeof(login.name));
            this->socket.recvall(login.pass, sizeof(login.pass));
            login.name[sizeof(login.name) - 1] = '\0';
            login.pass[sizeof(login.pass) - 1] = '\0';
            queue.push(std::make_unique<LoginCommand>(
                    static_cast<Id>(player_id), std::string(login.name), std::string(login.pass)));
            break;
        }
        case SIGNUP: {
            MsgSignup signup;
            // size_t bytes_restantes = sizeof(MsgSignup) - sizeof(signup.opcode);
            // socket.recvall(&signup.user, bytes_restantes);
            socket.recvall(signup.user, sizeof(signup.user));
            socket.recvall(signup.password, sizeof(signup.password));
            socket.recvall(&signup.traits.head, sizeof(signup.traits.head));
            socket.recvall(&signup.traits.body, sizeof(signup.traits.body));
            socket.recvall(&signup.traits.race, sizeof(signup.traits.race));
            socket.recvall(&signup.traits.clase, sizeof(signup.traits.clase));
            signup.user[sizeof(signup.user) - 1] = '\0';
            signup.password[sizeof(signup.password) - 1] = '\0';
            // CharacterTraits traits = signup.traits;

            queue.push(std::make_unique<SignupCommand>(player_id, std::string(signup.user),
                                                       std::string(signup.password),
                                                       std::move(signup.traits)));
            break;
        }
        // case CHARACTER_CREATE: {
        //     MsgCharacterCreate msg;
        //     socket.recvall(msg.name, sizeof(msg.name));
        //     socket.recvall(&msg.race, sizeof(msg.race));
        //     socket.recvall(&msg.clase, sizeof(msg.clase));
        //     msg.name[sizeof(msg.name) - 1] = '\0';
        //     queue.push(std::make_unique<CreateCharacterCommand>(
        //             player_id, std::string(msg.name), std::string(""),
        //             static_cast<TypeRace>(msg.race), static_cast<TypeClase>(msg.clase)));
        //     break;
        // }
        case MOVE: {
            uint8_t dir;
            this->socket.recvall(&dir, sizeof(dir));
            queue.push(std::make_unique<MoveCommand>(player_id, dir));
            break;
        }
        case ATTACK: {
            uint32_t target_id;
            socket.recvall(&target_id, sizeof(target_id));
            queue.push(std::make_unique<AttackCommand>(player_id, ntohl(target_id)));
            break;
        }
        case CHAT:
        case COMMAND: {
            uint16_t len;
            socket.recvall(&len, sizeof(len));
            len = ntohs(len);
            std::string texto(len, '\0');
            socket.recvall(texto.data(), len);
            queue.push(std::make_unique<ChatCommand>(player_id, std::move(texto)));
            break;
        }
        case USE_ITEM:
        case DROP_ITEM: {
            uint32_t instance_id;
            socket.recvall(&instance_id, 4);
            if (opcode == USE_ITEM) {
                queue.push(std::make_unique<UseItemCommand>(player_id, ntohl(instance_id)));
            } else {
                queue.push(std::make_unique<DropItemCommand>(player_id, ntohl(instance_id)));
            }
            break;
        }
        case INTERACT: {
            uint32_t npc_id;
            socket.recvall(&npc_id, 4);
            queue.push(std::make_unique<InteractCommand>(player_id, ntohl(npc_id)));
            break;
        }
        case TAKE_ITEM: {
            queue.push(std::make_unique<TakeItemCommand>(player_id));
            break;
        }
        case BUY_ITEM:
        case SELL_ITEM: {
            uint32_t network_npc_id;
            uint32_t network_item_id;

            socket.recvall(&network_npc_id, sizeof(network_npc_id));
            socket.recvall(&network_item_id, sizeof(network_item_id));

            Id npc_id = ntohl(network_npc_id);

            Id item_id = ntohl(network_item_id);

            if (opcode == BUY_ITEM) {
                queue.push(std::make_unique<BuyItemCommand>(player_id, npc_id, item_id));
            } else {
                queue.push(std::make_unique<SellItemCommand>(player_id, npc_id, item_id));
            }
            break;
        }
        case DEPOSIT_ITEM:
        case WITHDRAW_ITEM: {
            Id item_id;
            socket.recvall(&item_id, 2);
            item_id = ntohs(item_id);

            if (opcode == DEPOSIT_ITEM) {
                queue.push(std::make_unique<DepositItemCommand>(player_id, item_id));
            } else {
                queue.push(std::make_unique<WithdrawItemCommand>(player_id, item_id));
            }
            break;
        }
        case DEPOSIT_GOLD:
        case WITHDRAW_GOLD: {
            uint32_t amount;
            socket.recvall(&amount, 4);
            amount = ntohl(amount);

            if (opcode == DEPOSIT_GOLD) {
                queue.push(std::make_unique<DepositGoldCommand>(player_id, amount));
            } else {
                queue.push(std::make_unique<WithdrawGoldCommand>(player_id, amount));
            }
            break;
        }
        case LIST_ITEMS: {
            Id npc_id;
            socket.recvall(&npc_id, 4);
            npc_id = ntohl(npc_id);

            queue.push(std::make_unique<ListItemsCommand>(player_id, npc_id));
            break;
        }
        case DISCONNECT: {
            queue.push(std::make_unique<DisconnectCommand>(player_id));
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
        throw std::runtime_error(std::string("ERROR IN shutdown_peer -- ") + e.what());
    }
}

void ServerProtocol::close_peer() const {
    try {
        socket.close();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN close_peer -- ") + e.what());
    }
}
