#include "server/includes/npc/citizen_npc.h"

CitizenNPC::CitizenNPC(TypeNPC type, const std::string& name): type_npc(type), name(name) {}

TypeNPC CitizenNPC::getTypeNPC() { return this->type_npc; }
