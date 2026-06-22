#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "commands/command.h"
#include "commands/command_attack.h"
#include "commands/command_buy_item.h"
#include "commands/command_chat.h"
#include "commands/command_deposit.h"
#include "commands/command_deposit_gold.h"
#include "commands/command_disconnect.h"
#include "commands/command_drop_item.h"
#include "commands/command_equip.h"
#include "commands/command_heal.h"
#include "commands/command_interact.h"
#include "commands/command_list_items.h"
#include "commands/command_login.h"
#include "commands/command_meditate.h"
#include "commands/command_move.h"
#include "commands/command_resurrect.h"
#include "commands/command_sell_item.h"
#include "commands/command_signup.h"
#include "commands/command_take_item.h"
#include "commands/command_unequip.h"
#include "commands/command_use_item.h"
#include "commands/command_withdraw.h"
#include "commands/command_withdraw_gold.h"
#include "common/includes/core/snapshot.h"
#include "common/includes/map/map.h"
#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"
#include "common/includes/types.h"

#include "definitions.h"

#pragma pack(push, 1)

class ServerProtocol {
private:
    Socket& socket;
    void sendSimpleResponse(uint8_t opcode, bool success, const std::string& msg) const;

public:
    explicit ServerProtocol(Socket& s);

    // Send data to client
    void sendSnapshot(const Snapshot& state) const;
    void sendPlayerStats(const MsgPlayerStats& stats) const;
    void sendInventoryUpdate(const MsgInventoryUpdate& inv) const;
    void sendEquipmentUpdate(const MsgEquipmentUpdate& equip) const;
    void sendChatMsg(const std::string& msg) const;
    void sendLoginResponse(bool success, Id player_id, const std::string& msg = "") const;
    void sendSignupResponse(bool success, const std::string& msg = "") const;
    void sendChangeMap(uint16_t map_id) const;
    void sendActionError(const std::string& error_msg) const;
    void sendMap(const Map& map, const std::vector<CitizenNpcSnapshot>& citizen);
    void sendTraderCatalog(const std::map<TypeItem, uint32_t>& catalog);
    void sendBankContent(const std::map<TypeItem, uint32_t>& items, uint32_t gold);

    // Returns false when the client disconnects
    bool readCommand(Id player_id, QueueCmd& queue);

    void shutdown_peer();
    void close_peer() const;
};

#pragma pack(pop)
