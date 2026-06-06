#include "server/includes/npc/banker.h"

Banker::Banker(TypeNPC type, const std::string& name, Pose&& pose_):
        CityNPC(type, name, std::move(pose_)) {}

InteractionResult Banker::interact() {
    InteractionResult result;
    result.type = InteractionType::BANK_BOX;
    result.open_bank = true;
    return result;
}
