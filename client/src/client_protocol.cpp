#include "client/includes/client_protocol.h"

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

std::vector<char> makeTextMessage(uint8_t opcode, const std::string& text) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint16_t) + text.size());
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint16_t len = htons(static_cast<uint16_t>(text.size()));
    appendBytes(buffer, offset, &len, sizeof(len));

    if (!text.empty())
        appendBytes(buffer, offset, text.data(), text.size());

    return buffer;
}

std::vector<char> makeNpcItemMessage(uint8_t opcode, Id npc_id, Id item_id) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint16_t));
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint32_t npc_id_net = htonl(npc_id);
    appendBytes(buffer, offset, &npc_id_net, sizeof(npc_id_net));

    const uint16_t item_id_net = htons(static_cast<uint16_t>(item_id));
    appendBytes(buffer, offset, &item_id_net, sizeof(item_id_net));

    return buffer;
}

std::vector<char> makeNpcAmountMessage(uint8_t opcode, Id npc_id, uint32_t amount) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t));
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint32_t npc_id_net = htonl(npc_id);
    appendBytes(buffer, offset, &npc_id_net, sizeof(npc_id_net));

    const uint32_t amount_net = htonl(amount);
    appendBytes(buffer, offset, &amount_net, sizeof(amount_net));

    return buffer;
}

std::vector<char> makeIdMessage(uint8_t opcode, Id id) {
    std::vector<char> buffer(sizeof(uint8_t) + sizeof(uint32_t));
    size_t offset = 0;

    appendBytes(buffer, offset, &opcode, sizeof(opcode));

    const uint32_t id_net = htonl(static_cast<uint32_t>(id));
    appendBytes(buffer, offset, &id_net, sizeof(id_net));

    return buffer;
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

void receivePlayers(Socket& socket, Snapshot& world) {
    const uint16_t count = recvUint16(socket);
    world.players.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        PlayerSnapshotData player;
        socket.recvall(&player, sizeof(PlayerSnapshotData));
        player.id = ntohl(player.id);
        player.position.x = ntohl(player.position.x);
        player.position.y = ntohl(player.position.y);
        player.stats.max_hp = ntohs(player.stats.max_hp);
        player.stats.current_hp = ntohs(player.stats.current_hp);
        player.stats.current_mana = ntohs(player.stats.current_mana);
        player.stats.max_mana = ntohs(player.stats.max_mana);
        player.stats.xp = ntohl(player.stats.xp);
        player.ch_traits.body = ntohs(player.ch_traits.body);
        player.ch_traits.head = ntohs(player.ch_traits.head);
        player.name[sizeof(player.name) - 1] = '\0';
        player.resurrection_time_left_ms = ntohs(player.resurrection_time_left_ms);
        world.players.push_back(player);
    }
}

void receiveNpcs(Socket& socket, Snapshot& world) {
    const uint16_t count = recvUint16(socket);
    world.npcs.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        NpcSnapshotData npc;
        socket.recvall(&npc, sizeof(NpcSnapshotData));
        npc.id = ntohl(npc.id);
        npc.position.x = ntohl(npc.position.x);
        npc.position.y = ntohl(npc.position.y);
        npc.current_hp = ntohs(npc.current_hp);
        npc.max_hp = ntohs(npc.max_hp);
        world.npcs.push_back(npc);
    }
}

void receiveItemsOnFloor(Socket& socket, Snapshot& world) {
    const uint16_t count = recvUint16(socket);
    world.items_on_floor.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        ItemGroundSnapshotData item;
        socket.recvall(&item, sizeof(ItemGroundSnapshotData));
        item.position.x = ntohl(item.position.x);
        item.position.y = ntohl(item.position.y);
        world.items_on_floor.push_back(item);
    }
}

void receiveSoundEffects(Socket& socket, Snapshot& world) {
    const uint16_t count = recvUint16(socket);
    world.sound_effects.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        SoundEffectSnapshotData effect;
        socket.recvall(&effect, sizeof(SoundEffectSnapshotData));

        uint16_t effect_id;
        std::memcpy(&effect_id, &effect.effect_id, sizeof(effect_id));
        effect.effect_id = static_cast<SoundEffectID>(ntohs(effect_id));
        effect.pos_x = ntohl(effect.pos_x);
        effect.pos_y = ntohl(effect.pos_y);
        world.sound_effects.push_back(effect);
    }
}

void receiveVisualEffects(Socket& socket, Snapshot& world) {
    const uint16_t count = recvUint16(socket);
    world.visual_effects.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        VisualEffectSnapshotData effect;
        socket.recvall(&effect, sizeof(VisualEffectSnapshotData));

        uint16_t effect_id;
        std::memcpy(&effect_id, &effect.effect_id, sizeof(effect_id));
        effect.effect_id = static_cast<VisualEffectID>(ntohs(effect_id));
        effect.recipient_id = ntohl(effect.recipient_id);
        effect.pos_x = ntohl(effect.pos_x);
        effect.pos_y = ntohl(effect.pos_y);
        world.visual_effects.push_back(effect);
    }
}

void receiveSnapshot(Socket& socket, EventClient& out_event) {
    out_event.type = TypeEventClient::UPDATE_WORLD;
    out_event.world.players.clear();
    out_event.world.npcs.clear();
    out_event.world.items_on_floor.clear();
    out_event.world.sound_effects.clear();
    out_event.world.visual_effects.clear();

    receivePlayers(socket, out_event.world);
    receiveNpcs(socket, out_event.world);
    receiveItemsOnFloor(socket, out_event.world);
    receiveSoundEffects(socket, out_event.world);
    receiveVisualEffects(socket, out_event.world);
}

void receivePlayerStats(Socket& socket, EventClient& out_event, uint8_t opcode) {
    out_event.type = TypeEventClient::OWN_STATS;
    out_event.stats.opcode = opcode;
    out_event.stats.hp = recvUint32(socket);
    out_event.stats.max_hp = recvUint32(socket);
    out_event.stats.mana = recvUint32(socket);
    out_event.stats.max_mana = recvUint32(socket);
    out_event.stats.safe_gold = recvUint32(socket);
    out_event.stats.excess_gold = recvUint32(socket);
    out_event.stats.exp = recvUint32(socket);
    out_event.stats.exp_next_level = recvUint32(socket);
    socket.recvall(&out_event.stats.level, sizeof(out_event.stats.level));
}

void receiveLoginResponse(Socket& socket, EventClient& out_event) {
    out_event.type = TypeEventClient::LOGIN_RESPONSE;

    uint8_t success;
    socket.recvall(&success, sizeof(success));
    out_event.login_success = (success == 1);
    out_event.text_payload = recvText(socket);
    out_event.player_id = recvUint32(socket);
}

void receiveMapData(Socket& socket, EventClient& out_event) {
    out_event.type = TypeEventClient::MAP_DATA;

    const int width = static_cast<int>(recvUint32(socket));
    const int height = static_cast<int>(recvUint32(socket));
    Map map("Map", width, height);

    const std::array<Layer, layer_count> layers = {Layer::Background, Layer::Details, Layer::Object,
                                                   Layer::Roof};
    for (const Layer layer: layers) {
        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                int32_t sprite_id_net;
                uint8_t region_byte;
                uint8_t walkable_byte;

                socket.recvall(&sprite_id_net, sizeof(sprite_id_net));
                socket.recvall(&walkable_byte, sizeof(walkable_byte));
                socket.recvall(&region_byte, sizeof(region_byte));

                auto& [sprite_id, walkable, region] = map.tile_at(x, y, layer);
                sprite_id = ntohl(sprite_id_net);
                walkable = (walkable_byte == 1);
                region = static_cast<Region>(region_byte);
            }
        }
    }
    out_event.map_data = std::move(map);

    const uint16_t count_citizen = recvUint16(socket);
    out_event.citizens.clear();
    out_event.citizens.reserve(count_citizen);
    for (uint16_t i = 0; i < count_citizen; ++i) {
        CitizenNpcSnapshot npc;
        socket.recvall(&npc, sizeof(CitizenNpcSnapshot));
        npc.id = ntohl(npc.id);
        npc.position.x = ntohl(npc.position.x);
        npc.position.y = ntohl(npc.position.y);
        npc.name[sizeof(npc.name) - 1] = '\0';
        out_event.citizens.push_back(npc);
    }
}

bool receiveSlots(Socket& socket, std::vector<MsgSlot>& slots) {
    uint16_t count_net;
    if (socket.recvall(&count_net, sizeof(count_net)) <= 0)
        return false;

    const uint16_t count = ntohs(count_net);
    std::vector<MsgSlot> received_slots;
    received_slots.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        MsgSlot slot;
        socket.recvall(&slot, sizeof(slot));
        slot.quantity = ntohs(slot.quantity);
        received_slots.push_back(slot);
    }
    slots.swap(received_slots);
    return true;
}

void receiveTraderCatalog(Socket& socket, EventClient& out_event) {
    out_event.type = TypeEventClient::OPEN_MERCHANT;
    const uint16_t count = recvUint16(socket);
    out_event.merchant_data.catalog.clear();
    for (uint16_t i = 0; i < count; ++i) {
        TypeItem type_item;
        socket.recvall(&type_item, sizeof(type_item));
        const uint32_t purchase_price = recvUint32(socket);
        const uint32_t selling_price = recvUint32(socket);
        out_event.merchant_data.catalog.emplace(type_item,
                                                CatalogEntry{purchase_price, selling_price});
    }
}

void receiveBankContent(Socket& socket, EventClient& out_event) {
    out_event.type = TypeEventClient::OPEN_BANK;
    out_event.bank_data.gold = recvUint32(socket);

    const uint16_t count = recvUint16(socket);
    out_event.bank_data.items.clear();
    for (uint16_t i = 0; i < count; ++i) {
        TypeItem type_item;
        uint32_t amount;
        socket.recvall(&type_item, sizeof(type_item));
        socket.recvall(&amount, sizeof(amount));
        out_event.bank_data.items.emplace(type_item, ntohl(amount));
    }
}

}  // namespace

ClientProtocol::ClientProtocol(Socket& s): socket(s) {}

void ClientProtocol::sendLogin(const std::string& name, const std::string& pass) const {
    MsgLogin msg;
    std::memset(msg.name, 0, sizeof(msg.name));
    std::memset(msg.pass, 0, sizeof(msg.pass));
    std::strncpy(msg.name, name.c_str(), sizeof(msg.name) - 1);
    std::strncpy(msg.pass, pass.c_str(), sizeof(msg.pass) - 1);
    sendAll(socket, &msg, sizeof(MsgLogin), "sendLogin");
}

void ClientProtocol::sendMove(const uint8_t direction) const {
    MsgMove msg;
    msg.direction = direction;
    sendAll(socket, &msg, sizeof(MsgMove), "sendMove");
}

void ClientProtocol::sendAttack(const uint32_t target_id) const {
    MsgAttack msg;
    msg.target_id = htonl(target_id);
    sendAll(socket, &msg, sizeof(MsgAttack), "sendAttack");
}

void ClientProtocol::sendChat(const std::string& msg) const {
    const auto buffer = makeTextMessage(CHAT, msg);
    sendAll(socket, buffer.data(), buffer.size(), "sendChat");
}

void ClientProtocol::sendUseItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = USE_ITEM;
    msg.instance_id = htonl(static_cast<uint32_t>(slot_index));
    sendAll(socket, &msg, sizeof(MsgSlotItem), "sendUseItem");
}

void ClientProtocol::sendDropItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = DROP_ITEM;
    msg.instance_id = htonl(static_cast<uint32_t>(slot_index));
    sendAll(socket, &msg, sizeof(MsgSlotItem), "sendDropItem");
}

void ClientProtocol::sendCommand(const std::string& cmd) const {
    const auto buffer = makeTextMessage(COMMAND, cmd);
    sendAll(socket, buffer.data(), buffer.size(), "sendCommand");
}

void ClientProtocol::sendInteract(const uint32_t npc_id, const uint8_t action) const {
    MsgInteract msg;
    msg.npc_id = htonl(npc_id);
    msg.action = action;
    sendAll(socket, &msg, sizeof(MsgInteract), "sendInteract");
}

void ClientProtocol::sendTakeItem() const {
    constexpr uint8_t opcode = TAKE_ITEM;
    sendAll(socket, &opcode, sizeof(opcode), "sendTakeItem");
}

void ClientProtocol::sendBuyItem(const uint32_t npc_id, const uint8_t item_id,
                                 const uint16_t quantity) const {
    MsgTrade msg;
    msg.opcode = BUY_ITEM;
    msg.npc_id = htonl(npc_id);
    msg.item_id = item_id;
    msg.quantity = htons(quantity);
    sendAll(socket, &msg, sizeof(MsgTrade), "sendBuyItem");
}

void ClientProtocol::sendSellItem(const uint32_t npc_id, const uint8_t item_id,
                                  const uint16_t quantity) const {
    MsgTrade msg;
    msg.opcode = SELL_ITEM;
    msg.npc_id = htonl(npc_id);
    msg.item_id = item_id;
    msg.quantity = htons(quantity);
    sendAll(socket, &msg, sizeof(MsgTrade), "sendSellItem");
}

void ClientProtocol::sendDisconnect() const {
    constexpr uint8_t opcode = DISCONNECT;
    sendAll(socket, &opcode, sizeof(opcode), "sendDisconnect");
}

void ClientProtocol::sendSignup(const std::string& user, const std::string& password,
                                const CharacterTraits& traits) const {
    MsgSignup msg{};
    std::strncpy(msg.user, user.c_str(), sizeof(msg.user) - 1);
    std::strncpy(msg.password, password.c_str(), sizeof(msg.password) - 1);
    msg.traits = traits;
    msg.traits.head = htons(msg.traits.head);
    msg.traits.body = htons(msg.traits.body);
    sendAll(socket, &msg, sizeof(MsgSignup), "sendSignup");
}

void ClientProtocol::sendListItems(Id npc_id) {
    const auto buffer = makeIdMessage(LIST_ITEMS, npc_id);
    sendAll(socket, buffer.data(), buffer.size(), "sendListItems");
}

void ClientProtocol::sendDepositItem(Id npc_id, Id item_id) {
    const auto buffer = makeNpcItemMessage(DEPOSIT_ITEM, npc_id, item_id);
    sendAll(socket, buffer.data(), buffer.size(), "sendDepositItem");
}

void ClientProtocol::sendWithdrawItem(Id npc_id, Id item_id) {
    const auto buffer = makeNpcItemMessage(WITHDRAW_ITEM, npc_id, item_id);
    sendAll(socket, buffer.data(), buffer.size(), "sendWithdrawItem");
}

void ClientProtocol::sendDepositGold(Id npc_id, uint32_t amount) {
    const auto buffer = makeNpcAmountMessage(DEPOSIT_GOLD, npc_id, amount);
    sendAll(socket, buffer.data(), buffer.size(), "sendDepositGold");
}

void ClientProtocol::sendWithdrawGold(Id npc_id, uint32_t amount) {
    const auto buffer = makeNpcAmountMessage(WITHDRAW_GOLD, npc_id, amount);
    sendAll(socket, buffer.data(), buffer.size(), "sendWithdrawGold");
}

void ClientProtocol::sendEquipItem(Id item_id) {
    const auto buffer = makeIdMessage(EQUIP_ITEM, item_id);
    sendAll(socket, buffer.data(), buffer.size(), "sendEquipItem");
}

void ClientProtocol::sendUnequipItem(Id item_id) {
    const auto buffer = makeIdMessage(UNEQUIP_ITEM, item_id);
    sendAll(socket, buffer.data(), buffer.size(), "sendUnequipItem");
}

void ClientProtocol::sendResurrect() {
    constexpr uint8_t opcode = RESURRECT;
    sendAll(socket, &opcode, sizeof(opcode), "sendResurrect");
}

bool ClientProtocol::recvResponse(uint8_t expected_opcode, std::string& out_message) const {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0)
        return false;
    if (opcode != expected_opcode)
        return false;

    uint8_t success;
    socket.recvall(&success, 1);
    out_message = recvText(socket);
    return success == 1;
}

bool ClientProtocol::receiveMessage(EventClient& out_event) const {
    uint8_t opcode;
    if (socket.recvall(&opcode, 1) <= 0) {
        out_event.type = TypeEventClient::DISCONNECTION;
        return false;
    }

    switch (opcode) {
        case SNAPSHOT:
            receiveSnapshot(socket, out_event);
            break;
        case PLAYER_STATS:
            receivePlayerStats(socket, out_event, opcode);
            break;
        case LOGIN_RESPONSE:
            receiveLoginResponse(socket, out_event);
            break;
        case CHANGE_MAP:
            out_event.type = TypeEventClient::MAP_CHANGE;
            out_event.map_id = recvUint16(socket);
            break;
        case CHAT_MSG:
        case ACTION_ERROR:
            out_event.type = (opcode == CHAT_MSG) ? TypeEventClient::CHAT_MSG :
                                                    TypeEventClient::ERROR_ACTION;
            out_event.text_payload = recvText(socket);
            break;
        case MAP_DATA:
            receiveMapData(socket, out_event);
            break;
        case INVENTORY_UPDATE:
            out_event.type = TypeEventClient::INVENTORY_UPDATE;
            return receiveSlots(socket, out_event.inventory);
        case EQUIPMENT_UPDATE:
            out_event.type = TypeEventClient::EQUIPMENT_UPDATE;
            return receiveSlots(socket, out_event.equipment);
        case TRADER_CATALOG:
            receiveTraderCatalog(socket, out_event);
            break;
        case BANK_CONTENT:
            receiveBankContent(socket, out_event);
            break;
        default:
            return true;
    }
    return true;
}
