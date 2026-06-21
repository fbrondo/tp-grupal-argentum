#include "server/includes/npcs_positions.h"

#include <limits>
#include <ranges>
#include <stdexcept>

namespace {
uint32_t distanceBetweenPositions(const Position& a, const Position& b) {
    const uint32_t dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const uint32_t dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return dx + dy;
}
}  // namespace

void NpcPositions::add(const NpcInstance& npc) {
    switch (npc.type) {
        case MERCHANT:
            this->merchants.emplace(npc.id, npc);
            break;
        case PRIEST:
            this->priests.emplace(npc.id, npc);
            break;
        case BANKER:
            this->merchants.emplace(npc.id, npc);
            break;
        default:
            this->creatures.emplace(npc.id, npc);
            break;
    }
    this->npc_tiles.emplace(npc.pose.position, true);
}

NpcInstance NpcPositions::removeCreature(
        Id id) { /*Los unicos npcs que pueden removerse son las criaturas*/
    NpcInstance instance = this->creatures.at(id);
    this->npc_tiles.erase(instance.pose.position);
    this->creatures.erase(id);
    return instance;
}

const NpcInstance& NpcPositions::getCreature(Id id) const { return this->creatures.at(id); }

Pose NpcPositions::moveCreature(Id id, const Position& position, Direction direction) {
    NpcInstance& creature = this->creatures.at(id);
    this->npc_tiles.erase(creature.pose.position);
    creature.pose = Pose(position, direction);
    this->npc_tiles.emplace(position, true);
    return creature.pose;
}

bool NpcPositions::isOcupied(const Position& position) const {
    return this->npc_tiles.contains(position);
}

NpcInstance NpcPositions::findNearestPriest(const Position& position) const {
    if (this->priests.empty()) {
        throw std::runtime_error("No hay sanadores configurados en el mapa");
    }

    NpcInstance closest_priest;
    uint32_t min_distance = std::numeric_limits<uint32_t>::max();
    for (const auto& priest: this->priests | std::views::values) {
        const uint32_t current_distance = distanceBetweenPositions(position, priest.pose.position);
        if (current_distance < min_distance) {
            min_distance = current_distance;
            closest_priest = priest;
        }
    }
    return closest_priest;
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
