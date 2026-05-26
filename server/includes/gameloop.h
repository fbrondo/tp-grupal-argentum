#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <map>
#include <memory>

#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "core/item.h"

#include "definitions.h"
#include "monitor_queues.h"
#include "player.h"
#include "world.h"

class MoveCommand;

class Gameloop: public Thread {

private:
    std::map<TypeItem, std::unique_ptr<Item>> confItems;
    // std::map<Id, std::unique_ptr<NPC>> confNPC;
    MonitorQueues& monitor;
    QueueCmd& commands_queue;
    World world_game;
    void executeMovePlayer(MoveCommand* move_cmd);
    void execuetRequest();

public:
    explicit Gameloop(MonitorQueues& monitor, QueueCmd& cmmds_queue);
    void run() override;
};

#endif
