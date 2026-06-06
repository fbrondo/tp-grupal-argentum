#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "common/includes/queue.h"
#include "common/includes/types.h"
#include "server/includes/commands/command.h"
#include "server/includes/core/snapshot.h"
#include "server/includes/responses/response.h"

/*Cola de comandos*/
using QueueCmd = Queue<std::unique_ptr<Command>>;

/*Mapa con las colas de respuestas - las colas de los senders*/
using MapQueueResp = std::map<Id, Queue<std::shared_ptr<Response>>>;
using QueueResp = Queue<std::shared_ptr<Response>>;


#endif
