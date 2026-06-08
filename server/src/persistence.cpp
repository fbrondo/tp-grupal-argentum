#include "server/includes/persistence.h"

#include <stdexcept>

Persistence::Persistence(const FileData& paths): storage(paths) {}

bool Persistence::exists(const std::string& username) const { return storage.exists(username); }

bool Persistence::worldStateExists() const { return storage.worldStateExists(); }

void Persistence::savePlayer(const PlayerData& data) { storage.savePlayer(data); }

PlayerData Persistence::loadPlayer(const std::string& username) {
    return storage.loadPlayer(username);
}

WorldStateData Persistence::loadWorldState() { return storage.loadWorldState(); }

void Persistence::schedulePlayers(PlayerData data) {
    this->players_data_queue.push(std::move(data));
}

void Persistence::scheduleWorld(WorldStateData data) {
    this->world_data_queue.push(std::move(data));
}


void Persistence::run() {
    while (should_keep_running()) {
        try {
            PlayerData player;
            while (this->players_data_queue.try_pop(player)) {
                this->storage.updateStatePlayer(player);
            }
        } catch (const ClosedQueue&) {
            std::cerr << "[Persistence] run() exited: ClosedQueue - player_data" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[Persistencer] run() exited: " << e.what() << std::endl;
        }

        try {
            WorldStateData world;
            while (this->world_data_queue.try_pop(world)) {
                storage.saveWorldState(world);
            }
        } catch (const ClosedQueue&) {
            std::cerr << "[Persistence] run() exited: ClosedQueue - world_data" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[Persistencer] run() exited: " << e.what() << std::endl;
        }
    }
}
