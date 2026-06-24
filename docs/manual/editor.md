---
layout: default
title: Editor de Mapas
---

# Editor de Mapas (Qt6)

El editor de mapas es una herramienta gráfica que te permite crear y modificar los mapas del juego de forma visual e intuitiva.

---

## Requisitos

- Qt6 instalado (`qt6-base-dev`)
- El proyecto compilado con el editor habilitado

---

## Ejecución

### Modo Desarrollo (desde build/)

```bash
./build/taller_editor
```

### Modo Instalado

```bash
taller_editor
```

---

## Interfaz del Editor

![Editor de Mapas](assets/screenshots/editor.png){: .screenshot }

### Componentes Principales

1. **Barra de Menú**: Archivo, Editar, Ver, Ayuda
2. **Barra de Herramientas**: Acciones rápidas
3. **Paleta de Tiles**: Selección de tiles para pintar
4. **Vista del Mapa**: Área de edición principal
5. **Panel de Propiedades**: Información del tile seleccionado

---

## Crear un Nuevo Mapa

### Paso 1: Nuevo Mapa

1. Andá a **Archivo** → **Nuevo Mapa** (o presioná `Ctrl + N`)
2. Se abrirá el diálogo de nuevo mapa

![Diálogo Nuevo Mapa](assets/screenshots/nuevo-mapa.png){: .screenshot }

### Paso 2: Configurar Dimensiones

- **Ancho**: Cantidad de tiles horizontales (recomendado: 50-100)
- **Alto**: Cantidad de tiles verticales (recomendado: 50-100)
- **Nombre**: Nombre descriptivo del mapa

### Paso 3: Confirmar

Hacé click en **"Crear"** para generar el mapa vacío.

---

## Editar el Mapa

### Capas de Edición

El mapa tiene 4 capas que se superponen:

| Capa | Descripción | Uso |
|------|-------------|-----|
| **Background** | Suelo/base | Terreno, agua, hierba |
| **Details** | Detalles | Árboles, rocas, mobiliario |
| **Objects** | Objetos | Items interactuables |
| **Roof** | Techos | Techos de edificios |

### Seleccionar una Capa

En el panel de capas, seleccioná la capa que querés editar:

- **Background**: Para pintar el suelo
- **Details**: Para agregar detalles
- **Objects**: Para colocar objetos
- **Roof**: Para techos

### Seleccionar un Tile

1. En la **paleta de tiles**, navegá por las categorías
2. Hacé click en el tile que querés usar
3. El tile seleccionado se resaltará

### Pintar en el Mapa

1. Seleccioná la capa y el tile
2. Hacé click en el mapa para colocar el tile
3. **Click y arrastrá** para pintar múltiples tiles

### Herramientas de Edición

| Herramienta | Tecla | Descripción |
|-------------|-------|-------------|
| **Pincel** | `B` | Pinta tiles individuales |
| **Rectángulo** | `R` | Dibuja rectángulos de tiles |
- **Borrador** | `E` | Elimina tiles |
| **Selección** | `S` | Selecciona área |
| **Mover** | `M` | Mueve tiles seleccionados |

### Atajos de Teclado

| Acción | Atajo |
|--------|-------|
| Guardar | `Ctrl + S` |
| Guardar Como | `Ctrl + Shift + S` |
| Deshacer | `Ctrl + Z` |
| Rehacer | `Ctrl + Y` |
| Copiar | `Ctrl + C` |
| Pegar | `Ctrl + V` |
| Zoom In | `Ctrl + +` |
| Zoom Out | `Ctrl + -` |
| Reset Zoom | `Ctrl + 0` |

---

## Guardar y Cargar

### Guardar Mapa

1. **Archivo** → **Guardar** (`Ctrl + S`)
2. El mapa se guarda como archivo TOML en `server/config/data/`

### Guardar Como

1. **Archivo** → **Guardar Como** (`Ctrl + Shift + S`)
2. Elegí una ubicación y nombre

### Cargar Mapa

1. **Archivo** → **Abrir** (`Ctrl + O`)
2. Navegá hasta el archivo TOML del mapa
3. Hacé click en **"Abrir"**

---

## Formato del Archivo

El mapa se guarda como un archivo TOML con la siguiente estructura:

```toml
[map]
width = 100
height = 100
name = "mi-mapa"

[[layer]]
name = "background"
tiles = [
    [1, 1, 1, 1, ...],
    [1, 2, 2, 1, ...],
    ...
]

[[layer]]
name = "details"
tiles = [
    [0, 0, 0, 0, ...],
    [0, 5, 0, 0, ...],
    ...
]
```

---

## Tiles Disponibles

Los tiles están organizados por categorías:

### Background (Suelo)
- Hierba, tierra, arena
- Agua, lava
- Madera, piedra
- Alfombras

### Details (Detalles)
- Árboles, arbustos
- Rocas, montañas
- Muebles, postes
- Signos

### Objects (Objetos)
- Items recogibles
- Puntos de interacción
- Puertas

### Roof (Techos)
- Techo de madera
- Techo de piedra
- Techo de paja

---

## Publicar un Mapa

Para que un mapa esté disponible en el juego:

1. Guardá el mapa en `server/config/data/`
2. El nombre debe seguir el formato: `mapa-<nombre>.toml`
3. Actualizá `server/config/regions.toml` para incluir el nuevo mapa
4. Reiniciá el servidor

---

## Consejos

- **Usá la grilla**: La grilla ayuda a alinear los tiles correctamente
- **Trabajá por capas**: Primero background, luego details, objects, y roof
- **Guardá frecuentemente**: `Ctrl + S` cada pocos cambios
- **Probá en el cliente**: Cargá el mapa en el cliente para ver cómo se ve
- **Referenciá mapas existentes**: Abrí mapas originales para aprender la estructura

---

<div style="margin-top: 30px;">
  <a href="cliente.md">← Cliente</a> | <a href="launcher.md">Siguiente: Launcher →</a>
</div>
