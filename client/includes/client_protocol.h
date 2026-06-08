#pragma once
#include <map>
#include <string>
#include <vector>

#include "./common/includes/protocol.h"
#include "./common/includes/socket.h"
#include "server/includes/server_protocol.h"

enum class TypeEventClient {
    UPDATE_WORLD,
    OWN_STATS,
    DISCONNECTION,
    CHAT_MSG,
    ERROR_ACTION,
    LOGIN_RESPONSE,
    MAP_CHANGE,
    MAP_DATA,
    OPEN_MERCHANT,
    OPEN_BANK
};

struct MerchantEventData {
    std::map<TypeItem, uint32_t> catalog;
};

struct BankEventData {
    uint32_t gold{0};
    std::vector<MsgItemInfo> items;
};

struct MapData {
    int width{0};
    int height{0};
    std::vector<Tile> tiles;
};

struct EventClient {
    TypeEventClient type;
    Snapshot world;
    MsgPlayerStats stats;
    std::string text_payload;  // Se usa para mensajes de chat, errores o el "OK"/"ERROR" del login
    uint16_t map_id;           // Se usa para CAMBIO_MAPA
    MapData map;               // Se usa para MAP_DATA
    MerchantEventData merchant;  // Se usa para OPEN_MERCHANT
    BankEventData bank;          // Se usa para OPEN_BANK
};

#pragma pack(push, 1)

class ClientProtocol {
private:
    Socket& socket;

public:
    explicit ClientProtocol(Socket& s);

    // Game commands
    void sendLogin(const std::string& name, const std::string& pass) const;
    void sendMove(uint8_t direction) const;
    void sendAttack(uint32_t target_id) const;
    void sendChat(const std::string& msg) const;
    void sendUseItem(uint32_t instance_id) const;
    void sendDropItem(uint32_t instance_id) const;
    void sendCommand(const std::string& cmd) const;
    void sendInteract(uint32_t npc_id) const;
    void sendTakeItem() const;
    void sendBuyItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity) const;
    void sendSellItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity) const;
    void sendDisconnect() const;
    void sendListItems(Id npc_id);
    void sendDepositItem(Id item_id);
    void sendWithdrawItem(Id item_id);
    void sendDepositGold(uint32_t amount);
    void sendWithdrawGold(uint32_t amount);


    // Pre-game operations (signup, character)
    void sendSignup(const std::string& user, const std::string& password,
                    const CharacterTraits& traits) const;

    // Game loop receiver
    bool receiveMessage(EventClient& out_event) const;

    // Synchronous pre-game receiver
    bool recvResponse(uint8_t expected_opcode, std::string& out_message) const;
};

#pragma pack(pop)
