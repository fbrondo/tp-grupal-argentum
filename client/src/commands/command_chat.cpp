#include "client/includes/commands/command_chat.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

#include "client/includes/client_protocol.h"
#include "client/includes/core/item_names.h"
#include "common/includes/types.h"

ChatCommandClient::ChatCommandClient(std::string msg, std::optional<uint32_t> npc_id,
                                     std::optional<uint8_t> selected_slot):
        text(std::move(msg)), npc_id(npc_id), selected_slot(selected_slot) {}

void ChatCommandClient::execute(ClientProtocol& protocol) const {
    if (text.empty())
        return;

    const std::string lower = to_lower(text);

    if (npc_id.has_value()) {
        if (lower == "/curar" || lower == "/resucitar") {
            uint8_t action = (lower == "/resucitar") ? 1 : 0;
            protocol.sendInteract(*npc_id, action);
            return;
        }
        if (lower == "/listar") {
            protocol.sendListItems(*npc_id);
            return;
        }
        if (lower.rfind("/comprar ", 0) == 0) {
            uint8_t item_id = resolve_item_id(text.substr(9));
            if (item_id != NONE) {
                protocol.sendBuyItem(*npc_id, item_id, 1);
                return;
            }
        }
        if (lower.rfind("/vender ", 0) == 0) {
            uint8_t item_id = resolve_item_id(text.substr(8));
            if (item_id != NONE) {
                protocol.sendSellItem(*npc_id, item_id, 1);
                return;
            }
        }
        if (lower.rfind("/depositar oro ", 0) == 0) {
            try {
                uint32_t amount = static_cast<uint32_t>(std::stoul(text.substr(14)));
                protocol.sendDepositGold(*npc_id, amount);
                return;
            } catch (...) {}
        }
        if (lower.rfind("/depositar ", 0) == 0) {
            uint8_t item_id = resolve_item_id(text.substr(11));
            if (item_id != NONE) {
                protocol.sendDepositItem(*npc_id, item_id);
                return;
            }
        }
        if (lower.rfind("/retirar oro ", 0) == 0) {
            try {
                uint32_t amount = static_cast<uint32_t>(std::stoul(text.substr(12)));
                protocol.sendWithdrawGold(*npc_id, amount);
                return;
            } catch (...) {}
        }
        if (lower.rfind("/retirar ", 0) == 0) {
            uint8_t item_id = resolve_item_id(text.substr(9));
            if (item_id != NONE) {
                protocol.sendWithdrawItem(*npc_id, item_id);
                return;
            }
        }
    }
    if (lower == "/tomar") {
        protocol.sendTakeItem();
        return;
    }

    if (lower == "/tirar") {
        if (selected_slot.has_value()) {
            protocol.sendDropItem(*selected_slot);
        }
        return;
    }
    if (lower.rfind("/tirar ", 0) == 0) {
        try {
            uint32_t slot = static_cast<uint32_t>(std::stoul(text.substr(7)));
            if (slot >= 1) {
                protocol.sendDropItem(static_cast<uint8_t>(slot - 1));
            }
        } catch (...) {}
        return;
    }
    protocol.sendChat(text);
}
