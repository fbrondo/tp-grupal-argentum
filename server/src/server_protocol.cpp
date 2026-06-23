#include "../includes/server_protocol.h"

#include <array>
#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "common/includes/map/layer.h"

namespace {

void sendAll(Socket& socket, const void* data, size_t size, const std::string& context) {
    try {
        socket.sendall(data, size);
    } catch (const std::exception& e) {
        throw std::runtime_error("ERROR IN " + context + " -- " + e.what());
    }
}

void appendBytes(std::vector<char>& buffer, size_t& offset, const void* data, size_t size) {
    std::memcpy(buffer.data() + offset, data, size);
    offset += size;
}

uint16_t recvUint16(Socket& socket) {
    uint16_t value;
    socket.recvall(&value, sizeof(value));
    return ntohs(value);
}

uint32_t recvUint32(Socket& socket) {
    uint32_t value;
    socket.recvall(&value, sizeof(value));
    return ntohl(value);
}

std::string recvText(Socket& socket) {
    const uint16_t len = recvUint16(socket);
    std::string text(len, '\0');
    if (len > 0)
        socket.recvall(text.data(), len);
    return text;
}

std::vector<char> makeTextResponse(uint8_t opcode, const std::string& text) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint16_t) + text.size());
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint16_t len = htons(static_cast<uint16_t>(text.size()));
    appendBytes(buffer, offset, &len, sizeof(len));

    if (!text.empty())
        appendBytes(buffer, offset, text.data(), text.size());

    return buffer;
}

size_t snapshotSize(const Snapshot& state) {
    return sizeof(uint8_t) + sizeof(uint16_t) +
           (state.players.size() * sizeof(PlayerSnapshotData)) + sizeof(uint16_t) +
           (state.npcs.size() * sizeof(NpcSnapshotData)) + sizeof(uint16_t) +
           (state.items_on_floor.size() * sizeof(ItemGroundSnapshotData)) + sizeof(uint16_t) +
           (state.sound_effects.size() * sizeof(SoundEffectSnapshotData)) + sizeof(uint16_t) +
           (state.visual_effects.size() * sizeof(VisualEffectSnapshotData));
}

void appendPlayers(std::vector<char>& buffer, size_t& offset,
                   const std::vector<PlayerSnapshotData>& players) {
    const uint16_t count = htons(static_cast<uint16_t>(players.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto player: players) {
        player.id = htonl(player.id);
        player.position.x = htonl(player.position.x);
        player.position.y = htonl(player.position.y);
        player.stats.max_hp = htons(player.stats.max_hp);
        player.stats.current_hp = htons(player.stats.current_hp);
        player.stats.current_mana = htons(player.stats.current_mana);
        player.stats.max_mana = htons(player.stats.max_mana);
        player.stats.xp = htonl(player.stats.xp);
        player.ch_traits.body = htons(player.ch_traits.body);
        player.ch_traits.head = htons(player.ch_traits.head);
        player.resurrection_time_left_ms = htons(player.resurrection_time_left_ms);
        appendBytes(buffer, offset, &player, sizeof(player));
    }
}

void appendNpcs(std::vector<char>& buffer, size_t& offset,
                const std::vector<NpcSnapshotData>& npcs) {
    const uint16_t count = htons(static_cast<uint16_t>(npcs.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto npc: npcs) {
        npc.id = htonl(npc.id);
        npc.position.x = htonl(npc.position.x);
        npc.position.y = htonl(npc.position.y);
        npc.current_hp = htons(npc.current_hp);
        npc.max_hp = htons(npc.max_hp);
        appendBytes(buffer, offset, &npc, sizeof(npc));
    }
}

void appendItems(std::vector<char>& buffer, size_t& offset,
                 const std::vector<ItemGroundSnapshotData>& items) {
    const uint16_t count = htons(static_cast<uint16_t>(items.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto item: items) {
        item.position.x = htonl(item.position.x);
        item.position.y = htonl(item.position.y);
        appendBytes(buffer, offset, &item, sizeof(item));
    }
}

void appendSoundEffects(std::vector<char>& buffer, size_t& offset,
                        const std::vector<SoundEffectSnapshotData>& effects) {
    const uint16_t count = htons(static_cast<uint16_t>(effects.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto effect: effects) {
        const uint16_t effect_id = htons(static_cast<uint16_t>(effect.effect_id));
        effect.effect_id = static_cast<SoundEffectID>(effect_id);
        effect.pos_x = htonl(effect.pos_x);
        effect.pos_y = htonl(effect.pos_y);
        appendBytes(buffer, offset, &effect, sizeof(effect));
    }
}

void appendVisualEffects(std::vector<char>& buffer, size_t& offset,
                         const std::vector<VisualEffectSnapshotData>& effects) {
    const uint16_t count = htons(static_cast<uint16_t>(effects.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto effect: effects) {
        const uint16_t effect_id = htons(static_cast<uint16_t>(effect.effect_id));
        effect.effect_id = static_cast<VisualEffectID>(effect_id);
        effect.recipient_id = htonl(effect.recipient_id);
        effect.pos_x = htonl(effect.pos_x);
        effect.pos_y = htonl(effect.pos_y);
        appendBytes(buffer, offset, &effect, sizeof(effect));
    }
}

std::vector<char> makeSlotsUpdate(uint8_t opcode, const std::vector<MsgSlot>& slots) {
    size_t slot_count = 0;
    for (const auto& ignored_slot: slots) {
        (void)ignored_slot;
        ++slot_count;
    }
    const auto slot_count_16 = static_cast<uint16_t>(slot_count);
    const uint16_t count = htons(slot_count_16);
    std::vector<char> buffer(sizeof(opcode) + sizeof(count) + (slot_count * sizeof(MsgSlot)));
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto slot: slots) {
        slot.quantity = htons(slot.quantity);
        appendBytes(buffer, offset, &slot, sizeof(slot));
    }
    return buffer;
}

void appendMapTiles(std::vector<char>& buffer, size_t& offset, const Map& map) {
    const std::array<Layer, layer_count> layers = {Layer::Background, Layer::Details, Layer::Object,
                                                   Layer::Roof};
    for (const Layer layer: layers) {
        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                const auto& [sprite_id, walkable, region] = map.tile_at(x, y, layer);

                const int32_t sprite_id_net = htonl(sprite_id);
                appendBytes(buffer, offset, &sprite_id_net, sizeof(sprite_id_net));

                const uint8_t walkable_byte = walkable ? 1 : 0;
                appendBytes(buffer, offset, &walkable_byte, sizeof(walkable_byte));

                const uint8_t region_byte = static_cast<uint8_t>(region);
                appendBytes(buffer, offset, &region_byte, sizeof(region_byte));
            }
        }
    }
}

void appendCitizens(std::vector<char>& buffer, size_t& offset,
                    const std::vector<CitizenNpcSnapshot>& citizens) {
    const uint16_t count = htons(static_cast<uint16_t>(citizens.size()));
    appendBytes(buffer, offset, &count, sizeof(count));

    for (auto citizen: citizens) {
        citizen.id = htonl(citizen.id);
        citizen.position.x = htonl(citizen.position.x);
        citizen.position.y = htonl(citizen.position.y);
        appendBytes(buffer, offset, &citizen, sizeof(citizen));
    }
}

void pushTradeCommand(uint8_t opcode, Id player_id, Socket& socket, QueueCmd& queue) {
    const Id npc_id = recvUint32(socket);
    uint8_t item_id;
    uint16_t quantity;
    socket.recvall(&item_id, sizeof(item_id));
    socket.recvall(&quantity, sizeof(quantity));

    if (opcode == BUY_ITEM) {
        queue.push(std::make_unique<BuyItemCommand>(player_id, npc_id, item_id));
    } else {
        queue.push(std::make_unique<SellItemCommand>(player_id, npc_id, item_id));
    }
}

void pushItemBankCommand(uint8_t opcode, Id player_id, Socket& socket, QueueCmd& queue) {
    const Id npc_id = recvUint32(socket);
    const uint8_t item_id = static_cast<uint8_t>(recvUint16(socket));

    if (opcode == DEPOSIT_ITEM) {
        queue.push(std::make_unique<DepositItemCommand>(player_id, npc_id, item_id));
    } else {
        queue.push(std::make_unique<WithdrawItemCommand>(player_id, npc_id, item_id));
    }
}

void pushGoldBankCommand(uint8_t opcode, Id player_id, Socket& socket, QueueCmd& queue) {
    const Id npc_id = recvUint32(socket);
    const uint32_t amount = recvUint32(socket);

    if (opcode == DEPOSIT_GOLD) {
        queue.push(std::make_unique<DepositGoldCommand>(player_id, npc_id, amount));
    } else {
        queue.push(std::make_unique<WithdrawGoldCommand>(player_id, npc_id, amount));
    }
}

}  // namespace

ServerProtocol::ServerProtocol(Socket& s): socket(s) {}

void ServerProtocol::sendSnapshot(const Snapshot& state) const {
    std::vector<char> buffer(snapshotSize(state));
    size_t offset = 0;

    constexpr uint8_t opcode = SNAPSHOT;
    appendBytes(buffer, offset, &opcode, sizeof(opcode));
    appendPlayers(buffer, offset, state.players);
    appendNpcs(buffer, offset, state.npcs);
    appendItems(buffer, offset, state.items_on_floor);
    appendSoundEffects(buffer, offset, state.sound_effects);
    appendVisualEffects(buffer, offset, state.visual_effects);

    sendAll(socket, buffer.data(), buffer.size(), "sendSnapshot");
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
    sendAll(socket, &temp, sizeof(temp), "sendPlayerStats");
}

void ServerProtocol::sendInventoryUpdate(const MsgInventoryUpdate& inv) const {
    const auto buffer = makeSlotsUpdate(INVENTORY_UPDATE, inv.inventory);
    sendAll(socket, buffer.data(), buffer.size(), "sendInventoryUpdate");
}

void ServerProtocol::sendEquipmentUpdate(const MsgEquipmentUpdate& equip) const {
    const auto buffer = makeSlotsUpdate(EQUIPMENT_UPDATE, equip.equipment);
    sendAll(socket, buffer.data(), buffer.size(), "sendEquipmentUpdate");
}

void ServerProtocol::sendSimpleResponse(uint8_t opcode, bool success,
                                        const std::string& msg) const {
    const size_t total_size = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) + msg.size();
    std::vector<char> buffer(total_size);
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint8_t res = success ? 1 : 0;
    appendBytes(buffer, offset, &res, sizeof(res));

    const uint16_t len = htons(static_cast<uint16_t>(msg.size()));
    appendBytes(buffer, offset, &len, sizeof(len));

    if (!msg.empty())
        appendBytes(buffer, offset, msg.data(), msg.size());

    sendAll(socket, buffer.data(), buffer.size(), "sendSimpleResponse");
}

void ServerProtocol::sendLoginResponse(bool success, Id player_id, const std::string& msg) const {
    sendSimpleResponse(LOGIN_RESPONSE, success, msg);
    const uint32_t player_id_net = htonl(player_id);
    sendAll(socket, &player_id_net, sizeof(player_id_net), "sendLoginResponse");
}

void ServerProtocol::sendSignupResponse(bool success, const std::string& msg) const {
    sendSimpleResponse(SIGNUP_RESPONSE, success, msg);
}

void ServerProtocol::sendChangeMap(const uint16_t map_id) const {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint16_t));
    size_t offset = 0;

    constexpr uint8_t opcode = CHANGE_MAP;
    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint16_t net_map_id = htons(map_id);
    appendBytes(buffer, offset, &net_map_id, sizeof(net_map_id));

    sendAll(socket, buffer.data(), buffer.size(), "sendChangeMap");
}

void ServerProtocol::sendChatMsg(const std::string& message) const {
    const auto buffer = makeTextResponse(CHAT_MSG, message);
    sendAll(socket, buffer.data(), buffer.size(), "sendChatMsg");
}

void ServerProtocol::sendActionError(const std::string& error_msg) const {
    const auto buffer = makeTextResponse(ACTION_ERROR, error_msg);
    sendAll(socket, buffer.data(), buffer.size(), "sendActionError");
}

void ServerProtocol::sendMap(const Map& map, const std::vector<CitizenNpcSnapshot>& citizen) {
    const size_t total_tiles = map.width() * map.height() * layer_count;
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) +
                              (total_tiles * 6) + sizeof(uint16_t) +
                              (citizen.size() * sizeof(CitizenNpcSnapshot));

    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = MAP_DATA;
    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint32_t width = htonl(static_cast<uint32_t>(map.width()));
    appendBytes(buffer, offset, &width, sizeof(width));

    const uint32_t height = htonl(static_cast<uint32_t>(map.height()));
    appendBytes(buffer, offset, &height, sizeof(height));

    appendMapTiles(buffer, offset, map);
    appendCitizens(buffer, offset, citizen);

    sendAll(socket, buffer.data(), buffer.size(), "sendMap");
}

void ServerProtocol::sendTraderCatalog(
        const std::map<TypeItem, std::pair<uint32_t, uint32_t>>& catalog) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint16_t) + (catalog.size() * 9));
    size_t offset = 0;

    constexpr uint8_t opcode = TRADER_CATALOG;
    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint16_t total_items = htons(static_cast<uint16_t>(catalog.size()));
    appendBytes(buffer, offset, &total_items, sizeof(total_items));

    for (const auto& [item_type, prices]: catalog) {
        const uint8_t type_byte = static_cast<uint8_t>(item_type);
        appendBytes(buffer, offset, &type_byte, sizeof(type_byte));

        const uint32_t purchase_price = htonl(prices.first);
        appendBytes(buffer, offset, &purchase_price, sizeof(purchase_price));

        const uint32_t selling_price = htonl(prices.second);
        appendBytes(buffer, offset, &selling_price, sizeof(selling_price));
    }
    sendAll(socket, buffer.data(), buffer.size(), "sendTraderCatalog");
}

void ServerProtocol::sendBankContent(const std::map<TypeItem, uint32_t>& items, uint32_t gold) {
    const size_t size_items = items.size() * (sizeof(uint8_t) + sizeof(uint32_t));
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint16_t) + size_items);
    size_t offset = 0;

    constexpr uint8_t opcode = BANK_CONTENT;
    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint32_t net_gold = htonl(gold);
    appendBytes(buffer, offset, &net_gold, sizeof(net_gold));

    const uint16_t total_items = htons(static_cast<uint16_t>(items.size()));
    appendBytes(buffer, offset, &total_items, sizeof(total_items));

    for (const auto& [type_item, count]: items) {
        const uint8_t type_byte = static_cast<uint8_t>(type_item);
        appendBytes(buffer, offset, &type_byte, sizeof(type_byte));

        const uint32_t net_count = htonl(count);
        appendBytes(buffer, offset, &net_count, sizeof(net_count));
    }
    sendAll(socket, buffer.data(), buffer.size(), "sendBankContent");
}

bool ServerProtocol::readCommand(Id player_id, QueueCmd& queue) {
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
            queue.push(std::make_unique<LoginCommand>(player_id, login.name, login.pass));
            break;
        }
        case SIGNUP: {
            MsgSignup signup;
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
            queue.push(std::make_unique<SignupCommand>(player_id, signup.user, signup.password,
                                                       std::move(signup.traits)));
            break;
        }
        case MOVE: {
            uint8_t dir;
            socket.recvall(&dir, sizeof(dir));
            queue.push(std::make_unique<MoveCommand>(player_id, dir));
            break;
        }
        case ATTACK:
            queue.push(std::make_unique<AttackCommand>(player_id, recvUint32(socket)));
            break;
        case CHAT:
        case COMMAND:
            queue.push(std::make_unique<ChatCommand>(player_id, recvText(socket)));
            break;
        case USE_ITEM:
        case DROP_ITEM: {
            const uint32_t instance_id = recvUint32(socket);
            if (opcode == USE_ITEM) {
                queue.push(std::make_unique<UseItemCommand>(player_id, instance_id));
            } else {
                queue.push(std::make_unique<DropItemCommand>(player_id, instance_id));
            }
            break;
        }
        case INTERACT: {
            const Id npc_id = recvUint32(socket);
            uint8_t action;
            socket.recvall(&action, sizeof(action));
            if (action == 1) {
                queue.push(std::make_unique<ResurrectCommand>(player_id, npc_id));
            } else {
                queue.push(std::make_unique<InteractCommand>(player_id, npc_id, action));
            }
            break;
        }
        case RESURRECT:
            queue.push(std::make_unique<ResurrectCommand>(player_id));
            break;
        case TAKE_ITEM:
            queue.push(std::make_unique<TakeItemCommand>(player_id));
            break;
        case BUY_ITEM:
        case SELL_ITEM:
            pushTradeCommand(opcode, player_id, socket, queue);
            break;
        case DEPOSIT_ITEM:
        case WITHDRAW_ITEM:
            pushItemBankCommand(opcode, player_id, socket, queue);
            break;
        case DEPOSIT_GOLD:
        case WITHDRAW_GOLD:
            pushGoldBankCommand(opcode, player_id, socket, queue);
            break;
        case LIST_ITEMS:
            queue.push(std::make_unique<ListItemsCommand>(player_id, recvUint32(socket)));
            break;
        case EQUIP_ITEM:
        case UNEQUIP_ITEM: {
            const Id item_id = recvUint32(socket);
            if (opcode == EQUIP_ITEM) {
                queue.push(std::make_unique<EquipCommand>(player_id, item_id));
            } else {
                queue.push(std::make_unique<CommandUnequip>(player_id, item_id));
            }
            break;
        }
        case DISCONNECT:
            queue.push(std::make_unique<DisconnectCommand>(player_id));
            return false;
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
