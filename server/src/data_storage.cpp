#include "server/includes/data_storage.h"

DataStorage::DataStorage(const FileData& paths) {
    // std::filesystem::create_directories(path);
    std::filesystem::path data_players = paths.players;
    this->data_index_player = paths.indx_players;
    this->data_world_path = paths.world;
    if (!std::filesystem::exists(data_players)) {
        std::ofstream create(data_players, std::ios::binary);
    }
    /*Permite leer, escribir, bytes puros sin interpretar, escrituta al final*/
    this->data_file.open(data_players,
                         std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!this->data_file.is_open()) {
        throw std::runtime_error("Error al cargar datos al server");
    }
    std::filesystem::create_directories(data_players.parent_path());
    this->loadIndex();
}

// void DataStorage::writeDataPlayer(const PlayerData &data) {
//
// }

bool DataStorage::exists(const std::string& username) const {
    return this->index.contains(username);
}

void DataStorage::saveIndexEntry(const std::string& name, std::streampos offset) {
    std::ofstream idxFile(this->data_index_player, std::ios::binary | std::ios::app);
    uint8_t name_length = static_cast<uint8_t>(name.size());
    idxFile.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
    idxFile.write(name.data(), name_length);
    idxFile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
}

/*Cargamos el index en memoria*/
void DataStorage::loadIndex() {
    std::ifstream idxFile(this->data_index_player, std::ios::binary);
    if (!idxFile.is_open())  // primera vez — índice vacío
        return;
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
void DataStorage::savePlayer(const PlayerData& data) {
    // this->index[data.username] = offset;
    // this->saveIndexEntry(data.username, offset);  // actualizás el archivo de índice en disco
    this->data_file.seekp(0, std::ios::end); /*se mueve el curso hacia el final del archivo*/
    std::streampos offset = this->data_file.tellp(); /*tellp nos da la posicion en que quedo*/
    /* campos fijos */
    this->data_file.write(reinterpret_cast<const char*>(&data.x), sizeof(data.x));
    this->data_file.write(reinterpret_cast<const char*>(&data.y), sizeof(data.y));
    this->data_file.write(reinterpret_cast<const char*>(&data.direction), sizeof(data.direction));
    this->data_file.write(reinterpret_cast<const char*>(&data.charact_traits),
                          sizeof(data.charact_traits));
    this->data_file.write(reinterpret_cast<const char*>(&data.level), sizeof(data.level));
    this->data_file.write(reinterpret_cast<const char*>(&data.hp), sizeof(data.hp));
    this->data_file.write(reinterpret_cast<const char*>(&data.mana), sizeof(data.mana));
    this->data_file.write(reinterpret_cast<const char*>(&data.golden), sizeof(data.golden));

    /*inventario — primero el tamaño, luego los elementos*/
    const uint32_t inv_size = static_cast<uint32_t>(data.inventory.size());
    this->data_file.write(reinterpret_cast<const char*>(&inv_size), sizeof(inv_size));
    this->data_file.write(reinterpret_cast<const char*>(data.inventory.data()),
                          inv_size * sizeof(ItemInstanceData));
    /*Equipo*/
    const uint32_t equip_size = static_cast<uint32_t>(data.equipment.size());
    this->data_file.write(reinterpret_cast<const char*>(&equip_size), sizeof(equip_size));
    this->data_file.write(reinterpret_cast<const char*>(data.equipment.data()),
                          equip_size * sizeof(ItemInstanceData));

    this->data_file.flush(); /*fuerza que los datos lleguen al disco*/
    this->index[data.username] = offset;
    this->saveIndexEntry(data.username, offset);
}

PlayerData DataStorage::loadPlayer(const std::string& username) {
    PlayerData data;
    // std::streampos offset = this->index.at(username)
    this->data_file.seekg(this->index.at(username));
    // campos fijos
    this->data_file.read(reinterpret_cast<char*>(&data.x), sizeof(data.x));
    this->data_file.read(reinterpret_cast<char*>(&data.y), sizeof(data.y));
    this->data_file.read(reinterpret_cast<char*>(&data.direction), sizeof(data.direction));
    this->data_file.read(reinterpret_cast<char*>(&data.charact_traits),
                         sizeof(data.charact_traits));
    this->data_file.read(reinterpret_cast<char*>(&data.level), sizeof(data.level));
    this->data_file.read(reinterpret_cast<char*>(&data.hp), sizeof(data.hp));
    this->data_file.read(reinterpret_cast<char*>(&data.mana), sizeof(data.mana));
    this->data_file.read(reinterpret_cast<char*>(&data.golden), sizeof(data.golden));
    // inventario
    uint32_t inv_size;
    this->data_file.read(reinterpret_cast<char*>(&inv_size), sizeof(inv_size));
    data.inventory.resize(inv_size);
    this->data_file.read(reinterpret_cast<char*>(data.inventory.data()),
                         inv_size * sizeof(ItemInstanceData));
    // equipo
    uint32_t equip_size;
    this->data_file.read(reinterpret_cast<char*>(&equip_size), sizeof(equip_size));
    data.equipment.resize(equip_size);
    this->data_file.read(reinterpret_cast<char*>(data.equipment.data()),
                         equip_size * sizeof(ItemInstanceData));
    return data;
}

void DataStorage::updateStatePlayer(const PlayerData& data) {
    // va al offset original y sobreescribe
    // como vimos antes, va al final si el tamaño cambió
    std::streampos offset = this->index.at(data.username);
    this->data_file.seekp(offset);

    /* campos fijos */
    this->data_file.write(reinterpret_cast<const char*>(&data.x), sizeof(data.x));
    this->data_file.write(reinterpret_cast<const char*>(&data.y), sizeof(data.y));
    this->data_file.write(reinterpret_cast<const char*>(&data.direction), sizeof(data.direction));
    this->data_file.write(reinterpret_cast<const char*>(&data.charact_traits),
                          sizeof(data.charact_traits));
    this->data_file.write(reinterpret_cast<const char*>(&data.level), sizeof(data.level));
    this->data_file.write(reinterpret_cast<const char*>(&data.hp), sizeof(data.hp));
    this->data_file.write(reinterpret_cast<const char*>(&data.mana), sizeof(data.mana));
    this->data_file.write(reinterpret_cast<const char*>(&data.golden), sizeof(data.golden));

    /*inventario — primero el tamaño, luego los elementos*/
    const uint32_t inv_size = static_cast<uint32_t>(data.inventory.size());
    this->data_file.write(reinterpret_cast<const char*>(&inv_size), sizeof(inv_size));
    this->data_file.write(reinterpret_cast<const char*>(data.inventory.data()),
                          inv_size * sizeof(ItemInstanceData));
    /*Equipo*/
    const uint32_t equip_size = static_cast<uint32_t>(data.equipment.size());
    this->data_file.write(reinterpret_cast<const char*>(&equip_size), sizeof(equip_size));
    this->data_file.write(reinterpret_cast<const char*>(data.equipment.data()),
                          equip_size * sizeof(ItemInstanceData));

    this->data_file.flush();
}

void DataStorage::saveWorldState(const WorldStateData& state) const {
    std::ofstream world_file(this->data_world_path,
                             std::ios::binary | std::ios::trunc);  // trunc = borra y reescribe

    // cantidad de ciuadano npc
    const uint32_t citizen_npc_size = static_cast<uint32_t>(state.citizen_npcs.size());
    world_file.write(reinterpret_cast<const char*>(&citizen_npc_size), sizeof(citizen_npc_size));
    world_file.write(reinterpret_cast<const char*>(state.citizen_npcs.data()),
                     citizen_npc_size * sizeof(NpcStateData));

    /*cantidad de criaturas*/
    const uint32_t creatures_size = static_cast<uint32_t>(state.creatures.size());
    world_file.write(reinterpret_cast<const char*>(&creatures_size), sizeof(creatures_size));
    world_file.write(reinterpret_cast<const char*>(state.creatures.data()),
                     creatures_size * sizeof(NpcStateData));

    /*cantidad de tesoros*/
    const uint32_t treasures_size = static_cast<uint32_t>(state.treasures.size());
    world_file.write(reinterpret_cast<const char*>(&treasures_size), sizeof(treasures_size));
    world_file.write(reinterpret_cast<const char*>(state.treasures.data()),
                     treasures_size * sizeof(TreasureStateData));

    /*cantidad de bolsas de oros*/
    const uint32_t gold_bags_size = static_cast<uint32_t>(state.gold_bags.size());
    world_file.write(reinterpret_cast<const char*>(&gold_bags_size), sizeof(gold_bags_size));
    world_file.write(reinterpret_cast<const char*>(state.gold_bags.data()),
                     gold_bags_size * sizeof(GoldBagsData));

    // items en el suelo
    const uint32_t items_size = static_cast<uint32_t>(state.items.size());
    world_file.write(reinterpret_cast<const char*>(&items_size), sizeof(items_size));
    world_file.write(reinterpret_cast<const char*>(state.items.data()),
                     items_size * sizeof(ItemInstanceData));
    world_file.flush();
}

WorldStateData DataStorage::loadWorldState() {
    WorldStateData state;
    std::ifstream world_file(this->data_world_path, std::ios::binary);
    if (!world_file.is_open())
        return state;

    // ciudadanos
    uint32_t citizan_npc_size;
    world_file.read(reinterpret_cast<char*>(&citizan_npc_size), sizeof(citizan_npc_size));
    state.creatures.resize(citizan_npc_size);
    world_file.read(reinterpret_cast<char*>(state.creatures.data()),
                    citizan_npc_size * sizeof(NpcStateData));

    // criaturas
    uint32_t creatures_size;
    world_file.read(reinterpret_cast<char*>(&creatures_size), sizeof(creatures_size));
    state.creatures.resize(creatures_size);
    world_file.read(reinterpret_cast<char*>(state.creatures.data()),
                    creatures_size * sizeof(NpcStateData));

    // tesoros
    uint32_t treasures_size;
    world_file.read(reinterpret_cast<char*>(&treasures_size), sizeof(treasures_size));
    state.treasures.resize(treasures_size);
    world_file.read(reinterpret_cast<char*>(state.treasures.data()),
                    treasures_size * sizeof(TreasureStateData));

    // bolsas de oro
    uint32_t gold_bags;
    world_file.read(reinterpret_cast<char*>(&gold_bags), sizeof(gold_bags));
    state.treasures.resize(gold_bags);
    world_file.read(reinterpret_cast<char*>(state.treasures.data()),
                    gold_bags * sizeof(GoldBagsData));

    // items
    uint32_t items_size;
    world_file.read(reinterpret_cast<char*>(&items_size), sizeof(items_size));
    state.items.resize(items_size);
    world_file.read(reinterpret_cast<char*>(state.items.data()),
                    items_size * sizeof(ItemInstanceData));
    return state;
}

bool DataStorage::worldStateExists() const {
    return std::filesystem::exists(this->data_world_path);
}

DataStorage::~DataStorage() {
    if (this->data_file.is_open()) {
        this->data_file.flush();
        this->data_file.close();
    }
}
