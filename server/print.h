#ifndef PRINT_H
#define PRINT_H

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/core/user.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "includes/core/creature_attributes.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/definitions.h"

namespace Print {

void print_message_console(const std::string& message);
void printMessageConsole(const std::string& message);

std::string itemToString(TypeItem item);
std::string directionToString(Direction dir);
std::string RaceToString(TypeRace rac);
std::string claseToString(TypeClase clase);
std::string tileToString(Tile tile);
std::string npcToString(TypeNPC typeC);

void initServer();
/*World*/
void printInitMatrizMap(const std::vector<std::vector<Tile>>& map, uint32_t height, uint32_t width);
void printNpc(const NpcInstance& npc);
void printItem(const TreasureInstance& instance);
void printPositionRandom(const Position& pos);


void printPlayerData(std::string func, const PlayerData& player);

/*GAMELOADCONFIG*/
void printLoadPathsAndFiles(const Path& path, const PathsConfig& paths_config,
                            const FileData& file_data);
void draw_box(const std::string& title, const std::vector<std::string>& lines);
void printItems(const std::map<TypeItem, std::unique_ptr<Item>>& items);
void printCreatureLoads(const std::map<std::string, CreatureConfig>& info_npcs);
void printNpcsSafeLoads(const std::map<std::string, NpcSafeZone>& info_npcs);
void printRacesLoad(const std::map<TypeRace, Race>& info_races);
void printClasesLoad(const std::map<TypeClase, Clase>& info_clases);


void printNewPlayerArrived(const Id& id, const User& user, TypeRace rac, TypeClase cl);
void printInitProcessRegister();
void playerArrivedLogin(const Id& id, const User& user);
void playerLoad(std::string op, const PlayerData& player);
// void printPositionNewPlayer(const Id& id, const PlayerInstance& inst);
// void printPositionPlayerUpdate(const Id& id, const PlayerInstance& inst);

void printPositionTreasure(const Id& id, const Position& position);
void printPositionCreature(const Id& id, TypeNPC type, const Position& pos,
                           const NpcAttributes& attrib);
void printPositionNPC(const Id& id, TypeNPC type, const Position& pos);
void printPositionItem(const ItemInstance& item);
void printPositionGold(const Id& id, GoldBagInstance);


void printPositionMovePlayer(const Id& id, const Pose& pose, const Position& prev);
void imprimirCajaContenedora(const PlayerData& player);
void printInitGameloop(const std::string& message);
void imprimirTilesOcupadas(const std::unordered_map<Position, bool, PositionHash>& occupied_tiles);
};  // namespace Print

#endif
