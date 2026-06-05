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
    FIELD_GOBLIN = 1, /*GOBLIN DE CAMPO*/
    FOREST_GOBLIN,    /*GOBLIN DE BOSQUE*/
    SKELETON,         /*ESQUELETO*/
    WARRIOR_SKELETON, /*ESQUELETO GUERRERO*/
    ZOMBIE,           /*ZOMBIE*/
    DUNGEON_SPIDER,   /*ARANA DE MAZMORRA*/
    CAVE_SPIDER,      /*ARANA DE CAVERNA*/
    FOREST_SPIDER,    /*ARANA DE BOSQUE*/
    ORC,              /*ORCO*/
    GOLEM,            /*GOLEM*/
    INFERNAL_GOLEM,   /*GOLEM INFERNAL*/
    PRIEST,           /*SACERDOTA*/
    BANKER,           /*BANQUERO*/
    MERCHANT          /*COMERCIANTE*/
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
/*-------------------------------------------------------------------------------------------------*/
#ifndef __ID_BODY__
#define __ID_BODY__

enum TypeHumanBody : uint8_t {
    CITIZEN_LEATHER,  // 1036: Vestimenta de cuero marrón con verde y botas de caña alta
    CITIZEN_BLUE,     // 1056: Vestimenta de tela/básica azul con pantalones oscuros
    CITIZEN_RED,      // 1070: Vestimenta de tela/básica roja con pechera/tirantes negros
    PALADIN_PLATE,  // 1194: Armadura metálica de mallas pesada, cubierta por un tabardo naranja con
                    // una cruz
};

enum TypeElfBody : uint8_t {
    MAGE_ROBE_NAVY,     // 1689: Túnica larga de mago azul marino / monje oscuro
    NOBLE_COAT_BLUE,    // 1896: Saco/Sajón largo azul con detalles dorados y camisa blanca abajo
    DRUID_ROBE_TEAL,    // 1897: Túnica larga verde azulada/esmeralda con finos detalles amarillos
                        // (estilo elfo de los bosques)
    WIZARD_STARS_ROBE,  // 1900: Túnica clásica de archimago/hechicero gris oscura estampada con
                        // estrellas
};

enum TypeDwarfBody : uint8_t {
    WORKER_RED,       // 1681: Ropa de trabajo con remera roja, pantalones verdes y cinturón grueso
    ROYAL_TUNIC,      // 1718: Túnica elegante color rojo oscuro/bordó con bordes dorados
    CHAINMAIL_ARMOR,  // 1837: Cota de malla metálica gris con cinturón de cuero marrón
    ADVENTURER_ORANGE,  // 1921: Chaleco/chaqueta naranja sobre camisa blanca y pantalones oscuros
};

enum TypeGnomeBody : uint8_t {
    FORESTER_LEAVES,  // 1700: Ropa camuflada verde con patrón de hojas/manchas salvajes
    PIRATE_PANTS,  // 1710: Torso descubierto (sin remera) con pantalones bombachos rojos/atigrados
    ASSASSIN_LEATHER,  // 1841: Pechera de cuero ligera con tiras negras y guantes (estilo
                       // pícaro/asesino)
    HUNTER_FUR,  // 1861: Chaleco de piel/pelaje marrón moteado (estilo cazador o bárbaro gnomo)
};

#endif
/*-------------------------------------------------------------------------------------------------*/
#ifndef __ID_HEAD__
#define __ID_HEAD__

enum TypeHumanHead : uint8_t {
    BROWN_SPIKY_BEARD,    // 3060: Pelo castaño de punta, barba corta
    BROWN_LONG_GOATEE,    // 3061: Pelo castaño largo, candado/chiva
    DARK_RED_FULL_BEARD,  // 3062: Pelo rojizo oscuro, barba completa
    BLUE_PONYTAIL,        // 3063: Pelo azul, colita de caballo / peinado hacia atrás
    BROWN_MESSY_BEARD,    // 3064: Pelo castaño desarreglado, barba completa
    BLONDE_WILD,          // 3065: Pelo rubio salvaje/puntiagudo
    RED_SPIKY_BEARD,      // 3066: Pelo rojo brillante, barba con candado
};

enum TypeElfHead : uint8_t {
    BLONDE_LONG,  // 3000: Pelo rubio largo
    GREY_LONG,    // 3001: Pelo gris oscuro/negro largo
    BLUE_LONG,    // 3002: Pelo azul oscuro largo
    SILVER_LONG,  // 3003: Pelo plateado/canoso largo
    RED_SPIKY,    // 3004: Pelo rojo brillante despeinado/puntiagudo, ojos azules
    GREEN_SPIKY,  // 3005: Pelo verde oscuro puntiagudo, ojos azules
};

enum TypeDwarfHead : uint8_t {
    BROWN_SHORT_BEARD,  // 3020: Pelo castaño claro, barba corta
    GREY_SHORT_BEARD,   // 3021: Pelo gris, barba corta (igual diseño que 3020 pero gris)
    BROWN_FULL_BEARD,   // 3022: Pelo castaño con barba tupida que cubre la boca
    PURPLE_MOUSTACHE,   // 3023: Pelo violeta claro/grisáceo, bigote largo sin barba, ojos azules
    LIGHT_BROWN_LINE,   // 3024: Pelo marrón suave, barba recortada en línea
    SILVER_BRAIDED,     // 3025: Pelo plateado/blanco, barba trenzada o recortada prolija
};

enum TypeGnomeHead : uint8_t {
    BROWN_WILD_BEARD,    // 3040: Pelo castaño oscuro con barba y bigote salvajes, ojos verdes
    BROWN_LONG_BEARD,    // 3041: Pelo castaño oscuro, barba muy larga hacia abajo
    BROWN_CLEAN_SHAVEN,  // 3042: Pelo castaño, cara limpia sin barba/bigote
    GREY_CLEAN_SHAVEN,   // 3043: Pelo gris/azul metálico, cara limpia sin barba
    GREY_WILD_BEARD,     // 3044: Pelo gris claro, barba y bigotes largos (estilo sabio)
    BALD_GREY_BEARD,     // 3045: Calvo arriba, pelo y barba gris oscuro a los lados
};

#endif
/*-------------------------------------------------------------------------------------------------*/
#endif
