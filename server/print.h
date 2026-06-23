#ifndef PRINT_H
#define PRINT_H

#include <string>

#include "common/includes/core/user.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/definitions.h"

namespace Print {

void printMessageConsole(const std::string& message);
void initServer();

void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config,
                            const FileData& file_data);
void printNpc(const NpcInstance& npc);
void printItem(const TreasureInstance& instance);
void printNewPlayerArrived(const Id& id, const User& user, TypeRace race, TypeClase clase);
void printPositionMovePlayer(const Id& id, const Pose& pose, const Position& prev);
void imprimirCajaContenedora(const PlayerData& player);
void printInitGameloop(const std::string& message);
void printSizeInventory(const uint32_t& size_inventory);
void printInitAttackPlayer(const Id& attacker_id, const Id& victim_id);
void printAttackPlayerIsDead();
void printEvasiveMessageAttack(const std::string& name_victim);

}  // namespace Print

#endif
