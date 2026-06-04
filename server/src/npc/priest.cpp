#include "../../includes/npc/priest.h"

Priest::Priest(Id id, TypeNPC type, std::string&& name, Position&& pos,
               std::map<TypeItem, std::unique_ptr<Item>>&& store_, GameFormulas& formulas) :
        CityNPC(id, type, std::move(name), std::move(pos), formulas),
        store(std::move(store_)) {}
        
// void Priest::interact(const Id& id_player, World& world, Comand& cmd) {}
