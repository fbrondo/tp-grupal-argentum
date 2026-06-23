#include "server/includes/npc/priest.h"

#include "server/includes/world.h"

Priest::Priest(const std::string& name, const NpcInstance& instance,
               std::map<TypeItem, Item*>&& items_):
        TraderNPC(name, instance, std::move(items_)) {}


void Priest::heal(Player& player) const {
    player.restoreAllHp();
    player.restoreAllMana();
}

void Priest::resurrect(Player& player, World& world, Id player_id) const {
    Pose new_pose = world.teleportPlayer(player_id, this->pose.position);
    player.updatePose(std::move(new_pose));
    this->heal(player);
    player.finishResurrection();
}
