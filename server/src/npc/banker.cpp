#include "server/includes/npc/banker.h"

Banker::Banker(TypeNPC type, const std::string& name /*, const Pose& pose_*/):
        CitizenNPC(type, name /*, pose_*/) {}

void Banker::createPlayerAccount(const Id& player_id) { this->bank.emplace(player_id, Account()); }

InteractionResult Banker::interact() {
    InteractionResult result;
    result.type = InteractionType::BANK_BOX;
    result.open_bank = true;
    return result;
}
