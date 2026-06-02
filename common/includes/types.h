#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>

/*ID es global, me sirve para identificar :
    - Jugadores
    - Items en inventarios o en el suelo
    - NPCs que estan por el mundo.
*/
#ifndef __ID__
#define __ID__
using Id = uint32_t;
#endif
/*--------------------------------------------------------------------------------------*/
/*ID de los tiles - Azulejo*/
#ifndef __ID_TILE__
#define __ID_TILE__
using IdTile = uint32_t;
#endif

/*-------------------------------------------------------------------------------------*/
/*ID de los tipos de Items - Armas, Objetos Magicos, Equipos de defensa, pociones*/
#ifndef __ID_ITEM__
#define __ID_ITEM__
enum TypeItem : uint8_t {
    SWORD = 1,       /*ESPADA*/
    AXE,             /*HACHA*/
    HAMMER,          /*MARTILLO*/
    ASH_STAFF,       /*VARA DE FRESNO*/
    ELVEN_FLUTE,     /*FLUTA ELFICA*/
    KNOTTED_STAFF,   /*BACULO NUDOSO*/
    INLAID_STAFF,    /*BACULO ENGARZADO*/
    SIMPLE_BOW,      /*ARCO SIMPLE*/
    COMPOUND_BOW,    /*ARCO COMPUESTO*/
    LEATHER_ARMOR,   /*ARMADURA DE CUERO*/
    PLATE_AMOR,      /*ARMADURA DE PLACAS*/
    BLUE_TUNIC,      /*TUNICA AZUL*/
    HOOD,            /*CAPUCHA*/
    IRON_HELMET,     /*CASCO DE HIERRO*/
    TORTOISE_SHIELD, /*ESCUDO DE TORUGA*/
    IRON_SHIELD,     /*ESCUDO DE HIERRO*/
    MAGIC_HAT,       /*SOMBRERO MAGICO*/
    LIFE_POTION,     /*POCION VIDA*/
    MANA_POTION      /*POCION MANA*/
};

enum ItemClassification : uint8_t {
    ITEM_DEFENSIVE = 1, /*ITEM DE DEFENSA*/
    ITEM_ATTACK,        /*ITEM DE ATAQUE*/
    ITEM_HEALING        /*ITEM DE CURACION*/
};

#endif
/*------------------------------------------------------------------------------------------------*/
#ifndef __ID_RACE__
#define __ID_RACE__
enum TypeRace : uint8_t {
    HUMAN = 1, /*HUMANO*/
    ELF,       /*ELFO*/
    DWARF,     /*ENANO*/
    GNOME      /*GNOMO*/
};
#endif

/*-------------------------------------------------------------------------------------------------*/
#ifndef __ID_CLASE__
#define __ID_CLASE__
enum TypeClase : uint8_t {
    WIZARD = 1, /*MAGO*/
    CLERIC,     /*CLERIGO*/
    PALADIN,    /*PALADIN*/
    WARRIOR     /*GUERRERO*/
};
#endif

/*-------------------------------------------------------------------------------------------------*/
#ifndef __ID_NPC__
#define __ID_NPC__
enum TypeNPC : uint8_t {
    GOBLIN = 1, /*GOBLIN*/
    SKELETON,   /*ESQUELETO*/
    ZOMBIE,     /*ZOMBIE*/
    SPIDER,     /*ARANA*/
    ORC,        /*ORCO*/
    GOLEM,      /*GOLEM*/
    PRIEST,     /*SACERDOTA*/
    BANKER,     /*BANQUERO*/
    MERCHANT    /*COMERCIANTE*/
};

#endif
/*-------------------------------------------------------------------------------------------------*/
// #ifndef __ID_REGION__
// #define __ID_REGION__
// enum TypeRegion : uint8_t {
//     FIELD = 1, /*CAMPO*/
//     FOREST,    /*BOSQUE*/
//     DESERT,    /*DESIERTO*/
//     CAVER,      /*CAVERNAS*/
//     DUNGEON,   /*MAZMORRA*/
//     CITY,       /*CIUDAD*/
//     TOWN,    /*PUEBLO*/
// };
// #endif

#endif
