#include "server/includes/persistence_manager.h"
 using namespace  std;

PersistenceManager::PersistenceManager(const FileData& paths) {
    // std::filesystem::create_directories(path);
    filesystem::path data_players = paths.players;
    this->data_index_player = paths.indx_players;
    /*Permite leer, escribir, bytes puros sin interpretar, escrituta al final*/
    this->data_file.open(data_players,ios::in | ios::out | ios::binary | ios::app);
    if (!this->data_file.is_open()) {
        throw runtime_error("Error al cargar datos al server");
    }
    this->loadIndex();
}

bool PersistenceManager::exists(const string& username) const {
    return this->index.contains(username);
}

void PersistenceManager::saveIndexEntry(const string& name, streampos offset) {
    ofstream idxFile("players.idx", ios::binary | ios::app);
    uint8_t name_length = static_cast<uint8_t>(name.size());
    idxFile.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
    idxFile.write(name.data(), name_length);
    idxFile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
}

/*Cargamos el index en memoria*/
void PersistenceManager::loadIndex() {
    std::ifstream idxFile(this->data_index_player, ios::binary);
    if (!idxFile.is_open()) // primera vez — índice vacío
        return;
    /* el índice está guardado como pares (nombre, offset)*/
    while (idxFile.peek() != EOF) {
        uint8_t name_legth;
        idxFile.read(reinterpret_cast<char*>(&name_legth), sizeof(name_legth));

        string username(name_legth, '\0');
        idxFile.read(username.data(), name_legth);
        /*Lee el offset (posicion en el .dat) y lo asocia al nombre en el mapa*/
        streampos offset;
        idxFile.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        this->index[username] = offset;
    }
}

/* va al final del archivo y guarda el offset en el index*/
void PersistenceManager::savePlayer(const PlayerData& data) {
    // this->data_file.seekp(0, std::ios::end); /*se mueve el curso hacia el final del archivo*/
    // std::streampos offset = this->data_file.tellp(); /*tellp nos da la posicion en que quedo*/
    // this->data_file.write(reinterpret_cast<const char*>(&data), sizeof(PlayerData));
    // this->data_file.flush(); /*fuerza que los datos lleguen al disco*/
    // this->index[data.username] = offset;
    // this->saveIndexEntry(data.username, offset);  // actualizás el archivo de índice en disco
    this->data_file.seekp(0, std::ios::end);
    streampos offset = this->data_file.tellp();
    /* campos fijos */
    this->data_file.write(reinterpret_cast<const char*>(&data.x), sizeof(data.x));
    this->data_file.write(reinterpret_cast<const char*>(&data.y), sizeof(data.y));
    this->data_file.write(reinterpret_cast<const char*>(&data.direction), sizeof(data.direction));
    this->data_file.write(reinterpret_cast<const char*>(&data.charact_traits), sizeof(data.charact_traits));
    this->data_file.write(reinterpret_cast<const char*>(&data.level), sizeof(data.level));
    this->data_file.write(reinterpret_cast<const char*>(&data.hp), sizeof(data.hp));
    this->data_file.write(reinterpret_cast<const char*>(&data.mana), sizeof(data.mana));
    this->data_file.write(reinterpret_cast<const char*>(&data.golden), sizeof(data.golden));

    /*inventario — primero el tamaño, luego los elementos*/
    uint32_t inv_size = static_cast<uint32_t>(data.inventory.size());
    this->data_file.write(reinterpret_cast<const char*>(&inv_size), sizeof(inv_size));
    this->data_file.write(reinterpret_cast<const char*>(data.inventory.data()), inv_size * sizeof(ItemInstanceData));
    /*Equipo*/
    uint32_t equip_size = static_cast<uint32_t>(data.equipment.size());
    this->data_file.write(reinterpret_cast<const char*>(&equip_size), sizeof(equip_size));
    this->data_file.write(reinterpret_cast<const char*>(data.equipment.data()), equip_size * sizeof(ItemInstanceData));

    this->data_file.flush();
    this->index[data.username] = offset;
    this->saveIndexEntry(data.username, offset);
}

PlayerData PersistenceManager::loadPlayer(const std::string& username) {
    PlayerData data;
    // std::streampos offset = this->index.at(username);
    // this->data_file.seekg(offset);
    // this->data_file.read(reinterpret_cast<char*>(&data), sizeof(PlayerData));
    this->data_file.seekg(this->index.at(username));

    // campos fijos
    this->data_file.read(reinterpret_cast<char*>(&data.x), sizeof(data.x));
    this->data_file.read(reinterpret_cast<char*>(&data.y), sizeof(data.y));
    this->data_file.read(reinterpret_cast<char*>(&data.direction), sizeof(data.direction));
    this->data_file.read(reinterpret_cast<char*>(&data.charact_traits), sizeof(data.charact_traits));
    this->data_file.read(reinterpret_cast<char*>(&data.level), sizeof(data.level));
    this->data_file.read(reinterpret_cast<char*>(&data.hp), sizeof(data.hp));
    this->data_file.read(reinterpret_cast<char*>(&data.mana), sizeof(data.mana));
    this->data_file.read(reinterpret_cast<char*>(&data.golden), sizeof(data.golden));

    // inventario
    uint32_t inv_size;
    this->data_file.read(reinterpret_cast<char*>(&inv_size), sizeof(inv_size));
    data.inventory.resize(inv_size);
    this->data_file.read(reinterpret_cast<char*>(data.inventory.data()), inv_size * sizeof(ItemInstanceData));
    // equipo
    uint32_t equip_size;
    this->data_file.read(reinterpret_cast<char*>(&equip_size), sizeof(equip_size));
    data.equipment.resize(equip_size);
    this->data_file.read(reinterpret_cast<char*>(data.equipment.data()), equip_size * sizeof(ItemInstanceData));
    return data;
}
