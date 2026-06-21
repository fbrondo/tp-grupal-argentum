#include "../includes/server_protocol.h"

#include <cstring>
#include <iostream>

#include <arpa/inet.h>

#include "common/includes/direction.h"
#include "common/includes/map/layer.h"
#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

ServerProtocol::ServerProtocol(Socket& s): socket(s) {}

void ServerProtocol::sendSnapshot(const Snapshot& state) const {
    const size_t size_total =
            sizeof(uint8_t) + sizeof(uint16_t) +
            (state.players.size() * sizeof(PlayerSnapshotData)) + sizeof(uint16_t) +
            (state.npcs.size() * sizeof(NpcSnapshotData)) + sizeof(uint16_t) +
            (state.items_on_floor.size() * sizeof(ItemGroundSnapshotData)) + sizeof(uint16_t) +
            /*(state.gold_piles.size() * sizeof(GoldPileGroundSnapshotData)) + sizeof(uint16_t) +*/
            (state.sound_effects.size() * sizeof(SoundEffectSnapshotData)) + sizeof(uint16_t) +
            (state.visual_effects.size() * sizeof(VisualEffectSnapshotData));

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = SNAPSHOT;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    const uint16_t count_players = htons(static_cast<uint16_t>(state.players.size()));
    std::memcpy(buffer.data() + offset, &count_players, sizeof(count_players));
    offset += sizeof(count_players);

    for (auto p: state.players) {
        p.id = htonl(p.id);
        p.position.x = htonl(p.position.x);
        p.position.y = htonl(p.position.y);
        p.stats.max_hp = htons(p.stats.max_hp);
        p.stats.current_hp = htons(p.stats.current_hp);
        p.stats.current_mana = htons(p.stats.current_mana);
        p.stats.max_mana = htons(p.stats.max_mana);
        p.stats.xp = htonl(p.stats.xp);
        p.ch_traits.body = htons(p.ch_traits.body);
        p.ch_traits.head = htons(p.ch_traits.head);
        p.resurrection_time_left_ms = htons(p.resurrection_time_left_ms);
        std::memcpy(buffer.data() + offset, &p, sizeof(PlayerSnapshotData));
        offset += sizeof(PlayerSnapshotData);
    }

    // NPCs
    const uint16_t n_count_net = htons(static_cast<uint16_t>(state.npcs.size()));
    std::memcpy(buffer.data() + offset, &n_count_net, sizeof(n_count_net));
    offset += sizeof(n_count_net);

    for (auto n: state.npcs) {
        n.id = htonl(n.id);
        n.position.x = htonl(n.position.x);
        n.position.y = htonl(n.position.y);
        n.current_hp = htons(n.current_hp);
        n.max_hp = htons(n.max_hp);
        std::memcpy(buffer.data() + offset, &n, sizeof(NpcSnapshotData));
        offset += sizeof(NpcSnapshotData);
    }

    // Items en el suelo
    const uint16_t i_count_net = htons(static_cast<uint16_t>(state.items_on_floor.size()));
    std::memcpy(buffer.data() + offset, &i_count_net, sizeof(i_count_net));
    offset += sizeof(i_count_net);

    for (auto i: state.items_on_floor) {
        i.item_id = htons(i.item_id);
        i.position.x = htonl(i.position.x);
        i.position.y = htonl(i.position.y);
        std::memcpy(buffer.data() + offset, &i, sizeof(ItemGroundSnapshotData));
        offset += sizeof(ItemGroundSnapshotData);
    }

    // Piles de oro
    // const uint16_t g_count_net = htons(static_cast<uint16_t>(state.gold_piles.size()));
    // std::memcpy(buffer.data() + offset, &g_count_net, sizeof(g_count_net));
    // offset += sizeof(g_count_net);
    //
    // for (auto g: state.gold_piles) {
    //     g.amount = htonl(g.amount);
    //     g.pos_x = htonl(g.pos_x);
    //     g.pos_y = htonl(g.pos_y);
    //
    //     std::memcpy(buffer.data() + offset, &g, sizeof(GoldPileGroundSnapshotData));
    //     offset += sizeof(GoldPileGroundSnapshotData);
    // }

    // Efectos sonoros
    const uint16_t s_count_net = htons(static_cast<uint16_t>(state.sound_effects.size()));
    std::memcpy(buffer.data() + offset, &s_count_net, sizeof(s_count_net));
    offset += sizeof(s_count_net);

    for (auto s: state.sound_effects) {
        uint16_t id_numerico = htons(static_cast<uint16_t>(s.effect_id));
        s.effect_id = static_cast<SoundEffectID>(id_numerico);

        s.pos_x = htonl(s.pos_x);
        s.pos_y = htonl(s.pos_y);

        std::memcpy(buffer.data() + offset, &s, sizeof(SoundEffectSnapshotData));
        offset += sizeof(SoundEffectSnapshotData);
    }

    // Efectos visuales
    const uint16_t v_count_net = htons(static_cast<uint16_t>(state.visual_effects.size()));
    std::memcpy(buffer.data() + offset, &v_count_net, sizeof(v_count_net));
    offset += sizeof(v_count_net);

    for (auto v: state.visual_effects) {
        uint16_t id_numerico = htons(static_cast<uint16_t>(v.effect_id));
        v.effect_id = static_cast<VisualEffectID>(id_numerico);

        v.recipient_id = htonl(v.recipient_id);
        v.pos_x = htonl(v.pos_x);
        v.pos_y = htonl(v.pos_y);

        std::memcpy(buffer.data() + offset, &v, sizeof(VisualEffectSnapshotData));
        offset += sizeof(VisualEffectSnapshotData);
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
    temp.max_hp = htonl(stats.max_hp);
    temp.mana = htonl(stats.mana);
    temp.max_mana = htonl(stats.max_mana);
    temp.safe_gold = htonl(stats.safe_gold);
    temp.excess_gold = htonl(stats.excess_gold);
    temp.exp = htonl(stats.exp);
    temp.exp_next_level = htonl(stats.exp_next_level);
    try {
        socket.sendall(&temp, sizeof(MsgPlayerStats));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendPlayerStats -- ") + e.what());
    }
}

void ServerProtocol::sendInventoryUpdate(const MsgInventoryUpdate& inv) const {
    MsgInventoryUpdate temp = inv;
    constexpr uint8_t opcode = INVENTORY_UPDATE;
    uint16_t size_inventory = htons(static_cast<uint16_t>(temp.inventory.size()));
    size_t size_buffer =
            sizeof(opcode) + sizeof(size_inventory) + (temp.inventory.size() * sizeof(MsgSlot));
    std::vector<char> buffer(size_buffer);
    size_t offset = 0;

    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);
    std::memcpy(buffer.data() + offset, &size_inventory, sizeof(size_inventory));
    offset += sizeof(size_inventory);
    for (auto& slot: temp.inventory) {
        slot.quantity = htons(slot.quantity);
        std::memcpy(buffer.data() + offset, &slot, sizeof(slot));
        offset += sizeof(slot);
    }
    // temp.item_id = htons(inv.item_id);
    // temp.quantity = htons(inv.quantity);
    try {
        // socket.sendall(&temp, sizeof(MsgInventoryUpdate));
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendInventoryUpdate -- ") + e.what());
    }
}

void ServerProtocol::sendEquipmentUpdate(const MsgEquipmentUpdate& equip) const {
    // MsgEquipmentUpdate temp = equip;
    constexpr uint8_t opcode = EQUIPMENT_UPDATE;
    uint16_t equipment = htons(static_cast<uint16_t>(equip.equipment.size()));
    size_t size_buffer =
            sizeof(opcode) + sizeof(equipment) + (equip.equipment.size() * sizeof(MsgSlot));
    std::vector<char> buffer(size_buffer);
    size_t offset = 0;

    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);
    std::memcpy(buffer.data() + offset, &equipment, sizeof(equipment));
    offset += sizeof(equipment);
    for (auto slot: equip.equipment) {
        slot.quantity = htons(slot.quantity);
        std::memcpy(buffer.data() + offset, &slot, sizeof(slot));
        offset += sizeof(slot);
    }
    try {
        // socket.sendall(&temp, sizeof(MsgEquipmentUpdate));
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendEquipmentUpdate -- ") + e.what());
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

void ServerProtocol::sendLoginResponse(bool success, Id player_id, const std::string& msg) const {
    sendSimpleResponse(LOGIN_RESPONSE, success, msg);
    const uint32_t player_id_net = htonl(player_id);
    socket.sendall(&player_id_net, sizeof(player_id_net));
}

void ServerProtocol::sendSignupResponse(bool success, const std::string& msg) const {
    sendSimpleResponse(SIGNUP_RESPONSE, success, msg);
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

void ServerProtocol::sendMap(const Map& map, const std::vector<CitizenNpcSnapshot>& citizen) {
    // std::cout << "[SERVER] sendMap: enviando mapa " << map.width() << "x" << map.height()
    //           << " tiles." << std::endl;
    const size_t total_tiles = map.width() * map.height() * layer_count;
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) +
                              (total_tiles * 6) + sizeof(uint16_t) +
                              (citizen.size() * sizeof(CitizenNpcSnapshot));

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
    for (const Layer layer: layers) {
        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                const auto& [sprite_id, walkable, region] = map.tile_at(x, y, layer);

                int32_t sprite_id_net = htonl(sprite_id);
                std::memcpy(buffer.data() + offset, &sprite_id_net, sizeof(sprite_id_net));
                offset += sizeof(sprite_id_net);

                uint8_t walkable_byte = walkable ? 1 : 0;
                std::memcpy(buffer.data() + offset, &walkable_byte, sizeof(walkable_byte));
                offset += sizeof(walkable_byte);

                uint8_t region_byte = static_cast<uint8_t>(region);
                std::memcpy(buffer.data() + offset, &region_byte, sizeof(region_byte));
                offset += sizeof(region_byte);
            }
        }
    }
    const uint16_t count_citizen = htons(static_cast<uint16_t>(citizen.size()));
    std::memcpy(buffer.data() + offset, &count_citizen, sizeof(count_citizen));
    offset += sizeof(count_citizen);
    for (auto n: citizen) {
        n.id = htonl(n.id);
        n.position.x = htonl(n.position.x);
        n.position.y = htonl(n.position.y);
        std::memcpy(buffer.data() + offset, &n, sizeof(CitizenNpcSnapshot));
        offset += sizeof(CitizenNpcSnapshot);
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

void ServerProtocol::sendBankContent(const std::map<TypeItem, uint32_t>& items, uint32_t gold) {
    const size_t size_items =
            items.size() * (sizeof(uint8_t) + sizeof(uint32_t)) /*sizeof(MsgItemInfo)*/;
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

    for (const auto& [type_item, count]: items) {
        uint8_t type_byte = static_cast<uint8_t>(type_item);
        uint16_t count_ = htons(count);
        std::memcpy(buffer.data() + offset, &type_byte, sizeof(type_byte));
        offset += sizeof(type_byte);
        std::memcpy(buffer.data() + offset, &count_, sizeof(count_));
        offset += sizeof(count_);
    }
    // if (!items.empty()) {
    //     std::memcpy(buffer.data() + offset, items.data(), size_items);
    //     offset += size_items;
    // }
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
            signup.traits.head = ntohs(signup.traits.head);
            signup.traits.body = ntohs(signup.traits.body);
            signup.user[sizeof(signup.user) - 1] = '\0';
            signup.password[sizeof(signup.password) - 1] = '\0';
            // CharacterTraits traits = signup.traits;

            queue.push(std::make_unique<SignupCommand>(player_id, std::string(signup.user),
                                                       std::string(signup.password),
                                                       std::move(signup.traits)));
            break;
        }
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
            uint8_t action;
            socket.recvall(&npc_id, 4);
            socket.recvall(&action, 1);
            npc_id = ntohl(npc_id);
            if (action == 1) {
                queue.push(std::make_unique<ResurrectCommand>(player_id, npc_id));
            } else {
                queue.push(std::make_unique<InteractCommand>(player_id, npc_id, action));
            }
            break;
        }
        case RESURRECT: {
            queue.push(std::make_unique<ResurrectCommand>(player_id));
            break;
        }
        case TAKE_ITEM: {
            queue.push(std::make_unique<TakeItemCommand>(player_id));
            break;
        }
        case BUY_ITEM:
        case SELL_ITEM: {
            uint32_t network_npc_id;
            uint8_t network_item_id;
            uint16_t network_quantity;

            socket.recvall(&network_npc_id, sizeof(network_npc_id));
            socket.recvall(&network_item_id, sizeof(network_item_id));
            socket.recvall(&network_quantity, sizeof(network_quantity));

            Id npc_id = ntohl(network_npc_id);

            if (opcode == BUY_ITEM) {
                queue.push(std::make_unique<BuyItemCommand>(player_id, npc_id, network_item_id));
            } else {
                queue.push(std::make_unique<SellItemCommand>(player_id, npc_id, network_item_id));
            }
            break;
        }
        case DEPOSIT_ITEM:
        case WITHDRAW_ITEM: {
            Id item_id;
            socket.recvall(&item_id, sizeof(item_id));
            item_id = ntohs(item_id);

            Id npc_id;
            socket.recvall(&item_id, sizeof(npc_id));
            item_id = ntohs(item_id);

            uint8_t type_item;
            socket.recvall(&type_item, sizeof(type_item));

            if (opcode == DEPOSIT_ITEM) {
                queue.push(std::make_unique<DepositItemCommand>(player_id, npc_id, type_item));
            } else {
                queue.push(std::make_unique<WithdrawItemCommand>(player_id, npc_id, type_item));
            }
            break;
        }
        case DEPOSIT_GOLD:
        case WITHDRAW_GOLD: {
            uint32_t amount;
            uint32_t npc_id;
            socket.recvall(&npc_id, sizeof(npc_id));
            socket.recvall(&amount, sizeof(amount));
            amount = ntohl(amount);

            if (opcode == DEPOSIT_GOLD) {
                queue.push(std::make_unique<DepositGoldCommand>(player_id, npc_id, amount));
            } else {
                queue.push(std::make_unique<WithdrawGoldCommand>(player_id, npc_id, amount));
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
        case EQUIP_ITEM:
        case UNEQUIP_ITEM: {
            Id item_id;
            socket.recvall(&item_id, sizeof(item_id));
            item_id = ntohl(item_id);

            if (opcode == EQUIP_ITEM) {
                queue.push(std::make_unique<EquipCommand>(player_id, item_id));
            } else {
                queue.push(std::make_unique<CommandUnequip>(player_id, item_id));
            }
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
