#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/map.h"

/*VER SI ESTO PUEDE IR EN OTRO LADO*/
enum BodyPart : uint8_t {
    NO_BODY = 0,
    HEAD = 1, /*Cabeza - casco, capucha, sombrero*/
    BACK,     /*Dorso - armadura, tunica*/
    HAND,     /*Mano* - escudo, arma, objeto */
    MOUTH,    /*Boca - posiciones*/
};


struct Item {
    TypeItem type;
    explicit Item(TypeItem type_): type(type_) {}
    // Item() = default;

    // virtual bool use(Player& user) {
    //     throw std::runtime_error("Este objeto no se puede usar directamente desde el inventario.");
    // }
    virtual ~Item() = default;
};

struct GoldPouche: Item {
    uint32_t amount;
    Position pos;
    // GoldPouches() = default;
    explicit GoldPouche(TypeItem type, Position pos_, uint32_t amount_):
            Item(type), amount(amount_), pos(pos_) {}
};

/*Un ShopItem es equipable y almacenable en un inventario/tienda */
// consultar si es preferible usar una clase. Lo hice un struct porque no como tal no maneja logica.
struct ShopItem: Item {
    TypeItem type;/*quiero cambiar esto */
    BodyPart body_part_use{NO_BODY};      //= NO_BODY;
    ItemClassification classif{NO_CLASSIF};  // = NO_CLASS;
    std::string name;
    uint32_t selling_price{0};
    uint32_t purchase_price{0};


    ShopItem(TypeItem type, BodyPart body, ItemClassification classif_, const std::string& name,
             uint32_t sell_price, uint32_t purch_price):
            Item(type),
            body_part_use(body),
            classif(classif_),
            name(name),
            selling_price(sell_price),
            purchase_price(purch_price) {}

    virtual bool is_equpped() { return body_part_use != MOUTH; }
    virtual ~ShopItem() = default;
};

/* Con esto puedo representar mis objetos de defensa:
    - Armaduras.
    - Tunica.
    - Escudos.
    - Casco, capucha, sombrero.
*/
struct Defense: ShopItem {
    uint16_t minimal_defense;  //= 0;
    uint16_t maximun_defense;  //= 0;

    // Defense() = default;
    Defense(TypeItem type, BodyPart body, ItemClassification classif, const std::string& name,
            uint32_t sell_price, uint32_t purch_price, uint16_t min_def, uint16_t max_def):
            ShopItem(type, body, classif, name, sell_price, purch_price),
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
    uint16_t minimal_damage;  // = 0;
    uint16_t maximun_damage;  // = 0;
    uint16_t range_attack;    // = 0;

    // Weapon() = default;
    Weapon(TypeItem type, BodyPart body, ItemClassification classif, const std::string& name,
           uint32_t sell_price, uint32_t purch_price, uint16_t min_dam, uint16_t max_dam,
           uint16_t r_attack):
            ShopItem(type, body, classif, name, sell_price, purch_price),
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

    // ObjectMagic() = default;
    ObjectMagic(TypeItem type, BodyPart body, ItemClassification classif, const std::string& name,
                uint32_t sell_price, uint32_t purch_price, uint16_t m_cost, uint16_t range):
            ShopItem(type, body, classif, name, sell_price, purch_price),
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

    // MagicWeapon() = default;
    MagicWeapon(TypeItem type, BodyPart body, ItemClassification classif, const std::string& name,
                uint32_t sell_price, uint32_t purch_price, uint16_t min_dam, uint16_t max_dam,
                uint16_t m_cost, uint16_t range):
            Weapon(type, body, classif, name, sell_price, purch_price, min_dam, max_dam, range),
            mana_cost(m_cost) {}
};

struct Potion: ShopItem {
    uint16_t restore_amount;

    Potion(TypeItem type, BodyPart body, ItemClassification classif, const std::string& name,
           uint16_t sell_price, uint16_t purch_price, uint16_t rest_amount):
            ShopItem(type, body, classif, name, sell_price, purch_price),
            restore_amount(rest_amount) {}


    // bool use(Player& user) override {
    //     if (this->type == TypeItem::LIFE_POTION) {
    //         user.restoreHp(this->restore_amount);
    //     } else if (this->type == TypeItem::MANA_POTION) {
    //         user.restoreMana(this->restore_amount);
    //     }
    //     return true;
    // }
};
#endif
