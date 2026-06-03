#ifndef PRINT_H
#define PRINT_H

#include <string>
#include <vector>

#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "server/includes/core/config.h"
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

/*Carga de archivos*/
void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config,
                            const FileData& file_data);


void printNewPlayerArrived(const Id& id, const std::string& username, const std::string& password,
                           TypeRace rac, TypeClase cl);
void printPositionNewPlayer(const Id& id, const PlayerInstance& inst);
void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst);

};  // namespace Print

#endif
