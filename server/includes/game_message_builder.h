#ifndef GAME_MESSAGE_BUILDER_H
#define GAME_MESSAGE_BUILDER_H
#include <cstdint>
#include <string>

namespace GameMessageBuilder {
std::string damageMessgToThePlayer(const std::string& attacker_name, const uint16_t& damage,
                                   const bool& is_died);
std::string messgAboutDamageDealtByThePlayer(const std::string& victim_name, const uint16_t& damage,
                                             const bool& victim_died);
std::string messageReportingMditationStatus(const std::string& name);
std::string messgAttackDodged(const std::string& attacker_name, const std::string& victim_name);
std::string messgTheOpponentDodgedTheAttack(const std::string& victim_name);
std::string mssgeFromAClanMemberUnderAttack(const std::string& victim_name);
}  // namespace GameMessageBuilder


#endif  // TALLER_TP_GAME_MESSAGE_BUILDER_H
