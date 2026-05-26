#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "common/includes/queue.h"
#include "common/includes/types.h"

#include "snapshot.h"
//#include "commands/command.h"
class Command;
/*Cola de comandos*/
using QueueCmd = Queue<std::unique_ptr<Command>>;
using MapQueues = std::unordered_map<Id, Queue<Snapshot>>;

/*Logica del juego*/
// using MapMatriz =  std::vector<std::vector<Tiles>>;

#endif
