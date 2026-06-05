#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/map.h"

/*VER SI ESTO PUEDE IR EN OTRO LADO*/
enum BodyPart : uint8_t {
    HEAD = 1, /*Cabeza - casco, capucha, sombrero*/
    BACK,     /*Dorso - armadura, tunica*/
    HAND,     /*Mano* - escudo, arma, objeto */
    MOUTH,    /*Boca - posiciones*/
};


struct Item {
    TypeItem type;
    Item() = default;
    Item(TypeItem type): type(type) {}
    virtual ~Item() = default;
};
struct GoldPile: Item {
    uint32_t amount;
    Position pos;
};

/*Un ShopItem es equipable y almacenable en un inventario/tienda */
// consultar si es preferible usar una clase. Lo hice un struct porque no como tal no maneja logica.
struct ShopItem : Item { /*quiero cambiar esto */
    BodyPart body_part_use;
    ItemClassification classif;
    std::string name;
    uint16_t selling_price;
    uint16_t purchase_price;

    ShopItem() = default;
    ShopItem(TypeItem type, BodyPart body, ItemClassification classif_, std::string&& name, uint16_t sell_price, uint16_t purch_price):
            Item(type),
            body_part_use(body),
            classif(classif_),
            name(std::move(name)),
            selling_price(sell_price),
            purchase_price(purch_price) {}
    virtual ~ShopItem() = default;
};

/* Con esto puedo representar mis objetos de defensa:
    - Armaduras.
    - Tunica.
    - Escudos.
    - Casco, capucha, sombrero.
*/
struct Defense: ShopItem {
    uint16_t minimal_defense;
    uint16_t maximun_defense;

    Defense() = default;
    Defense(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
            uint16_t sell_price, uint16_t purch_price, uint16_t min_def, uint16_t max_def):
            ShopItem(type, body, classif, std::move(name), sell_price, purch_price),
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
struct Weapon: ShopItem {
    uint16_t minimal_damage;
    uint16_t maximun_damage;
    uint16_t range_attack;

    Weapon() = default;
    Weapon(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
           uint16_t sell_price, uint16_t purch_price, uint16_t min_dam, uint16_t max_dam,
           uint16_t r_attack):
            ShopItem(type, body, classif, std::move(name), sell_price, purch_price),
            minimal_damage(min_dam),
            maximun_damage(max_dam),
            range_attack(r_attack) {}
    virtual ~Weapon() = default;
};
/* Con esto puedo representar un objeto magico, el mas simple:
    - flauta elfica -> lanza hechico que cura vida
*/
struct ObjectMagic: ShopItem {
    uint16_t mana_cost;
    uint16_t range;

    ObjectMagic() = default;
    ObjectMagic(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
                uint16_t sell_price, uint16_t purch_price, uint16_t m_cost, uint16_t range):
            ShopItem(type, body, classif, std::move(name), sell_price, purch_price),
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

    MagicWeapon() = default;
    MagicWeapon(TypeItem type, BodyPart body, ItemClassification classif, std::string&& name,
                uint16_t sell_price, uint16_t purch_price, uint16_t min_dam, uint16_t max_dam,
                uint16_t m_cost, uint16_t range):
            Weapon(type, body, classif, std::move(name), sell_price, purch_price, min_dam, max_dam,
                   range),
            mana_cost(m_cost) {}
};

#endif
