#include "client/includes/client_protocol.h"

#include <cstring>
#include <iostream>
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
    msg.instance_id = slot_index;
    try {
        socket.sendall(&msg, sizeof(MsgSlotItem));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendUseItem -- ") + e.what());
    }
}

void ClientProtocol::sendDropItem(const uint8_t slot_index) const {
    MsgSlotItem msg;
    msg.opcode = DROP_ITEM;
    msg.instance_id = slot_index;
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

void ClientProtocol::sendSignup(const std::string& user, const std::string& password,
                                const CharacterTraits& traits) const {
    MsgSignup msg{};
    std::strncpy(msg.user, user.c_str(), sizeof(msg.user) - 1);
    std::strncpy(msg.password, password.c_str(), sizeof(msg.password) - 1);
    msg.traits = traits;
    msg.traits.head = htons(msg.traits.head);
    msg.traits.body = htons(msg.traits.body);
    try {
        socket.sendall(&msg, sizeof(MsgSignup));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendSignup -- ") + e.what());
    }
}

void ClientProtocol::sendListItems(Id npc_id) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = LIST_ITEMS;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t npc_id_net = htonl(npc_id);
    std::memcpy(buffer.data() + offset, &npc_id_net, sizeof(npc_id_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendListItems -- ") + e.what());
    }
}

void ClientProtocol::sendDepositItem(Id item_id) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = DEPOSIT_ITEM;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t item_id_net = htonl(item_id);
    std::memcpy(buffer.data() + offset, &item_id_net, sizeof(item_id_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendDepositItem -- ") + e.what());
    }
}

void ClientProtocol::sendWithdrawItem(Id item_id) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = WITHDRAW_ITEM;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t item_id_net = htonl(item_id);
    std::memcpy(buffer.data() + offset, &item_id_net, sizeof(item_id_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendWithdrawItem -- ") + e.what());
    }
}

void ClientProtocol::sendDepositGold(uint32_t amount) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = DEPOSIT_GOLD;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t amount_net = htonl(amount);
    std::memcpy(buffer.data() + offset, &amount_net, sizeof(amount_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendDepositGold -- ") + e.what());
    }
}

void ClientProtocol::sendWithdrawGold(uint32_t amount) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = WITHDRAW_GOLD;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t amount_net = htonl(amount);
    std::memcpy(buffer.data() + offset, &amount_net, sizeof(amount_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendWithdrawGold -- ") + e.what());
    }
}

void ClientProtocol::sendEquipItem(Id item_id) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = EQUIP_ITEM;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t item_id_net = htonl(static_cast<uint32_t>(item_id));
    std::memcpy(buffer.data() + offset, &item_id_net, sizeof(item_id_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendEquipItem -- ") + e.what());
    }
}

void ClientProtocol::sendUnequipItem(Id item_id) {
    const size_t size_total = sizeof(uint8_t) + sizeof(uint32_t);
    std::vector<char> buffer(size_total);
    size_t offset = 0;

    constexpr uint8_t opcode = UNEQUIP_ITEM;
    std::memcpy(buffer.data() + offset, &opcode, sizeof(opcode));
    offset += sizeof(opcode);

    uint32_t item_id_net = htonl(static_cast<uint32_t>(item_id));
    std::memcpy(buffer.data() + offset, &item_id_net, sizeof(item_id_net));

    try {
        socket.sendall(buffer.data(), buffer.size());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendUnequipItem -- ") + e.what());
    }
}

void ClientProtocol::sendResurrect() {
    constexpr uint8_t opcode = RESURRECT;
    try {
        socket.sendall(&opcode, 1);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ERROR IN sendResurrect -- ") + e.what());
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
            out_event.world.gold_piles.clear();
            out_event.world.sound_effects.clear();

            uint16_t p_count;
            socket.recvall(&p_count, sizeof(p_count));
            p_count = ntohs(p_count);
            for (uint16_t i = 0; i < p_count; ++i) {
                PlayerSnapshotData p;
                socket.recvall(&p, sizeof(PlayerSnapshotData));
                p.id = ntohl(p.id);
                p.pos_x = ntohl(p.pos_x);
                p.pos_y = ntohl(p.pos_y);
                p.stats.max_hp = ntohs(p.stats.max_hp);
                p.stats.current_hp = ntohs(p.stats.current_hp);
                p.stats.current_mana = ntohs(p.stats.current_mana);
                p.stats.max_mana = ntohs(p.stats.max_mana);
                p.stats.xp = ntohs(p.stats.xp);
                p.ch_traits.body = ntohs(p.ch_traits.body);
                p.ch_traits.head = ntohs(p.ch_traits.head);
                p.resurrection_time_left_ms = ntohs(p.resurrection_time_left_ms);
                p.name[sizeof(p.name) - 1] = '\0';
                out_event.world.players.push_back(p);
            }

            uint16_t n_count;
            socket.recvall(&n_count, sizeof(n_count));
            n_count = ntohs(n_count);
            for (uint16_t i = 0; i < n_count; ++i) {
                NpcSnapshotData n;
                socket.recvall(&n, sizeof(NpcSnapshotData));
                n.id = ntohl(n.id);
                n.pos_x = ntohl(n.pos_x);
                n.pos_y = ntohl(n.pos_y);
                n.current_hp = ntohs(n.current_hp);
                n.max_hp = ntohs(n.max_hp);
                out_event.world.npcs.push_back(n);
            }

            uint16_t i_count;
            socket.recvall(&i_count, sizeof(i_count));
            i_count = ntohs(i_count);
            for (uint16_t i = 0; i < i_count; ++i) {
                ItemGroundSnapshotData it;
                socket.recvall(&it, sizeof(ItemGroundSnapshotData));
                it.item_id = ntohs(it.item_id);
                it.pos_x = ntohl(it.pos_x);
                it.pos_y = ntohl(it.pos_y);
                out_event.world.items_on_floor.push_back(it);
            }

            uint16_t g_count;
            if (socket.recvall(&g_count, 2) <= 0)
                return false;
            g_count = ntohs(g_count);
            for (uint16_t i = 0; i < g_count; ++i) {
                GoldPileGroundSnapshotData g;
                socket.recvall(&g, sizeof(GoldPileGroundSnapshotData));
                g.amount = ntohl(g.amount);
                g.pos_x = ntohl(g.pos_x);
                g.pos_y = ntohl(g.pos_y);
                out_event.world.gold_piles.push_back(g);
            }

            // 4. Efectos sonoros
            uint16_t sound_count;
            socket.recvall(&sound_count, 2);
            sound_count = ntohs(sound_count);

            for (uint16_t i = 0; i < sound_count; ++i) {
                SoundEffectSnapshotData e;
                socket.recvall(&e, sizeof(SoundEffectSnapshotData));
                uint16_t id_numerico;
                std::memcpy(&id_numerico, &e.effect_id, sizeof(uint16_t));
                id_numerico = ntohs(id_numerico);
                e.effect_id = static_cast<SoundEffectID>(id_numerico);
                e.pos_x = ntohl(e.pos_x);
                e.pos_y = ntohl(e.pos_y);
                out_event.world.sound_effects.push_back(e);
            }

            break;
        }
        case PLAYER_STATS: {
            out_event.type = TypeEventClient::OWN_STATS;
            out_event.stats.opcode = opcode;

            socket.recvall(&out_event.stats.hp, sizeof(out_event.stats.hp));
            out_event.stats.hp = ntohl(out_event.stats.hp);

            socket.recvall(&out_event.stats.max_hp, sizeof(out_event.stats.max_hp));
            out_event.stats.max_hp = ntohl(out_event.stats.max_hp);

            socket.recvall(&out_event.stats.mana, sizeof(out_event.stats.mana));
            out_event.stats.mana = ntohl(out_event.stats.mana);

            socket.recvall(&out_event.stats.max_mana, sizeof(out_event.stats.max_mana));
            out_event.stats.max_mana = ntohl(out_event.stats.max_mana);

            socket.recvall(&out_event.stats.gold, sizeof(out_event.stats.gold));
            out_event.stats.gold = ntohl(out_event.stats.gold);

            socket.recvall(&out_event.stats.exp, sizeof(out_event.stats.exp));
            out_event.stats.exp = ntohl(out_event.stats.exp);

            socket.recvall(&out_event.stats.exp_next_level, sizeof(out_event.stats.exp_next_level));
            out_event.stats.exp_next_level = ntohl(out_event.stats.exp_next_level);

            socket.recvall(&out_event.stats.level, sizeof(out_event.stats.level));
            break;
        }
        case LOGIN_RESPONSE: {
            out_event.type = TypeEventClient::LOGIN_RESPONSE;
            uint8_t success;
            socket.recvall(&success, sizeof(success));
            out_event.login_success = (success == 1);

            uint16_t len;
            socket.recvall(&len, sizeof(len));
            len = ntohs(len);
            out_event.text_payload.resize(len);
            socket.recvall(out_event.text_payload.data(), len);
            socket.recvall(&out_event.player_id, sizeof(out_event.player_id));
            out_event.player_id = ntohl(out_event.player_id);
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
            out_event.type = TypeEventClient::MAP_DATA;

            uint32_t w_net, h_net;
            socket.recvall(&w_net, sizeof(w_net));
            socket.recvall(&h_net, sizeof(h_net));

            // TODO: Revisar si hace falta ntohl y borrar prints
            int width = static_cast<int>(ntohl(w_net));
            int height = static_cast<int>(ntohl(h_net));
            std::cout << "[CLIENT] MAP_DATA recibido: " << width << "x" << height << " tiles."
                      << std::endl;
            Map map("Map", width, height);

            std::array<Layer, layer_count> layers = {Layer::Background, Layer::Details,
                                                     Layer::Object, Layer::Roof};

            for (const Layer layer: layers) {
                for (int y = 0; y < map.height(); ++y) {
                    for (int x = 0; x < map.width(); ++x) {
                        int32_t sprite_id_net;
                        uint8_t region_byte, walkable_byte;

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

            uint16_t count_citizen;
            socket.recvall(&count_citizen, sizeof(count_citizen));
            count_citizen = ntohs(count_citizen);

            for (uint16_t i = 0; i < count_citizen; ++i) {
                CitizenNpcSnapshot n;
                socket.recvall(&n, sizeof(CitizenNpcSnapshot));
                // socket.recvall(n.name, sizeof(n.name));
                n.id = ntohl(n.id);
                n.position.x = ntohl(n.position.x);
                n.position.y = ntohl(n.position.y);
                n.name[sizeof(n.name) - 1] = '\0';
                out_event.citizens.push_back(n);
            }
            break;
        }
        case INVENTORY_UPDATE: {
            out_event.type = TypeEventClient::INVENTORY_UPDATE;
            // MsgInventoryUpdate msg;
            uint16_t size_net;
            if (socket.recvall(&size_net, sizeof(size_net)) <= 0)
                return false;

            const uint16_t count = ntohs(size_net);
            // out_event.inventory_update.resize(count);
            for (uint16_t i = 0; i < count; i++) {
                MsgSlot slot;
                socket.recvall(&slot, sizeof(slot));
                slot.quantity = ntohs(slot.quantity);
                // out_event.inventory_update[i] = slot;
            }

            // if (socket.recvall(&msg, sizeof(MsgInventoryUpdate)) <= 0) return false;

            // msg.item_id = ntohs(msg.item_id);
            // msg.quantity = ntohs(msg.quantity);

            // out_event.inventory_update.slot_index = msg.slot_index;
            // out_event.inventory_update.item_id = msg.item_id;
            // out_event.inventory_update.quantity = msg.quantity;
            // out_event.inventory_update.is_equipped = msg.is_equipped;

            break;
        }
        case EQUIPMENT_UPDATE: {
            // out_event.type = TypeEventClient::EQUIPMENT_UPDATE;
            // uint16_t size_net;
            // if (socket.recvall(&size_net, sizeof(size_net)) <= 0)
            //     return false;
            //
            // const uint16_t count = ntohs(size_net);
            // out_event.equipment_update.resize(count);
            // for (uint16_t i = 0; i < count; i++) {
            //     MsgSlot slot;
            //     socket.recvall(&slot, sizeof(slot));
            //     out_event.equipment_update[i] = slot;
            // }
            // MsgEquipmentUpdate msg;
            // if (socket.recvall(&msg, sizeof(MsgInventoryUpdate)) <= 0) return false;
            //
            // out_event.equip_update.slot_index = msg.slot_index;
            // out_event.equip_update.item_id = msg.type_item;
            break;
        }
        default:
            return true;
    }
    return true;
}
