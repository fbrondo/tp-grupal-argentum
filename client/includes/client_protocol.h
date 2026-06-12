#pragma once
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
    MAP_DATA
};

/*struct MapData {
    uint32_t width{0};
    uint32_t height{0};
    std::vector<Tile> tiles;
};*/

struct EventClient {
    TypeEventClient type;
    Snapshot world;
    MsgPlayerStats stats;
    std::string text_payload;  // Se usa para mensajes de chat, errores o el "OK"/"ERROR" del login
    bool login_success{false};
    uint32_t player_id{0};
    uint16_t map_id;  // Se usa para CAMBIO_MAPA
    Map map_data;     // Se usa para MAP_DATA
    EventClient() = default;
    EventClient(EventClient&&) = default;
    EventClient& operator=(EventClient&&) = default;
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
    void sendUseItem(uint8_t slot_index) const;
    void sendDropItem(uint8_t slot_index) const;
    void sendCommand(const std::string& cmd) const;
    void sendInteract(uint32_t npc_id) const;
    void sendTakeItem() const;
    void sendBuyItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity) const;
    void sendSellItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity) const;
    void sendDisconnect() const;

    // Pre-game operations (signup, character)
    void sendSignup(const std::string& user, const std::string& password,
                    const CharacterTraits& traits) const;

    // Game loop receiver
    bool receiveMessage(EventClient& out_event) const;

    // Synchronous pre-game receiver
    bool recvResponse(uint8_t expected_opcode, std::string& out_message) const;
};

#pragma pack(pop)
