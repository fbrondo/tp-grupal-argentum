---
layout: default
title: Configuración
---

# Configuración del Juego

Argentum Online usa archivos TOML para toda su configuración. Esta sección explica cada archivo y sus parámetros.

---

## Ubicación de Archivos de Configuración

### Modo Desarrollo (sin instalar)

```
tp-grupal-argentum/
├── config.toml                    # Configuración global
├── client/assets/config/          # Configuración del cliente
└── server/config/                 # Configuración del servidor
    ├── paths.toml
    ├── game.toml
    ├── items.toml
    ├── creatures.toml
    ├── races.toml
    ├── clases.toml
    ├── regions.toml
    ├── npcs.toml
    ├── building.toml
    └── data/
        └── mapa-*.toml
```

### Modo Instalado

```
~/.config/argentum/server/config/  # Configuración del servidor
~/.local/share/argentum/client/    # Assets del cliente
~/.local/share/argentum/common/    # Assets compartidos
```

---

## Archivo Principal: `config.toml`

Ubicación: Raíz del proyecto (`config.toml`)

```toml
[server]
port = 8080                    # Puerto del servidor
max_players = 100              # Máximo de jugadores simultáneos
tick_rate_ms = 50              # Intervalo del loop principal (ms)

[game]
inventory_size = 20            # Slots del inventario
clan_max_members = 16          # Jugadores máximo por clan

[balance]
exp_limit_base = 1000          # Límite XP = base * nivel^1.8
gold_max_base = 100            # Oro máximo = base * nivel^1.1
gold_excess_factor = 0.5       # +50% tolerancia de oro

[npc]
spawn_interval_s = 30          # Segundos entre ciclos de spawn
max_population = 200           # NPCs máximos en el mapa

[gameplay]
ms_per_step = 264              # Milisegundos por paso lógico

[animations]
frame_rate_walk_ms = 66        # Duración de cuadro de caminata

[window]
fullscreen = false             # Modo pantalla completa
width = 960                    # Ancho de ventana
height = 540                   # Alto de ventana
```

---

## Configuración del Servidor (`server/config/`)

### `paths.toml`

Archivo maestro que referencia todos los demás archivos:

```toml
game = "game.toml"
items = "items.toml"
creatures = "creatures.toml"
races = "races.toml"
clases = "clases.toml"
regions = "regions.toml"
npcs = "npcs.toml"
building = "building.toml"
data_path = "data"
```

### `game.toml`

Configuración general del gameplay:

- Estadísticas iniciales del jugador
- Reglas de clan
- Temporizadores de spawn
- Cooldowns de ataque
- Parámetros de persistencia

### `items.toml`

Define todos los items del juego:

```toml
# Ejemplo de definición de arma
[[item]]
id = 1
name = "Espada de Madera"
type = "weapon"
damage = 5
value = 100
```

Items disponibles: armas (espadas, hachos, martillos, arcos, bastones), armaduras (cuero, placa, túnicas, cascos, escudos), pociones.

### `creatures.toml`

Define las criaturas del mundo:

```toml
# Ejemplo de criatura
[[creature]]
id = 300
name = "Goblin"
life = 50
damage = 8
experience = 25
```

11 criaturas: goblins, esqueletos, zombies, arañas, orcos, gólems.

### `races.toml`

Stats de las 4 razas:

| Raza | Factor Vida | Factor Mana | Factor Recuperación |
|------|-------------|-------------|---------------------|
| Humano | 1.0 | 1.0 | 1.0 |
| Elfo | 0.8 | 1.2 | 1.1 |
| Enano | 1.3 | 0.7 | 0.9 |
| Gnomo | 0.9 | 1.3 | 1.2 |

### `clases.toml`

Stats de las 4 clases:

| Clase | Factor Vida | Factor Meditación | Factor Mana |
|-------|-------------|-------------------|-------------|
| Mago | 0.7 | 1.5 | 1.5 |
| Clérigo | 1.0 | 1.2 | 1.2 |
| Paladín | 1.2 | 1.0 | 0.8 |
| Guerrero | 1.5 | 0.7 | 0.5 |

### `regions.toml`

Define las 7 regiones del mundo:

1. **Cavern** - Cueva subterránea
2. **Dungeon** - Mazmorra
3. **Forest** - Bosque
4. **Desert** - Desierto
4. **Field** - Campo abierto
6. **City** - Ciudad principal
7. **Town** - Pueblo

Cada región define: zona segura, criaturas, tesoro, y edificios.

### `npcs.toml`

Define los NPCs comerciales:

- **Mercader**: Vende armas y armaduras
- **Sacerdote**: Cura y resucita
- **Banquero**: Almacena oro y objetos

### `building.toml`

Mapea edificios a NPCs y reglas de detección.

### `data/mapa-*.toml`

15 archivos de mapas que definen el mundo del juego:

- `mapa-0.toml` hasta `mapa-final.toml`
- `iglesia.toml` (iglesia)
- Y otros mapas especializados

---

## Configuración del Cliente (`client/assets/config/`)

### `items.toml`

Catálogo de texturas de items para el cliente:

```toml
[[item]]
id = 1
texture = "Sprites/Items/1.png"
```

### `npcs.toml`

Texturas de criaturas y NPCs ciudadanos.

### `bodies.toml` y `heads.toml`

Sprites de cuerpo y cabeza por raza.

### `hud.toml`

Texturas de la interfaz (HUD): barras de vida, inventario, consola.

### `animated_items.toml`

Spritesheets de equipamiento animado (armas, escudos, cascos).

---

## Personalización

### Cambiar el Puerto del Servidor

Editá `config.toml`:

```toml
[server]
port = 9090  # Puerto personalizado
```

### Cambiar el Tamaño de Ventana

```toml
[window]
width = 1920
height = 1080
fullscreen = true
```

### Agregar una Nueva Criatura

1. Editá `server/config/creatures.toml`
2. Agregá la definición con ID único
3. Agregá el sprite en `client/assets/Sprites/Units/criatures/`
4. Actualizá `client/assets/config/npcs.toml`

### Crear un Nuevo Mapa

Usá el [Editor de Mapas](editor.md) o creá manualmente un archivo TOML en `server/config/data/`.

---

<div style="margin-top: 30px;">
  <a href="compilacion.md">← Compilación</a> | <a href="servidor.md">Siguiente: Servidor →</a>
</div>
