#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <memory>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "common/includes/queue.h"
#include "common/includes/types.h"
#include "server/includes/commands/command.h"
#include "server/includes/responses/response.h"
#include "server/includes/core/snapshot.h"


using Path = std::filesystem::path;

/*Cola de comandos*/
using QueueCmd = Queue<std::unique_ptr<Command>>;

/*Mapa con las colas de respuestas - las colas de los senders*/
using MapQueueResp = std::unordered_map<Id, Queue<std::unique_ptr<Response>>>;
using QueueResp = Queue<std::unique_ptr<Response>>;

/*Lo que retornan los distintos commands */
using RegisterInfo = std::tuple<Id, std::string, std::string, TypeRace, TypeClase>;
using MoveInfo = std::tuple<Id, Direction>;
// using MapMatriz =  std::vector<std::vector<Tiles>>;

#endif
