#pragma once
#include <memory>
#include <string>
#include <vector>

#include "commands/command.h"
#include "commands/command_attack.h"
#include "commands/command_buy_item.h"
#include "commands/command_chat.h"
#include "commands/command_disconnect.h"
#include "commands/command_drop_item.h"
#include "commands/command_interact.h"
#include "commands/command_login.h"
#include "commands/command_move.h"
#include "commands/command_sell_item.h"
#include "commands/command_take_item.h"
#include "commands/command_use_item.h"
#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

#include "snapshot.h"

#pragma pack(push, 1)

class ServerProtocol {
private:
    Socket& socket;

public:
    explicit ServerProtocol(Socket& s);

    // Mandar datos al Cliente
    void sendSnapshot(const Snapshot& state) const;
    void sendPlayerStats(const MsgPlayerStats& stats) const;
    void sendInventoryUpdate(const MsgInventoryUpdate& inv) const;
    void sendChatMsg(const std::string& msg) const;
    void sendLoginResponse(bool success, const std::string& error_msg = "") const;
    void sendChangeMap(uint16_t map_id) const;
    void sendActionError(const std::string& error_msg) const;

    // Recibir un comando del Cliente
    bool readCommand(uint32_t player_id,
                     Queue<std::unique_ptr<Command>>& queue);  // Devuelve false si el cliente
    // se desconecto
    void shutdown_peer();
    void close_peer() const;
};

#pragma pack(pop)
