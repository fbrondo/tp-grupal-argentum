#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "common/includes/queue.h"
#include "common/includes/thread.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/data_storage.h"

class Persistence: public Thread {
private:
    DataStorage storage;
    Queue<PlayerData> players_data_queue;  // todos los jugadores activos
    Queue<WorldStateData> world_data_queue;

public:
    ~Persistence();
    explicit Persistence(const FileData& paths);
    void run() override;

    bool exists(const std::string& username) const;
    bool worldStateExists() const;

    void savePlayer(const PlayerData& data);

    PlayerData loadPlayer(const std::string& username);
    WorldStateData loadWorldState();

    void schedulePlayers(PlayerData data);
    void scheduleWorld(WorldStateData data);
};


#endif
