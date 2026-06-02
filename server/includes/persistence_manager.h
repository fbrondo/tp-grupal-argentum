#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include <fstream>
#include <filesystem>
#include <unordered_map>
#include "server/includes/core/data.h"
#include "server/includes/core/config.h"

class PersistenceManager {
private:
    std::fstream data_file;   // archivo 1 — datos
    std::unordered_map<std::string, std::streampos> index;  // archivo 2 — índice en memoria
    void saveIndexEntry(const std::string& name, std::streampos offset);
    void loadIndex(const std::filesystem::path& index_data_path);

public:
    explicit PersistenceManager(const FileData& paths);

    bool exists(const std::string& username) const;
    void savePlayer(const PlayerData& data);
    PlayerData loadPlayer(const std::string& username);
    void updateStatePlayer(const PlayerData& data);  // para guardado periódico
};
#endif

