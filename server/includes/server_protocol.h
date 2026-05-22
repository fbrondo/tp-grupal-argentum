#pragma once
#include <string>
#include <vector>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

// Struct auxiliar para que el GameLoop le pase a este protocol que enviar en la Snapshot
struct WorldState {
    std::vector<PlayerSnapshotData> players;
    std::vector<NpcSnapshotData> npcs;
    std::vector<ItemGroundSnapshotData> items;
};

struct Command {};

#pragma pack(push, 1)

class ServerProtocol {
private:
    Socket& socket;

public:
    explicit ServerProtocol(Socket& s);

    // Mandar datos al Cliente
    void sendSnapshot(const WorldState& state) const;
    void sendPlayerStats(const MsgPlayerStats& stats) const;
    void sendInventoryUpdate(const MsgInventoryUpdate& inv) const;
    void sendChatMsg(const std::string& msg) const;
    void sendLoginResponse(bool success, const std::string& error_msg = "") const;
    void sendChangeMap(uint16_t map_id) const;
    void sendActionError(const std::string& error_msg) const;

    // Recibir un comando del Cliente
    // bool readCommand(uint32_t player_id, Queue<Command>& queue); // Devuelve false si el cliente
    // se desconecto
};

#pragma pack(pop)
