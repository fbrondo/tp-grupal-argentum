#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H
#pragma once
#include "../common/socket.h"
#include "../common/protocol.h"
#include <vector>
#include <string>

// Sturct auxiliar para que el GameLoop le pase a este protocol que enviar en la Snapshot
struct WorldState {
    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items;
};

class ServerProtocol {
private:
    Socket& socket;

public:
    explicit ServerProtocol(Socket& s);

    // Mandar datos al Cliente
    void sendSnapshot(const WorldState& state);
    void sendPlayerStats(const MsgPlayerStats& stats);
    void sendInventoryUpdate(const MsgInventoryUpdate& inv);
    void sendChatMsg(const std::string& msg);
    void sendLoginResponse(bool success, const std::string& error_msg = "");
    void sendChangeMap(uint16_t map_id);
    void sendActionError(const std::string& error_msg);

    // Recibir un comando del Cliente
    bool readCommand(uint32_t player_id, class CommandQueue& queue); // Devuelve false si el cliente se desconecto
};

#endif
