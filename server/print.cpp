#include "print.h"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include "common/includes/direction.h"
#define SALTO "\n"


namespace Print {
void print_message_console(const std::string& message) {
    std::cout << message << std::endl;
}

std::string directionToString(Direction dir) {
    switch (dir) {
        case DOWN: return "DOWN";
        case UP: return "UP";
        case LEFT:  return "LEFT";
        case RIGHT:  return "RIGHT";
        default: return "UNKNOWN";
    }
}

std::string RaceToString(TypeRace rac) {
    switch (rac) {
        case HUMAN: return "HUMANO";
        case ELF: return "ELFO";
        case DWARF:  return "ENANO";
        case GNOME:  return "GNOME";
        default: return "UNKNOWN";
    }
}

std::string claseToString(TypeClase clase) {
    switch (clase) {
        case WIZARD: return "MAGO";
        case CLERIC: return "CLERIGO";
        case PALADIN:  return "PALADIN";
        case WARRIOR:  return "GUERRERO";
        default: return "UNKNOWN";
    }
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

void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config, const FileData& file_data) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
       std::vector<std::string> renglones = {
            "CARGANDO: " + path.filename().string() + " ---- " + path.string(),
            "", // Línea vacía
            "Rutas de Archivos de configuracion:",
            " - Game:    " + paths_config.game.string(),
            " - Races:   " + paths_config.races.string(),
            " - Clases:  " + paths_config.clases.string(),
            " - NPCs:    " + paths_config.npcs.string(),
            " - Items:   " + paths_config.items.string(),
            " - Regions: " + paths_config.regions.string(),
            "", // Línea vacía
            "Rutas de Archivos de Datos:",
            " - Players: " + file_data.players.string(),
            " - Index:   " + file_data.indx_players.string(),
            " - World:   " + file_data.world.string(),
            " - Map:     " + file_data.map.string()
       };
        size_t max_largo = 0;
        for (const auto& renglon : renglones) {
            if (renglon.length() > max_largo) {
                max_largo = renglon.length();
            }
        }
        // El ancho interior de la caja será el largo máximo más los espacios de cortesía a los costados
        int ancho_caja = static_cast<int>(max_largo) + 4;
        std::string borde_horizontal(ancho_caja, '-');
        //Empezamos a armar el ostringstream
        std::ostringstream oss;
        // Techo de la caja
        oss << borde_horizontal << SALTO;
        // Recorremos los renglones guardados y los rellenamos dinámicamente
        for (const auto& renglon : renglones) {
            oss << "| " << renglon;
            // Calculamos cuántos espacios le faltan a ESTE renglón específico para alcanzar al más largo
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

void printNewPlayerArrived(const Id& id, const std::string& name, const std::string& pass, TypeRace rac, TypeClase clase) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        oss << " ========== Client Arrived =========" << "\n";
        oss << "- id: " << id << "\n";
        oss << "- username: " << name << "\n";
        oss << "- pass: " << pass << "\n";
        oss << "- race: " << RaceToString(rac) << "\n";
        oss << "- clase: " << claseToString(clase);
        oss << " ===================================" << "\n";
        std::string message = oss.str();
        print_message_console(message);
    }
}
void printPositionNewPlayer(const Id& id, const PlayerInstance& inst) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        auto pos = inst.position;
        auto dir = inst.direct;
        oss << "[DEBUG] NEW PlayerID: " << id
                  << " | Pos: (" << pos.x << ", " << pos.y << ")"
                  << " | Dir: " << directionToString(dir);
        std::string message = oss.str();
        print_message_console(message);
    }
}

void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst) {
    const char* env_p = std::getenv("DEBUG");
    bool debug_mode = (env_p != nullptr && std::string(env_p) == "1");
    if (debug_mode) {
        std::ostringstream oss;
        auto pos = inst.position;
        auto dir = inst.direct;
        oss << "[DEBUG] PlayerID: " << id
                  << " | Pos: (" << pos.x << ", " << pos.y << ")"
                  << " | Dir: " <<  directionToString(dir);
        std::string message = oss.str();
        print_message_console(message);
    }
}
}

