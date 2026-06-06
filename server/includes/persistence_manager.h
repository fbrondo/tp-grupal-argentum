#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "server/includes/core/config.h"
#include "server/includes/core/data.h"

class PersistenceManager {
private:
    std::filesystem::path data_index_player;
    std::fstream data_file; // archivo 1 — datos
    std::unordered_map<std::string, std::streampos> index;
    void saveIndexEntry(const std::string& name, std::streampos offset);
    void loadIndex();

public:
    explicit PersistenceManager(const FileData& paths);

    bool exists(const std::string& username) const;
    void savePlayer(const PlayerData& data);
    PlayerData loadPlayer(const std::string& username);
    void updateStatePlayer(const PlayerData& data);  // para guardado periódico
};
#endif
