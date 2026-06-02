#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <string>
#include "common/includes/types.h"
#include "server/includes/definitions.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/config.h"
namespace  Print {

    void print_message_console(const std::string& message);

    std::string directionToString(Direction dir);
    std::string RaceToString(TypeRace rac);
    std::string claseToString(TypeClase clase);

    void initServer();

    /*Carga de archivos*/
    void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config, const FileData& file_data);


void printNewPlayerArrived(const Id& id, const std::string& username, const std::string& password, TypeRace rac, TypeClase cl);
    void printPositionNewPlayer(const Id& id, const PlayerInstance& inst);
    void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst);

};

#endif
