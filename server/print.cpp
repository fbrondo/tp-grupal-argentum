#include "print.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "common/includes/direction.h"
#define SALTO "\n"


namespace Print {
void print_message_console(const std::string& message) { std::cout << message << std::endl; }

void printMessageConsole(const std::string &message) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "----------[" << message << "]----------";
        std::string m = oss.str();
        print_message_console(m);
    }
}

std::string itemToString(TypeItem item) {
    switch (item) {
        case SWORD:
            return "Espada";
        case AXE:
            return "Hacha";
        case HAMMER:
            return "Martillo";
        case ASH_STAFF:
            return "Vara de fresno";
        case ELVEN_FLUTE:
            return "Flauta elfica";
        case KNOTTED_STAFF:
            return "Baculo nudoso";
        case INLAID_STAFF:
            return "Baculo engarzado";
        case SIMPLE_BOW:
            return "Arco simple";
        case COMPOUND_BOW:
            return "Arco compuesto";
        case LEATHER_ARMOR:
            return "Armadura de cuero";
        case PLATE_AMOR:
            return "Armadura de placas";
        case BLUE_TUNIC:
            return "Tunica Azul";
        case HOOD:
            return "Capucha";
        case IRON_HELMET:
            return "Casco de hierro";
        case TORTOISE_SHIELD:
            return "Escudo de tortuga";
        case IRON_SHIELD:
            return "Escudo de hierro";
        case MAGIC_HAT:
            return "Sombrero magico";
        case LIFE_POTION:
            return "Pocion vida";
        case MANA_POTION:
            return "Pocion mana";
        case GOLD:
            return "Oro";
        default:
            return "UNKNOWN";
    }
}

std::string directionToString(Direction dir) {
    switch (dir) {
        case DOWN:
            return "DOWN";
        case UP:
            return "UP";
        case LEFT:
            return "LEFT";
        case RIGHT:
            return "RIGHT";
        default:
            return "UNKNOWN";
    }
}

std::string RaceToString(TypeRace rac) {
    switch (rac) {
        case HUMAN:
            return "HUMANO";
        case ELF:
            return "ELFO";
        case DWARF:
            return "ENANO";
        case GNOME:
            return "GNOME";
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

std::string tileToString(Region region) {
    switch (region) {
        case Region::Cavern:
            return "CAVERNA";
        case Region::Dungeon:
            return "MAZMORRA";
        case Region::Forest:
            return "BOSQUE";
        case Region::Desert:
            return "DESIERTO";
        case Region::Field:
            return "CAMPO";
        case Region::City:
            return "CIUDAD";
        case Region::Town:
            return "PUEBLO";
    }
    return "DESCONOCIDO";
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
            return "ARAÑA DE MAZMORRA";
        case CAVE_SPIDER:
            return "ARAÑA DE CAVERNA";
        case FOREST_SPIDER:
            return "ARAÑA DE BOSQUE";
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
            break;
    }
    return "DESCONOCIDO";
}


void initServer() {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::cout << "" << std::endl;
        std::string mensaje = "| Server started |";
        std::string borde(mensaje.length(), '-');
        std::cout << borde << SALTO << mensaje << SALTO << borde << std::endl;
    }
}

// void printPlayerData(std::string func, const PlayerData& player) {
//     const char* env_p = std::getenv("DEBUG");
//     bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
//     if (debug_mode) {
//
//         std::vector<std::string> renglones = {
//                 func + " DATOS JUGADOR " + std::string(player.username) + " ---- ",
//                 "",  // Línea vacía
//                 " - Password:    " + std::string(player.password),
//                 " - Races:   " + paths_config.races.string(),
//                 " - Clases:  " + paths_config.clases.string(),
//                 " - NPCs:    " + paths_config.npcs.string(),
//                 " - Items:   " + paths_config.items.string(),
//                 " - Regions: " + paths_config.regions.string(),
//                 "",  // Línea vacía
//                 "Rutas de Archivos de Datos:",
//                 " - Players: " + file_data.players.string(),
//                 " - Index:   " + file_data.indx_players.string(),
//                 " - World:   " + file_data.world.string(),
//                 " - Map:     " + file_data.map.string()};
//         size_t max_largo = 0;
//         auto it = std::max_element(
//                 renglones.begin(), renglones.end(),
//                 [](const auto& a, const auto& b) { return a.length() < b.length(); });
//         if (it != renglones.end())
//             max_largo = it->length();
//         // El ancho interior de la caja será el largo máximo más los espacios de cortesía a los
//         // costados
//         int ancho_caja = static_cast<int>(max_largo) + 4;
//         std::string borde_horizontal(ancho_caja, '-');
//         // Empezamos a armar el ostringstream
//         std::ostringstream oss;
//         // Techo de la caja
//         oss << borde_horizontal << SALTO;
//         // Recorremos los renglones guardados y los rellenamos dinámicamente
//         for (const auto& renglon: renglones) {
//             oss << "| " << renglon;
//             // Calculamos cuántos espacios le faltan a ESTE renglón específico para alcanzar al
//             más
//             // largo
//             int espacios_necesarios = static_cast<int>(max_largo - renglon.length());
//             if (espacios_necesarios > 0) {
//                 oss << std::string(espacios_necesarios, ' ');
//             }
//             oss << " |" << SALTO;
//         }
//         // Piso de la caja
//         oss << borde_horizontal;
//         print_message_console(oss.str());
//     }
// }

void printInitMatrizMap(const std::vector<std::vector<Tile>>& map, uint32_t height,
                        uint32_t width) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "--- Mundo ---" << std::endl;
        for (uint32_t i = 0; i < height; i++) {                             /*recorre columnas*/
            for (uint32_t j = 0; j < width; j++) {                          // Recorre las filas
                std::cout << "[" << tileToString(map[j][i].region) << "]";  // \t añade un espacio
            }
        }
        std::string message = oss.str();
        print_message_console(message);
    }
}


void printNewPlayerArrived(const Id& id, const User& user, TypeRace rac, TypeClase clase) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << " ========== Client Arrived =========" << SALTO;
        oss << "- id: " << id << SALTO;
        oss << "- username: " << user.username << SALTO;
        oss << "- pass: " << user.password << SALTO;
        oss << "- race: " << RaceToString(rac) << SALTO;
        oss << "- clase: " << claseToString(clase) << SALTO;
        oss << " ===================================" << SALTO;
        std::string message = oss.str();
        print_message_console(message);
    }
}

void playerArrivedLogin(const Id& id, const User& user) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << " ========== [LOGIN] Client Arrived =========" << SALTO;
        oss << "- id: " << id << SALTO;
        oss << "- username: " << user.username << SALTO;
        oss << "- pass: " << user.password << SALTO;
        oss << " ===========================================" << SALTO;
        std::string message = oss.str();
        print_message_console(message);
    }
}

// void printPositionNewPlayer(const Id& id, const PlayerInstance& inst) {
//     const char* env_p = std::getenv("DEBUG");
//     bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
//     if (debug_mode) {
//         std::ostringstream oss;
//         auto pos = inst.position;
//         auto dir = inst.direct;
//         oss << "[DEBUG] NEW PlayerID: " << id << " | Pos: (" << pos.x << ", " << pos.y << ")"
//             << " | Dir: " << directionToString(dir);
//         std::string message = oss.str();
//         print_message_console(message);
//     }
// }
//
// void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst) {
//     const char* env_p = std::getenv("DEBUG");
//     bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
//     if (debug_mode) {
//         std::ostringstream oss;
//         auto pos = inst.position;
//         auto dir = inst.direct;
//         oss << "[DEBUG] PlayerID: " << id << " | Pos: (" << pos.x << ", " << pos.y << ")"
//             << " | Dir: " << directionToString(dir);
//         std::string message = oss.str();
//         print_message_console(message);
//     }
// }

void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config,
                            const FileData& file_data) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::vector<std::string> renglones = {
                "CARGANDO: " + path.filename().string() + " ---- " + path.string(),
                "",  // Línea vacía
                "Rutas de Archivos de configuracion:",
                " - Game:    " + paths_config.game.string(),
                " - Races:   " + paths_config.races.string(),
                " - Clases:  " + paths_config.clases.string(),
                " - NPCs:    " + paths_config.npcs.string(),
                " - Creatures: " + paths_config.creatures.string(),
                " - Items:   " + paths_config.items.string(),
                " - Regions: " + paths_config.regions.string(),
                "",  // Línea vacía
                "Rutas de Archivos de Datos:",
                " - Players: " + file_data.players.string(),
                " - Index:   " + file_data.indx_players.string(),
                " - World:   " + file_data.world.string(),
                " - Map:     " + file_data.map.string()};
        size_t max_largo = 0;
        auto it = std::max_element(
                renglones.begin(), renglones.end(),
                [](const auto& a, const auto& b) { return a.length() < b.length(); });
        if (it != renglones.end())
            max_largo = it->length();
        // El ancho interior de la caja será el largo máximo más los espacios de cortesía a los
        // costados
        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde_horizontal(ancho_caja, '-');
        // Empezamos a armar el ostringstream
        std::ostringstream oss;
        // Techo de la caja
        oss << borde_horizontal << SALTO;
        // Recorremos los renglones guardados y los rellenamos dinámicamente
        for (const auto& renglon: renglones) {
            oss << "| " << renglon;
            // Calculamos cuántos espacios le faltan a ESTE renglón específico para alcanzar al más
            // largo
            int espacios_necesarios = static_cast<int>(max_largo - renglon.length());
            if (espacios_necesarios > 0) {
                oss << std::string(espacios_necesarios, ' ');
            }
            oss << " |" << SALTO;
        }
        // Piso de la caja
        oss << borde_horizontal;
        print_message_console(oss.str());
    }
}

void printPositionTreasure(const Id &id, const Position &position) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] - Tesoro | ID: "<< id << "---" << "Pos: (" << position.x << ", " << position.y << ")" << " |";
        std::string message = oss.str();
        print_message_console(message);
    }
}

void printPositionCreature(const Id& id, TypeNPC type, const Position& pos, const NpcAttributes& attrib) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] - Creature: " << npcToString(type);
        oss << "| ID: "<< id ;
        oss << "---" << "Pos: (" << pos.x << ", " << pos.y << ")" << " |";
        oss << "Atributos: " << SALTO;
        oss << " > HP: " << attrib.hp_current << SALTO;
        oss << " > MAX HP: " << attrib.hp_max << SALTO;
        oss << " > RANGO ATAQUE: " << attrib.range_attack << SALTO;
        oss << " > LEVEL: " <<attrib.difficulty_level << SALTO;
        std::string message = oss.str();
        print_message_console(message);
    }
}

void printPositionNPC(const Id &id, TypeNPC type, const Position &pos) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] - NPC: " << npcToString(type);
        oss << "| ID: "<< id ;
        oss << "---" << "Pos: (" << pos.x << ", " << pos.y << ")" << " |";
        std::string message = oss.str();
        print_message_console(message);
    }
}

void printPositionGold(const Id& id, GoldBagInstance go) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] - BOLSA DE ORO: " << go.amount;
        oss << "| ID: "<< id ;
        oss << "---" << "Pos: (" << go.pos.x << ", " << go.pos.y << ")" << " |";
        std::string message = oss.str();
        print_message_console(message);
    }

}

void printPositionItem(const ItemInstance& item) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] - ITEM: " << itemToString(item.type);
        oss << "| ID: "<< item.id ;
        oss << "---" << "Pos: (" << item.pos.x << ", " << item.pos.y << ")" << " |";
        std::string message = oss.str();
        print_message_console(message);
    }

}

void printCreatureLoads(const std::map<std::string, CreatureConfig>& info_npcs) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::vector<std::string> renglones;
        for (const auto& [name, npc]: info_npcs) {
            renglones.push_back("");  // Línea
            renglones.push_back(" - Creature: " + npc.name);
            renglones.push_back(" - Type:        " + std::to_string(npc.type));
            renglones.push_back(" - hp_max_initial: " + std::to_string(npc.hp_max_initial));
            renglones.push_back(" - attack_range: " + std::to_string(npc.attack_range));
            renglones.push_back(" - minimal_level: " + std::to_string(npc.minimal_level));
            renglones.push_back(" - maximun_level: " + std::to_string(npc.maximun_level));
            renglones.push_back("");
        }
        size_t max_largo = 0;
        for (const auto& r: renglones) max_largo = std::max(max_largo, r.length());
        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde(ancho_caja, '-');
        std::ostringstream oss;
        oss << borde << SALTO;
        for (const auto& renglon: renglones) {
            int espacios = static_cast<int>(max_largo - renglon.length());
            oss << "| " << renglon << std::string(espacios, ' ') << " |" << SALTO;
        }
        oss << borde;
        print_message_console(oss.str());
    }
}

void printNpcsSafeLoads(const std::map<std::string, NpcSafeZone>& info_npcs) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::vector<std::string> renglones;
        for (const auto& [name, npc]: info_npcs) {
            renglones.push_back("");  // Línea
            renglones.push_back(" - NPC: " + npc.name);
            renglones.push_back(" - Type:        " + std::to_string(npc.type));
            renglones.push_back(" - Store: ");
            for (const auto& id: npc.ids_items_store) {
                renglones.push_back("    > " + itemToString(id));
            }
            renglones.push_back("");
        }
        size_t max_largo = 0;
        for (const auto& r: renglones) max_largo = std::max(max_largo, r.length());
        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde(ancho_caja, '-');
        std::ostringstream oss;
        oss << borde << SALTO;
        for (const auto& renglon: renglones) {
            int espacios = static_cast<int>(max_largo - renglon.length());
            oss << "| " << renglon << std::string(espacios, ' ') << " |" << SALTO;
        }
        oss << borde;
        print_message_console(oss.str());
    }
}
void printRacesLoad(const std::map<TypeRace, Race>& info_races) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::vector<std::string> renglones;
        for (const auto& [type, race]: info_races) {
            renglones.push_back("");  // separador entre razas
            renglones.push_back(" - Raza:             " + race.name);
            renglones.push_back(" - type:             " + std::to_string(race.type));
            renglones.push_back(" - hp_factor:        " + std::to_string(race.hp_factor));
            renglones.push_back(" - recovery_factor:  " + std::to_string(race.recovery_factor));
            renglones.push_back(" - mana_factor:      " + std::to_string(race.mana_factor));
            renglones.push_back(" - Estadisticas:");
            renglones.push_back("   > Intelligence:   " +
                                std::to_string(race.statistics.intelligence));
            renglones.push_back("   > Constitution:   " +
                                std::to_string(race.statistics.constitution));
            renglones.push_back("   > Strength:       " + std::to_string(race.statistics.strength));
            renglones.push_back("   > Agility:        " + std::to_string(race.statistics.agility));
            renglones.push_back("");  // línea vacía al final de cada raza
        }
        size_t max_largo = 0;
        for (const auto& r: renglones) max_largo = std::max(max_largo, r.length());

        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde(ancho_caja, '-');

        std::ostringstream oss;
        oss << borde << SALTO;
        for (const auto& renglon: renglones) {
            int espacios = static_cast<int>(max_largo - renglon.length());
            oss << "| " << renglon << std::string(espacios, ' ') << " |" << SALTO;
        }
        oss << borde;
        print_message_console(oss.str());
    }
}

void printClasesLoad(const std::map<TypeClase, Clase>& info_clases) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::vector<std::string> renglones;
        for (const auto& [type, clase]: info_clases) {
            renglones.push_back("");  // Línea
            renglones.push_back(" - Clase:               " + clase.name);
            renglones.push_back(" - type:               " + std::to_string(clase.type));
            renglones.push_back(" - meditation_factor:  " +
                                std::to_string(clase.meditation_factor));
            renglones.push_back(" - mana_factor:        " + std::to_string(clase.mana_factor));
            renglones.push_back("- Estadisticas:     ");
            renglones.push_back("> Intelligence  " + std::to_string(clase.statistics.intelligence));
            renglones.push_back("> Constitution: " + std::to_string(clase.statistics.constitution));
            renglones.push_back("> Strength:     " + std::to_string(clase.statistics.strength));
            renglones.push_back("> Agility:      " + std::to_string(clase.statistics.agility));
            renglones.push_back("");
        }
        size_t max_largo = 0;
        for (const auto& r: renglones) max_largo = std::max(max_largo, r.length());

        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde(ancho_caja, '-');

        std::ostringstream oss;
        oss << borde << SALTO;
        for (const auto& renglon: renglones) {
            int espacios = static_cast<int>(max_largo - renglon.length());
            oss << "| " << renglon << std::string(espacios, ' ') << " |" << SALTO;
        }
        oss << borde;
        print_message_console(oss.str());
    }
}

void printPositionRandom(const Position& pos) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - World] - RANDOM | Pos: (" << pos.x << ", " << pos.y << ")" << " |";
        std::string message = oss.str();
        print_message_console(message);
    }
}

 void printInitGameloop(std::string mess) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << "[DEBUG - Init] ----- " << mess << " |";
        std::string message = oss.str();
        print_message_console(message);
    }
}

}
 // namespace Print
