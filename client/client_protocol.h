#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H
#pragma once
#include "../common/socket.h"
#include "../common/protocol.h"
#include "../server/server_protocol.h"
#include <string>

enum class TypeEventClient {
    UPDATE_WORLD,
    OWN_STATS,
    DISCONNECTION,
    CHAT_MSG,
    ERROR_ACTION,
    LOGIN_RESPONSE,
    MAP_CHANGE
};

struct EventClient {
    TypeEventClient type;
    WorldState world;
    MsgPlayerStats stats;
    std::string text_payload;     // Se usa para mensajes de chat, errores o el "OK"/"ERROR" del login
    uint16_t map_id;              // Se usa para CAMBIO_MAPA
};

class ClientProtocol {
private:
    Socket& socket;

public:
    explicit ClientProtocol(Socket& s);

    // Mandar comandos al Servidor
    void sendLogin(const std::string& name, const std::string& pass);
    void sendMove(uint8_t direction);
    void sendAttack(uint32_t target_id);
    void sendChat(const std::string& msg);
    void sendUseItem(uint8_t slot_index);
    void sendDropItem(uint8_t slot_index);
    void sendCommand(const std::string& cmd);
    void sendInteract(uint32_t npc_id);
    void sendTakeItem();
    void sendBuyItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity);
    void sendSellItem(uint32_t npc_id, uint16_t item_id, uint16_t quantity);

    // Recibir actualizaciones del Servidor
    // Lee del socket para actualizar la interfaz grafica 
    bool ClientProtocol::recieveMessage(EventClient& out_evento);
};

#endif 
