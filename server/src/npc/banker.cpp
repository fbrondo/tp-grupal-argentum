#include "server/includes/npc/banker.h"

Banker::Banker(TypeNPC type, std::string&& name, Pose&& pose_):
        CityNPC(type, std::move(name), std::move(pose_)) {}

// void Banker::interact(const Id& id_player, World& word, Comand& cmd) {}
