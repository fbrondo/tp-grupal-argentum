#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>

#include "server/includes/core/config.h"
#include "server/includes/core/data.h"

class DataStorage {
private:
    mutable std::mutex file_mutex;
    std::fstream data_file;
    std::filesystem::path data_index_player;
    std::filesystem::path data_world_path;
    std::unordered_map<std::string, std::streampos> index;
    void saveIndexEntry(const std::string& name, std::streampos offset);
    void loadIndex();
    // void writeDataPlayer(const PlayerData& data);

public:
    explicit DataStorage(const FileData& paths);
    ~DataStorage();

    bool exists(const std::string& username) const;
    void savePlayer(const PlayerData& data);
    PlayerData loadPlayer(const std::string& username);

    void updateStatePlayer(const PlayerData& data);  // para guardado periódico
    void saveWorldState(const WorldStateData& state) const;
    WorldStateData loadWorldState();
    bool worldStateExists() const;
};
#endif
