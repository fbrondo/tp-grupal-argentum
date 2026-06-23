#include "print.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#define SALTO "\n"

namespace {

bool debugMode() {
    const char* env_p = std::getenv("DEBUG");
    return env_p != nullptr && std::string(env_p) == "1";
}

void printMessage(const std::string& message) { std::cout << message << std::endl; }

std::string raceToString(TypeRace race) {
    switch (race) {
        case HUMAN:
            return "HUMANO";
        case ELF:
            return "ELFO";
        case DWARF:
            return "ENANO";
        case GNOME:
            return "GNOMO";
        default:
            return "UNKNOWN";
    }
}

std::string claseToString(TypeClase clase) {
    switch (clase) {
        case WIZARD:
            return "MAGO";
        case CLERIC:
            return "CLERIGO";
        case PALADIN:
            return "PALADIN";
        case WARRIOR:
            return "GUERRERO";
        default:
            return "UNKNOWN";
    }
}

std::string npcToString(TypeNPC type) {
    switch (type) {
        case FIELD_GOBLIN:
            return "GOBLIN DE CAMPO";
        case FOREST_GOBLIN:
            return "GOBLIN DE BOSQUE";
        case SKELETON:
            return "ESQUELETO";
        case WARRIOR_SKELETON:
            return "ESQUELETO GUERRERO";
        case ZOMBIE:
            return "ZOMBIE";
        case DUNGEON_SPIDER:
            return "ARANA DE MAZMORRA";
        case CAVE_SPIDER:
            return "ARANA DE CAVERNA";
        case FOREST_SPIDER:
            return "ARANA DE BOSQUE";
        case ORC:
            return "ORCO";
        case GOLEM:
            return "GOLEM";
        case INFERNAL_GOLEM:
            return "GOLEM INFERNAL";
        case PRIEST:
            return "SACERDOTE";
        case BANKER:
            return "BANQUERO";
        case MERCHANT:
            return "COMERCIANTE";
        default:
            return "DESCONOCIDO";
    }
}

void printBox(const std::vector<std::string>& lines) {
    size_t max_length = 0;
    for (const auto& line: lines) {
        max_length = std::max(max_length, line.length());
    }

    constexpr int MARGIN = 2;
    const size_t total_width = max_length + (MARGIN * 2) + 2;
    std::cout << std::string(total_width, '*') << "\n";
    for (const auto& line: lines) {
        std::cout << "*" << std::string(MARGIN, ' ') << std::left << std::setw(max_length) << line
                  << std::string(MARGIN, ' ') << "*\n";
    }
    std::cout << std::string(total_width, '*') << "\n";
}

}  // namespace

namespace Print {

void printMessageConsole(const std::string& message) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "----------[" << message << "]----------";
    printMessage(oss.str());
}

void initServer() {
    if (!debugMode())
        return;

    const std::string message = "| Server started |";
    const std::string border(message.length(), '-');
    std::cout << "\n" << border << SALTO << message << SALTO << border << std::endl;
}

void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config,
                            const FileData& file_data) {
    if (!debugMode())
        return;

    printBox({"CARGANDO: " + path.filename().string() + " ---- " + path.string(), "",
              "Rutas de Archivos de configuracion:", " - Game:    " + paths_config.game.string(),
              " - Races:   " + paths_config.races.string(),
              " - Clases:  " + paths_config.clases.string(),
              " - NPCs:    " + paths_config.npcs.string(),
              " - Creatures: " + paths_config.creatures.string(),
              " - Items:   " + paths_config.items.string(),
              " - Regions: " + paths_config.regions.string(), "",
              "Rutas de Archivos de Datos:", " - Players: " + file_data.players.string(),
              " - Index:   " + file_data.indx_players.string(),
              " - World:   " + file_data.world.string(), " - Map:     " + file_data.map.string()});
}

void printNpc(const NpcInstance& npc) {
    if (!debugMode())
        return;

    printBox({" NPC: " + npcToString(npc.type), "ID: " + std::to_string(npc.id),
              " Posicion: (" + std::to_string(npc.pose.position.x) + ", " +
                      std::to_string(npc.pose.position.y) + ")"});
}

void printItem(const TreasureInstance& instance) {
    if (!debugMode())
        return;

    printBox({" TESORO", "ID: " + std::to_string(instance.id),
              " Posicion: (" + std::to_string(instance.position.x) + ", " +
                      std::to_string(instance.position.y) + ")"});
}

void printNewPlayerArrived(const Id& id, const User& user, TypeRace race, TypeClase clase) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << " ========== Client Arrived =========" << SALTO;
    oss << "- id: " << id << SALTO;
    oss << "- username: " << user.username << SALTO;
    oss << "- pass: " << user.password << SALTO;
    oss << "- race: " << raceToString(race) << SALTO;
    oss << "- clase: " << claseToString(clase) << SALTO;
    oss << " ===================================" << SALTO;
    printMessage(oss.str());
}

void printPositionMovePlayer(const Id& id, const Pose& pose, const Position& prev) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "[DEBUG - Game] - El jugador de ";
    oss << "| ID: " << id << SALTO;
    oss << "Se movio de Pos: (" << prev.x << ", " << prev.y << ") a" << SALTO;
    oss << "Nueva posicion Pos: (" << pose.position.x << ", " << pose.position.y << ")" << SALTO;
    printMessage(oss.str());
}

void imprimirCajaContenedora(const PlayerData& player) {
    if (!debugMode())
        return;

    printBox({" JUGADOR: ", player.username,
              " Posicion: (" + std::to_string(player.position.x) + ", " +
                      std::to_string(player.position.y) + ")",
              " Nivel:    " + std::to_string(static_cast<int>(player.level)),
              " HP:       " + std::to_string(player.hp),
              " Mana:     " + std::to_string(player.mana)});
}

void printInitGameloop(const std::string& message) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "[DEBUG - Init] ----- " << message << " |";
    printMessage(oss.str());
}

void printSizeInventory(const uint32_t& size_inventory) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "[DEBUG - INVENTORY] - Cantidad items en inventario: " << size_inventory << SALTO;
    printMessage(oss.str());
}

void printInitAttackPlayer(const Id& attacker_id, const Id& victim_id) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "[DEBUG - ATTACK] - attacker=" << attacker_id << " victim=" << victim_id << SALTO;
    printMessage(oss.str());
}

void printAttackPlayerIsDead() {
    if (!debugMode())
        return;

    printMessage("[DEBUG - ATTACK] - blocked: attacker is dead ");
}

void printEvasiveMessageAttack(const std::string& name_victim) {
    if (!debugMode())
        return;

    std::ostringstream oss;
    oss << "[DEBUG - Game] - La victima " << name_victim << " esquivo el ataque." << SALTO;
    printMessage(oss.str());
}

}  // namespace Print
