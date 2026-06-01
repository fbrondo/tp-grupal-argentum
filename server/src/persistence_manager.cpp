#include "server/includes/persistence_manager.h"

PersistenceManager::PersistenceManager(const PathConfig& paths) {
    //std::filesystem::create_directories(path);
    std::filesystem::path data_players = paths.players_data;
    std::filesystem::path data_index_player = paths.players_indx;
    /*Permite leer, escribir, bytes puros sin interpretar, escrituta al final*/
    this->data_file.open(data_players, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if(!this->data_file.is_open()) {
        throw std::runtime_error("Error al cargar datos al server");
    }
    this->loadIndex(data_index_player);
}

bool PersistenceManager::exists(const std::string& username) const {
    return this->index.contains(username);
}

void PersistenceManager::saveIndexEntry(const std::string& name,std::streampos offset) {
    /*player.idx puede estar harcodeado?*/
    std::ofstream idxFile("players.idx", std::ios::binary | std::ios::app);
    uint8_t name_length = static_cast<uint8_t>(name.size());
    idxFile.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
    idxFile.write(name.data(), name_length);
    idxFile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
}

/*Cargamos el index en memoria*/
void PersistenceManager::loadIndex(const std::filesystem::path& index_data) {
    std::ifstream idxFile(index_data, std::ios::binary);
    if (!idxFile.is_open()) return;  // primera vez — índice vacío

    /* el índice está guardado como pares (nombre, offset)*/
    while (idxFile.peek() != EOF) {
        uint8_t name_legth;
        idxFile.read(reinterpret_cast<char*>(&name_legth), sizeof(name_legth));

        std::string username(name_legth, '\0');
        idxFile.read(username.data(), name_legth);

        /*Lee el offset (posicion en el .dat) y lo asocia al nombre en el mapa*/
        std::streampos offset;
        idxFile.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        this->index[username] = offset;
    }
}

 /* va al final del archivo y guarda el offset en el index*/
void PersistenceManager::savePlayer(const PlayerData& data) {
    this->data_file.seekp(0, std::ios::end); /*se mueve el curso hacia el final del archivo*/
    std::streampos offset = this->data_file.tellp(); /*tellp nos da la posicion en que quedo*/
    this->data_file.write(reinterpret_cast<const char*>(&data), sizeof(PlayerData));
    this->data_file.flush(); /*fuerza que los datos lleguen al disco*/
    this->index[data.username] = offset;
    this->saveIndexEntry(data.username, offset);  // actualizás el archivo de índice en disco
}

PlayerData PersistenceManager::loadPlayer(const std::string& username) {
    PlayerData data;
    std::streampos offset = this->index.at(username);
    this->data_file.seekg(offset);
    this->data_file.read(reinterpret_cast<char*>(&data), sizeof(PlayerData));
    return data;
}

