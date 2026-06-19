#include "client/includes/commands/command_attack.h"

#include "client/includes/client_protocol.h"

AttackCommandClient::AttackCommandClient(uint32_t id): target_id(id) {}

void AttackCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendAttack(target_id);
}
