#include "server/includes/persistence.h"

#include <stdexcept>

Persistence::~Persistence() {
    try {
        this->players_data_queue.close();
    } catch (...) {}
    try {
        this->world_data_queue.close();
    } catch (...) {}
    std::cerr << "[Persistence] ----- Exit" << std::endl;
}

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
    try {
        while (should_keep_running()) {
            PlayerData player;
            while (this->players_data_queue.try_pop(player)) {
                this->storage.updateStatePlayer(player);
            }
            WorldStateData world;
            while (this->world_data_queue.try_pop(world)) {
                this->storage.saveWorldState(world);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    } catch (const std::exception& e) {
        std::cerr << "[Persistence] Error inesperado en el hilo: " << e.what() << std::endl;
    }
    std::clog << "[Persistence] Bucle principal terminado correctamente." << std::endl;
}

