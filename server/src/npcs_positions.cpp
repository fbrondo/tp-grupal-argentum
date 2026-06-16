#include "server/includes/npcs_positions.h"

    void NpcPositions::add(const NpcInstance& npc) {
        switch (npc.type) {
            case MERCHANT:
                this->merchants.emplace(npc.id, npc); break;
            case PRIEST: this->priests.emplace(npc.id, npc); break;
            case BANKER: this->merchants.emplace(npc.id, npc); break;
            default: this->creatures.emplace(npc.id, npc);break;
        }
        this->npc_tiles.emplace(npc.pose.position, true);
    }

    void  NpcPositions::remove(Id id) { /*Los unicos npcs que pueden removerse son las criaturas*/
       this->creatures.erase(id);
    }

bool NpcPositions::isOcupied(const Position &position) const {
        return this->npc_tiles.contains(position);
}

// void removeByIdOnly(uint32_t id) {
    //     if (creatures.erase(id)) return;
    //     if (citizens.erase(id)) return;
    //     if (merchants.erase(id)) return;
    // }

    // 3. BUSCAR/OBTENER: Devuelve el NPC sin importar en qué mapa esté alojado
    // NpcData& get(uint32_t id, uint8_t type) {
    //     NpcCategory category = getCategory(type);
    //     switch (category) {
    //         case NpcCategory::CITIZEN:  return citizens.at(id);
    //         case NpcCategory::CREATURE: return creatures.at(id);
    //         case NpcCategory::MERCHANT: return merchants.at(id);
    //         default: throw std::out_of_range("NPC no encontrado");
    //     }
    // }

    // Getters para cuando el Gameloop/Broadcast necesite iterar sobre un grupo específico
    // const std::unordered_map<uint32_t, NpcData>& getCreatures() const { return creatures; }
    // const std::unordered_map<uint32_t, NpcData>& getCitizens() const { return citizens; }
    // const std::unordered_map<uint32_t, NpcData>& getMerchants() const { return merchants; }