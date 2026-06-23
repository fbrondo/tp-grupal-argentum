#include "client/includes/core/item_names.h"

#include <algorithm>
#include <string>
#include <unordered_map>

const char* item_name(TypeItem type) {
    switch (type) {
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
            return "Tunica azul";
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
            return "?";
    }
}

std::string to_lower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

uint8_t resolve_item_id(const std::string& name) {
    static const std::unordered_map<std::string, uint8_t> item_names = {
            {"espada", SWORD},
            {"hacha", AXE},
            {"martillo", HAMMER},
            {"vara de fresno", ASH_STAFF},
            {"vara", ASH_STAFF},
            {"flauta elfica", ELVEN_FLUTE},
            {"flauta", ELVEN_FLUTE},
            {"baculo nudoso", KNOTTED_STAFF},
            {"baculo", KNOTTED_STAFF},
            {"baculo engarzado", INLAID_STAFF},
            {"arco simple", SIMPLE_BOW},
            {"arco", SIMPLE_BOW},
            {"arco compuesto", COMPOUND_BOW},
            {"armadura de cuero", LEATHER_ARMOR},
            {"armadura", LEATHER_ARMOR},
            {"armadura de placas", PLATE_AMOR},
            {"tunica azul", BLUE_TUNIC},
            {"tunica", BLUE_TUNIC},
            {"capucha", HOOD},
            {"casco de hierro", IRON_HELMET},
            {"casco", IRON_HELMET},
            {"escudo de tortuga", TORTOISE_SHIELD},
            {"escudo", TORTOISE_SHIELD},
            {"escudo de hierro", IRON_SHIELD},
            {"sombrero magico", MAGIC_HAT},
            {"sombrero", MAGIC_HAT},
            {"pocion de vida", LIFE_POTION},
            {"pocion vida", LIFE_POTION},
            {"pocion de mana", MANA_POTION},
            {"pocion mana", MANA_POTION},
            {"oro", GOLD},
    };
    auto it = item_names.find(to_lower(name));
    if (it != item_names.end())
        return it->second;
    return NONE;
}
