#include "server/includes/game_message_builder.h"

#include <sstream>
namespace GameMessageBuilder {
std::string messgStatingCannotUseWeapon(const std::string& player_name) {
    std::ostringstream oss;
    oss << player_name << " no puedes atacar, tu arma se esta enfriando.";
    return oss.str();
}

std::string messgInterruptedMeditation(const std::string& player_name) {
    std::ostringstream oss;
    oss << player_name << " tu meditación es interrumpida.";
    return oss.str();
}

std::string damageMessgToThePlayer(const std::string& attacker_name, const uint16_t& damage,
                                   const bool& is_died) {
    std::ostringstream oss;
    oss << "Recibiste " << damage << " de daño de " << attacker_name << ".";
    if (is_died) {
        oss << " Has muerto.";
    }
    return oss.str();
}

std::string messgAboutDamageDealtByThePlayer(const std::string& victim_name, const uint16_t& damage,
                                             const bool& victim_died) {
    std::ostringstream oss;
    oss << "Infligiste " << damage << " de daño a " << victim_name << ".";
    if (victim_died) {
        oss << " " << victim_name << " ha muerto.";
    }
    return oss.str();
}

std::string messageReportingMditationStatus(const std::string& name) {
    std::ostringstream oss;
    oss << name << ", entrás en estado de meditación.";
    return oss.str();
}

std::string messgAttackDodged(const std::string& attacker_name, const std::string& victim_name) {
    std::ostringstream oss;
    oss << victim_name << " has esquivado el ataque de " << attacker_name << ".";
    return oss.str();
}

std::string messgTheOpponentDodgedTheAttack(const std::string& victim_name) {
    std::ostringstream oss;
    oss << victim_name << " esquivó tu ataque.";
    return oss.str();
}

std::string mssgeFromAClanMemberUnderAttack(const std::string& victim_name) {
    std::string notif = victim_name + " está siendo atacado!";
    return notif;
}

std::string messgThisNPCDoesNotbuyItem(const std::string& name_npc) {
    std::ostringstream oss;
    oss << "El " << name_npc << " no compra este item.";
    return oss.str();
}
}  // namespace GameMessageBuilder
