---
layout: default
title: Servidor
---

# Levantar el Servidor

El servidor es el componente que ejecuta toda la lógica del juego: física, combate, NPCs, persistencia, y más. Funciona de manera "headless" (sin interfaz gráfica).

---

## Requisitos

- El proyecto compilado (ver [Compilación](compilacion.md))
- Si usás el instalador, los binarios están en `~/.local/bin/`

---

## Ejecución

### Modo Desarrollo (desde build/)

```bash
./build/taller_server 8080
```

### Modo Instalado

```bash
taller_server 8080
```

### Parámetros

| Parámetro | Descripción | Ejemplo |
|-----------|-------------|---------|
| `<puerto>` | Puerto TCP para escuchar conexiones | `8080` |

---

## Mensaje de Inicio

Al ejecutar el servidor, verás algo como:

```
=== ARGENTUM SERVER ===
Puerto: 8080
Máximo de jugadores: 100
Cargando configuración...
Cargando mapas...
Servidor listo para conexiones.
```

---

## Lo que Hace el Servidor

Al iniciar, el servidor:

1. **Carga la configuración** de `server/config/`
2. **Carga los 15 mapas** del mundo
3. **Spawn NPCs**: Mercaderes, sacerdotes, banqueros
4. **Spawn Criaturas**: Goblins, esqueletos, etc. en sus regiones
5. **Escucha conexiones** en el puerto especificado
6. **Ejecuta el game loop**: Física, combate, IA cada 50ms
7. **Persiste datos**: Guarda progreso de jugadores periódicamente

---

## Puerto

El puerto por defecto es **8080**. Podés cambiarlo:

```bash
# Usar otro puerto
./build/taller_server 9090
```

Asegurate de que el puerto esté disponible y no bloqueado por un firewall.

---

## Múltiples Servidores

Podés ejecutar varios servidores en diferentes puertos:

```bash
# Terminal 1
./build/taller_server 8080

# Terminal 2
./build/taller_server 9090
```

Cada servidor corre de forma independiente con su propia configuración.

---

## Detener el Servidor

Presioná `Ctrl + C` en la terminal donde está corriendo el servidor.

El servidor guardará automáticamente el estado de los jugadores antes de cerrar.

---

## Archivos Generados

El servidor crea los siguientes archivos de persistencia:

```
server/config/data/
├── players.dat    # Datos de los jugadores
├── players.idx    # Índice de jugadores
└── mapa-*.toml    # Mapas (solo lectura)
```

---

## Log y Depuración

El servidor imprime información en la terminal:

- Conexiones de jugadores
- Mensajes de chat
- Errores de red
- Eventos de combate

Para ver más detalles, compilá en modo Debug:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build/
```

---

## Solución de Problemas

| Problema | Solución |
|----------|----------|
| "Puerto ya en uso" | Cambiá el puerto o mate el proceso que lo usa |
| "No se encuentran mapas" | Verificá que `server/config/data/` tenga los archivos `mapa-*.toml` |
| "Configuración no encontrada" | Verificá las rutas en `server/config/paths.toml` |

Para más errores, consultá [Solución de Problemas](troubleshoot.md).

---

<div style="margin-top: 30px;">
  <a href="configuracion.md">← Configuración</a> | <a href="cliente.md">Siguiente: Cliente →</a>
</div>
