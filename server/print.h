#ifndef PRINT_H
#define PRINT_H

#include <string>
#include <vector>
#include "common/includes/core/user.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/definitions.h"

namespace Print {

void print_message_console(const std::string& message);

std::string directionToString(Direction dir);
std::string RaceToString(TypeRace rac);
std::string claseToString(TypeClase clase);
std::string tileToString(Tile tile);

void initServer();
// void printinitMatrizMap(const std::vector<std::vector<Tile>>& map, const uint32_t height, const
// uint32_t width);


void printPlayerData(std::string func, const PlayerData& player);

/*GAMELOADCONFIG*/
void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config, const FileData& file_data);


void printNewPlayerArrived(const Id& id,  const User& user, TypeRace rac, TypeClase cl);
void printInitProcessRegister();
// void printPositionNewPlayer(const Id& id, const PlayerInstance& inst);
// void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst);

void printNPCsLoads(const std::map<std::string, NpcConfig>& info_npcs);
void printRacesLoad(const std::map<TypeRace, Race>& info_races);
void printClasesLoad(const std::map<TypeClase, Clase> &info_clases);


    void printPositionRandom(const Position& pos);

};  // namespace Print

#endif
