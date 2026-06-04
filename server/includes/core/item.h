#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <utility>

#include "common/includes/types.h"

/*VER SI ESTO PUEDE IR EN OTRO LADO*/
enum BodyPart : uint8_t {
    HEAD = 1, /*Cabeza - casco, capucha, sombrero*/
    BACK,     /*Dorso - armadura, tunica*/
    HAND,     /*Mano* - escudo, arma, objeto */
    MOUTH,    /*Boca - posiciones*/
};

/*Un Item es equipable y almacenable en un inventario/tienda */
// consultar si es preferible usar una clase. Lo hice un struct porque no como tal no maneja logica.
struct Item {
    TypeItem type;
    BodyPart body_part_use;
    ItemClassification classif;
    std::string name;
    uint16_t selling_price;
    uint16_t purchase_price;

    Item(TypeItem type, BodyPart body, ItemClassification classif_, std::string&& name,
         uint16_t sell_price, uint16_t purch_price):
            type(type),
            body_part_use(body),
            classif(classif_),
            name(std::move(name)),
            selling_price(sell_price),
            purchase_price(purch_price) {}
    virtual ~Item() = default;
};

/* Con esto puedo representar mis objetos de defensa:
    - Armaduras.
    - Tunica.
    - Escudos.
    - Casco, capucha, sombrero.
*/
struct Defense: Item {
    uint16_t minimal_defense;
    uint16_t maximun_defense;

    Defense(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
            uint16_t sell_price, uint16_t purch_price, uint16_t min_def, uint16_t max_def):
            Item(type, body, classif, std::move(name), sell_price, purch_price),
            minimal_defense(min_def),
            maximun_defense(max_def) {}
};

/* Con esto puedo representa las arma de combate a mano directamente:
    - Espada
    - Martillo
    - Hacha
    - Arco compuest
    - Arco simple
*/
struct Weapon: Item {
    uint16_t minimal_damage;
    uint16_t maximun_damage;
    uint16_t range_attack;

    Weapon(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
           uint16_t sell_price, uint16_t purch_price, uint16_t min_dam, uint16_t max_dam,
           uint16_t r_attack):
            Item(type, body, classif, std::move(name), sell_price, purch_price),
            minimal_damage(min_dam),
            maximun_damage(max_dam),
            range_attack(r_attack) {}
};

/* Con esto puedo representa las arma de combate a distancia:
    - Arco compuest
    - Arco simple
*/
// struct RangedWeapon: Weapon {
//     RangedWeapon(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
//                  uint16_t sell_price, uint16_t purch_price, uint16_t min_dam, uint16_t max_dam,
//                  uint16_t range):
//             Weapon(type, body, classif, std::move(name), sell_price, purch_price, min_dam,
//             max_dam, range){}
// };

/* Con esto puedo representar un objeto magico, el mas simple:
    - flauta elfica -> lanza hechico que cura vida
*/
struct ObjectMagic: Item {
    uint16_t mana_cost;
    uint16_t range;
    ObjectMagic(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
                uint16_t sell_price, uint16_t purch_price, uint16_t m_cost, uint16_t range):
            Item(type, body, classif, std::move(name), sell_price, purch_price),
            mana_cost(m_cost),
            range(range) {}
};
/* Con esto puedo representar las armas magicas:
    - Vara de fresno
    - Baculo engarzado.
    - Baculo nudoso.
*/
struct MagicWeapon: Weapon {
    uint16_t mana_cost;
    MagicWeapon(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
                uint16_t sell_price, uint16_t purch_price, uint16_t min_dam, uint16_t max_dam,
                uint16_t m_cost, uint16_t range):
            Weapon(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam,
                   range),
            mana_cost(m_cost) {}
};

#endif
