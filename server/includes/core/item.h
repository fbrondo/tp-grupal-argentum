#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <utility>

#include "common/includes/types.h"

/*VER SI ESTO PUEDE IR EN OTRO LADO*/
enum BodyPart : uint8_t {
    HEAD,  /*Cabeza - casco, capucha, sombrero*/
    BACK,  /*Dorso - armadura, tunica*/
    HAND,  /*Mano* - escudo, arma, objeto */
    MOUTH, /*Boca - posiciones*/
};

/*Un Item es equipable y almacenable en un inventario/tienda */
// consultar si es preferible usar una clase. Lo hice un struct porque no como tal no maneja logica.
struct Item {
    TypeItem type;
    std::string name;
    uint16_t sellingPrice;
    uint16_t purchasePrice;

    Item(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice):
            type(std::move(type)),
            name(std::move(name)),
            sellingPrice(sellingPrice),
            purchasePrice(purchasePrice) {}

    virtual ~Item() = default;
};

/* Con esto puedo representar mis objetos de defensa:
    - Armaduras.
    - Tunica.
    - Escudos.
    - Casco, capucha, sombrero.
*/
struct Defense: Item {
    uint16_t minimalDefense;
    uint16_t maximunDefense;
    BodyPart bodyPart; /*Parte del cuerpo donde se lleva el item defensivo*/

    Defense(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice,
            uint16_t minDef, uint16_t maxDef, BodyPart bp):
            Item(type, std::move(name), sellingPrice, purchasePrice),
            minimalDefense(minDef),
            maximunDefense(maxDef),
            bodyPart(bp) {}
};

/* Con esto puedo representar un objeto magico, el mas simple:
    - flauta elfica -> lanza hechico que cura vide
*/
struct ObjectMagic: Item {
    uint16_t manaCost;
    uint16_t range;

    ObjectMagic(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice,
                uint16_t mCost, uint16_t range):
            Item(type, std::move(name), sellingPrice, purchasePrice),
            manaCost(mCost),
            range(range) {}
};

/* Con esto puedo representa las arma de combate a mano directamente:
    - Espada
    - Martillo
    - Hacha
*/
struct Weapon: Item {
    uint16_t minimalDamage;
    uint16_t maximunDamage;

    Weapon(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice,
           uint16_t minDam, uint16_t maxDam):
            Item(type, std::move(name), sellingPrice, purchasePrice),
            minimalDamage(minDam),
            maximunDamage(maxDam) {}
};

/* Con esto puedo representa las arma de combate a distancia:
    - Arco compuest
    - Arco simple
*/
struct RangedWeapon: Weapon {
    uint16_t rangedAttack;

    RangedWeapon(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice,
                 uint16_t minDam, uint16_t maxDam, uint16_t range):
            Weapon(type, std::move(name), sellingPrice, purchasePrice, minDam, maxDam),
            rangedAttack(range) {}
};


/* Con esto puedo representar las armas magicas:
    - Baculo engarzado.
    - Baculo nudoso.
*/
struct MagicWeapon: Weapon {
    uint16_t manaCost;
    uint16_t range;

    MagicWeapon(TypeItem type, std::string&& name, uint16_t sellingPrice, uint16_t purchasePrice,
                uint16_t minDam, uint16_t maxDam, uint16_t mCost, uint16_t range):
            Weapon(type, std::move(name), sellingPrice, purchasePrice, minDam, maxDam),
            manaCost(mCost),
            range(range) {}
};


#endif
