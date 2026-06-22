#pragma once

#include <optional>
#include <string>

#include "client/includes/commands/command_client.h"

class ChatCommandClient: public ClientCommand {
    std::string text;
    std::optional<uint32_t> npc_id;
    std::optional<uint8_t> selected_slot;

public:
    explicit ChatCommandClient(std::string msg, std::optional<uint32_t> npc_id = std::nullopt,
                               std::optional<uint8_t> selected_slot = std::nullopt);
    void execute(ClientProtocol& protocol) const override;
};
