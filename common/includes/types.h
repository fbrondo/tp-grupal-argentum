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
    NONE = 0,        /*NADA*/
    SWORD,           /*ESPADA*/
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
    MANA_POTION,     /*POCION MANA*/
    GOLD,            /*ORO*/
    TREASURE,
};

enum ItemClassification : uint8_t {
    NO_CLASSIF = 0,
    ITEM_DEFENSIVE, /*ITEM DE DEFENSA*/
    ITEM_ATTACK,    /*ITEM DE ATAQUE*/
    ITEM_HEALING    /*ITEM DE CURACION*/
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

#ifndef __ID_SOUND_EFFECT__
#define __ID_SOUND_EFFECT__
enum class SoundEffectID : uint16_t {
    // --- EFECTOS GENERALES Y CRIATURAS ---
    GRITO_RISA_MUJER_ECO = 1,
    GOLPE_ARMA = 2,
    SPAWN_NPC = 3,
    CERDO = 4,
    PUERTA_ABRIENDOSE = 5,
    SUBIR_NIVEL_ANTIGUO = 6,
    COMIENDO = 7,
    MUGIDO_VACA = 8,
    AULLIDO_LOBO = 9,
    GOLPE_RECIBIDO = 10,
    MUERTE_HOMBRE = 11,
    GOLPE_ARMA_2 = 12,
    TALAR = 13,
    PESCAR = 14,
    MINAR = 15,
    INMOVILIZAR = 16,
    CURAR = 17,
    CURAR_2 = 18,
    FLECHA_MAGICA = 19,
    RESUCITAR = 20,
    PAJARITOS = 21,
    PAJARITOS_2 = 22,
    PASO = 23,
    PASO_2 = 24,
    EQUIPAR_ARMA = 25,
    MOSQUITO = 26,
    TORMENTA_DE_FUEGO = 27,
    PAJARITOS_GRILLOS = 28,
    PAJARITOS_3 = 29,
    GRAN_DRAGON_LEVIATAN = 30,
    GRAN_DRAGON_2 = 31,
    GRAN_DRAGON_3 = 32,
    ZOMBIE = 33,
    PAJARITOS_4 = 34,
    LOBO_ATACANDO = 35,
    RESORTE_EXPLOSIVO = 36,
    RECHAZO_ESCUDO_CASCO = 37,
    LAUD_COMIENZO_HISTORIA = 38,
    LAUD_TERROR = 39,
    LAUD_SIMILAR_COMIENZO = 40,
    MARTILLAZOS = 41,
    TALAR_2 = 42,
    LLAMADA_CLAN = 43,
    CREACION_CLAN = 44,
    TROMPETAS = 45,
    TOMAR_LIQUIDO_POTAS = 46,
    PULGA = 47,
    REDOBLANTES = 48,
    LAUD_REPETIDO_38 = 49,
    MAR = 50,
    FLAUTA = 51,
    MOSCAS = 52,
    AGUILA = 53,
    LEVIATAN = 54,
    RED_DE_PESCA = 55,
    CABALLO_RELINCHANDO = 56,
    CABALLO_RELINCHANDO_2 = 57,
    CAMPANADA_RELOJ_ANTIGUO = 58,
    CERDO_ENOJADO = 59,
    CUERVOS = 60,
    CUERVOS_2 = 61,
    CUERVO = 62,
    TIJERAS = 63,
    TALAR_HACHAZO = 64,
    HACHAZO_GOLPE_CLAVADO = 65,
    ENGRANAJE = 66,
    GALLO_CANTANDO = 67,
    GOLPES_PISO = 68,
    PASOS = 69,
    CABALGATA = 70,
    CABALGATA_2 = 71,
    GRITO_CORTO_MUJER = 72,
    PERRO_POSEIDO = 73,
    GRITO_AL_MORIR_MUJER = 74,
    GRITO_CORTO_MUJER_2 = 75,
    GOLPE_SECO_PUERTA = 76,
    LATIDO_FIN_FUERZA_AGILIDAD = 77,
    OSO = 78,
    LEON = 79,
    LLUVIA = 80,
    AULLIDO_LOBO_2 = 81,
    COMER = 82,
    MONO_1 = 83,
    MONO_2 = 84,
    MONO_3 = 85,
    MOSCA_MOLESTA = 86,
    MAR_2 = 87,
    MAR_3 = 88,
    OSO_2 = 89,
    OVEJA = 90,
    CABRA = 91,
    MONO_4 = 92,
    PALOMA = 93,
    PASOS_EN_GRAVA = 94,
    PATO = 95,
    PATO_2 = 96,
    PERRO = 97,
    CERDO_2 = 98,
    RISA_MALVADA_HOMBRE_CORTA = 99,
    RISA_MALVADA_HOMBRE_LARGA = 100,
    RANA = 101,
    RANA_2 = 102,
    CASCABEL_SERPIENTE = 103,
    TOS_HOMBRE = 104,
    TRUENO = 105,
    CAMPANITAS = 106,
    VIENTO_FUERTE_LARGO = 107,
    VIENTO_FUERTE = 108,
    DIOSES_JOMJOMJOM = 109,
    APERTURA_MAGICA = 110,
    LANZAR_HECHIZO = 111,
    SONIDO_FUERZA_AGILIDAD = 112,
    LANZAR_HECHIZO_2 = 113,
    LANZAR_HECHIZO_3 = 114,
    BURBUJAS = 115,
    INCENDIO = 116,
    RESUCITAR_SACERDOTE = 117,
    GRITO_BRUJA = 118,
    GRITO_HOMBRE = 119,
    CORO_RARO = 120,
    ESTRUCTURA_CAYENDO = 121,
    CUERNO_GUERRA_GAITA = 122,
    POCION_INVISIBILIDAD_TOLKIEN = 123,
    LOBO_AULLANDO = 124,
    MURCIELAGOS = 125,
    SONIDO_ASESINO = 126,
    SERPIENTE = 127,
    SERPIENTE_2 = 128,
    MUCHOS_LOBOS_AULLANDO = 129,
    OVEJA_BEBE = 130,
    GRILLOS = 131,
    DROP_ESPECIAL_NPC = 132,
    GOLPE_CON_ECO = 133,
    ACOPLE_CORTO = 134,
    TOMAR_POCION = 135,
    GOLPE_CON_ECO_2 = 136,
    GALLO_CANTANDO_2 = 137,
    OSO_3 = 138,
    OSO_4 = 139,
    SONIDO_BESTIA = 140,
    LYNN = 141,
    GALLO_MURIENDO = 142,
    MONSTRUO_MARINO = 143,
    RISA_MALVADA_MUJER = 144,
    RISA_MALVADA_MUJER_2 = 145,
    CURACION_DIVINA = 146,
    PORTAL_MAGICO = 147,
    PORTAL_MAGICO_2 = 148,
    DISPARO_GALOPE = 149,
    CAMPANADA_RELOJ_ANTIGUO_2 = 150,
    MOTOR_ARRANCANDO = 151,
    FLAUTIN = 152,
    FLAUTIN_2 = 153,
    FLAUTIN_3 = 154,
    HOMBRE_MUERTE_DISPARO = 155,
    RISA_MALVADA_HOMBRE = 156,
    GAS = 157,
    MEDITACION = 158,
    CAMPANADAS = 161,
    APOCALIPSIS = 162,
    SONIDO_NO_MUERTO = 168,
    GOLPE_CAIDA_AGUA = 180,
    LLUVIA_2 = 191,
    LLUVIA_3 = 192,
    MAR_4 = 193,
    LLUVIA_4 = 194,
    LLUVIA_PARANDO = 195,
    PASO_3 = 197,
    PASO_4 = 198,
    PASO_5 = 199,
    PASO_6 = 200,
    PASO_7 = 201,
    PESCANDO = 202,
    DISPARO_2 = 203,
    CAPTURANDO_ALGO = 204,
    CAPTURANDO_ALGO_2 = 205,
    TAMBOR = 206,
    SIN_DISTINGUIR = 207,
    CHATARRA = 208,
    GOTA_CON_ECO = 209,
    CLICK = 210,
    DISPARO_3 = 211,
    ALIENTO_DRAGON = 231,
    CURACION_DIVINA_2 = 238,
    FLECHA = 249,
    GRITO_GUERRA_ARMONIA = 251,
    FUEGOS_ARTIFICIALES = 264,
    DISPARO_4 = 312,
    LYNN_2 = 345,
    NPC_ATRAPA = 349,
    NPC_ATRAPA_2 = 358,
    FUEGO_ARTIFICIAL = 375,
    FUEGO_ARTIFICIAL_2 = 376,
    ATAQUE_CADENA_PESADA = 380,
    TRUENO_2 = 400,
    TRUENO_3 = 401,
    TRUENO_4 = 402,
    TRUENO_DESAPARECIENDO = 403,
    TRUENO_FUERTE = 404,
    PRE_TRUENO = 405,
    POST_TRUENO = 406,
    ATRAPADA = 448,
    ONDA = 449,
    MONEDAS = 453,
    ONDA_2 = 463,
    POCION_STAT_IAO = 481,
    DEPOSITAR_RETIRAR_BOVEDA = 500,
    DEPOSITAR_RETIRAR_BOVEDA_2 = 501,
    DESCARGA_ELECTRICA = 520,
    MENSAJE_PRIVADO = 522,
    RUNA = 528,
    SUBIR_LEVEL_NUEVO = 554,
    ARENA_CAYENDO = 555,
    BURBUJAS_ALQUIMIA = 1152,
    TREX = 2323,
    PISTOLA_NUEVA = 2541,
    VELOCIRAPTOR = 2525,
    HIENA = 2626,

    // VOCES DE COMERCIANTES / BANQUEROS
    BIENVENIDO_A_MI_TIENDA = 600,
    BUENOS_DIAS = 601,
    COMO_PUEDO_AYUDAR = 602,
    LO_PIDES_LO_TIENES = 603,
    ME_PARECE_BIEN = 604,
    MIRA_CUANTO_QUIERAS = 605,
    TENGO_LO_QUE_BUSCAS = 606,
    UNA_GRAN_ELECCION = 607,
    UNA_GRANDIOSA_IDEA = 608,
    VUELVE_PRONTO = 609,
    ANOS_TRAYECTORIA_COMPROMISO = 610,
    BIENVENIDO_AL_BANCO = 611,
    CUIDAMOS_SUS_COSAS = 612,
    PUEDE_CONFIAR_EN_NOSOTROS = 613,
    QUE_TRANSACCION_DESEA = 614,

    // VOCES DE SACERDOTES Y EVENTOS
    OIGO_TUS_PLEGARIAS = 620,
    ORARE_POR_TI = 621,
    QUE_LA_LUZ_GUIE = 622,
    TEN_CUIDADO_AHI_FUERA = 623,
    REDYMON_NOTICIAS = 648,
    EN_QUE_PUEDO_AYUDARTE = 700,
    OBSERVA_TENGO_LO_QUE_NECESITAS = 701,
    EXCELENTE_TRATO = 702,
    MEJOR_SALIR_PREPARADO = 703,
    TENGO_UNA_MISION = 704,
    EXCELENTE_TEN_TU_RECOMPENSA = 705,
    NO_ME_QUEDAN_MISIONES = 706,
    NUNCA_SE_SABE_QUE_PELIGROS = 707,
    PERFECTO_EXCELENTE_TRABAJO = 708,
    TE_GUSTARIA_PROBAR_VALOR = 709,

    // BIENVENIDAS LARGAS DE FACCIONES
    BIENVENIDO_TRISTEZA_AUSENCIA = 1190,
    BIENVENIDO_ENANO_ULTIMO_DIA = 1191,
    BIENVENIDO_ESPERANZA_FUERTES = 1192,
    BIENVENIDO_MALDITO_HONOR = 1193,
    BIENVENIDO_PAPEL_DESTINO = 1194,
    BIENVENIDO_LUZ_BLANCA = 1195,
    SITUACION_GUL_BELTHOR = 1219,

    // NPC JEFES / ENEMIGOS ESPECIFICOS
    AKSHA = 2028,
    BANDIDO = 2036,
    BANDIDO_2 = 2046,
    FALSO_ARRANQUE_ECO = 2052,
    ESPADAZO = 2058,
    RISA_MALVADA_HOMBRE_2 = 2102,
    LYNN_3 = 2114,
    YUNQUE = 2117,
    SIMILAR_APUNALADA = 2185,
    ESPADAZO_SANGRE_CAYENDO = 2186,
    APUNALADA = 2187,
    ESCUDO_ANTE_TODO = 2188,
    ESCUDO_ANTE_TODO_2 = 2189,
    CASCO_MUY_IMPORTANT = 2190,
    CASCO_MUY_IMPORTANT_2 = 2191,
    CASCO_MUY_IMPORTANT_3 = 2192,
    ARMADURAS_MAS_FUERTES = 2193,
    ARMADURAS_MAS_FUERTES_2 = 2194,
    TROZO_METAL_CUERPO = 2195,
    TROZO_METAL_CUERPO_2 = 2196,
    TROZO_METAL_CUERPO_3 = 2197,
    ESCUDO_ANTE_TODO_3 = 2198,
    ARMADURAS_MAS_FUERTES_3 = 2199,
    BUENAS_TARDES_SENOR = 2200,
    NOS_VEMOS_PRONTO = 2201,
    BUENAS_TARDES_SENOR_2 = 2202,
    BUENAS_TARDES_SENOR_3 = 2203,
    NOS_VEMOS_PRONTO_2 = 2204,
    NOS_VEMOS_PRONTO_3 = 2205,
    CRONICA_TV = 9999,

    // CASOS ESPECIALES
    DADOS = 50001,               // cupdice.wav
    FUEGO = 50002,               // fuego.wav
    ARPA = 50003,                // harp3.wav
    LLUVIA_EMPEZANDO = 50004,    // lluviain.wav
    LLUVIA_TERMINANDO = 50005,   // lluviainend.wav
    LLUVIA_EMPEZANDO_2 = 50006,  // lluviainst.wav
    LLUVIA_AFUERA = 50007,       // lluviaout.wav
    LLUVIA_AFUERA_FIN = 50008,   // lluviaoutend.wav
    LLUVIA_AFUERA_ST = 50009     // lluviaoutst.wav
};

#endif
#ifndef __ID_BODY__
#define __ID_BODY__

enum TypeHumanBody : uint16_t {
    CITIZEN_LEATHER = 1036,  // Vestimenta de cuero marrón con verde y botas de caña alta
    CITIZEN_BLUE = 1056,     // Vestimenta de tela/básica azul con pantalones oscuros
    CITIZEN_RED = 1070,      // Vestimenta de tela/básica roja con pechera/tirantes negros
    PALADIN_PLATE = 1194,    // Armadura metálica de mallas pesada, cubierta por un tabardo naranja
};

enum TypeElfBody : uint16_t {
    MAGE_ROBE_NAVY = 1689,     // Túnica larga de mago azul marino / monje oscuro
    NOBLE_COAT_BLUE = 1896,    // Saco/Sajón largo azul con detalles dorados
    DRUID_ROBE_TEAL = 1897,    // Túnica larga verde azulada/esmeralda
    WIZARD_STARS_ROBE = 1900,  // Túnica clásica de archimago/hechicero
};

enum TypeDwarfBody : uint16_t {
    WORKER_RED = 1681,         // Ropa de trabajo con remera roja
    ROYAL_TUNIC = 1718,        // Túnica elegante color rojo oscuro/bordó
    CHAINMAIL_ARMOR = 1837,    // Cota de malla metálica gris
    ADVENTURER_ORANGE = 1921,  // Chaleco/chaqueta naranja
};

enum TypeGnomeBody : uint16_t {
    FORESTER_LEAVES = 1700,   // Ropa camuflada verde
    PIRATE_PANTS = 1710,      // Torso descubierto con pantalones bombachos
    ASSASSIN_LEATHER = 1841,  // Pechera de cuero ligera
    HUNTER_FUR = 1861,        // Chaleco de piel/pelaje marrón
};

#endif

#ifndef __ID_HEAD__
#define __ID_HEAD__

enum TypeHumanHead : uint16_t {
    BROWN_SPIKY_BEARD = 3060,    // Pelo castaño de punta, barba corta
    BROWN_LONG_GOATEE = 3061,    // Pelo castaño largo, candado/chiva
    DARK_RED_FULL_BEARD = 3062,  // Pelo rojizo oscuro, barba completa
    BLUE_PONYTAIL = 3063,        // Pelo azul, colita de caballo
    BROWN_MESSY_BEARD = 3064,    // Pelo castaño desarreglado
    BLONDE_WILD = 3065,          // Pelo rubio salvaje/puntiagudo
    RED_SPIKY_BEARD = 3066,      // Pelo rojo brillante, barba con candado
};

enum TypeElfHead : uint16_t {
    BLONDE_LONG = 3000,  // Pelo rubio largo
    GREY_LONG = 3001,    // Pelo gris oscuro/negro largo
    BLUE_LONG = 3002,    // Pelo azul oscuro largo
    SILVER_LONG = 3003,  // Pelo plateado/canoso largo
    RED_SPIKY = 3004,    // Pelo rojo brillante despeinado
    GREEN_SPIKY = 3005,  // Pelo verde oscuro puntiagudo
};

enum TypeDwarfHead : uint16_t {
    BROWN_SHORT_BEARD = 3020,  // Pelo castaño claro, barba corta
    GREY_SHORT_BEARD = 3021,   // Pelo gris, barba corta
    BROWN_FULL_BEARD = 3022,   // Pelo castaño con barba tupida
    PURPLE_MOUSTACHE = 3023,   // Pelo violeta, bigote largo
    LIGHT_BROWN_LINE = 3024,   // Pelo marrón suave, barba recortada
    SILVER_BRAIDED = 3025,     // Pelo plateado/blanco
};

enum TypeGnomeHead : uint16_t {
    BROWN_WILD_BEARD = 3040,    // Pelo castaño oscuro con barba salvaje
    BROWN_LONG_BEARD = 3041,    // Pelo castaño oscuro, barba muy larga
    BROWN_CLEAN_SHAVEN = 3042,  // Pelo castaño, cara limpia
    GREY_CLEAN_SHAVEN = 3043,   // Pelo gris/azul metálico
    GREY_WILD_BEARD = 3044,     // Pelo gris claro, barba larga
    BALD_GREY_BEARD = 3045,     // Calvo arriba, barba gris
};

#endif

#endif
