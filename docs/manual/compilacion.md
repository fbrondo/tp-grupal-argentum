---
layout: default
title: Compilación
---

# Compilación del Proyecto

Si preferís compilar manualmente en vez de usar el instalador, esta sección explica el proceso paso a paso.

---

## Prerrequisitos

Antes de compilar, asegurate de tener instaladas las [dependencias del sistema](requisitos.md).

---

## Métodos de Compilación

### Método 1: Usando Make (Recomendado)

El repositorio incluye un `Makefile` que simplifica los comandos de CMake.

**Compilación debug:**

```bash
make
```

Esto ejecuta automáticamente:

```bash
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
cmake --build build/
```

**Compilar con el Launcher Qt6 habilitado:**

```bash
make EXTRA_GENERATE="-DTALLER_LAUNCHER=ON"
```

**Ejecutar tests:**

```bash
make test
```

### Método 2: Usando CMake Directamente

Si necesitás más control sobre la compilación:

**Paso 1: Configurar el proyecto**

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DARGENTUM_SHARE_PATH="$HOME/.local/share/argentum" \
    -DARGENTUM_CONFIG_PATH="$HOME/.config/argentum/server/config"
```

**Paso 2: Compilar**

```bash
cmake --build build/ -j$(nproc)
```

El flag `-j$(nproc)` usa todos los cores disponibles para compilar más rápido.

---

## Opciones de CMake

| Opción | Default | Descripción |
|--------|---------|-------------|
| `TALLER_CLIENT` | `ON` | Compilar el cliente SDL2 |
| `TALLER_SERVER` | `ON` | Compilar el servidor headless |
| `TALLER_EDITOR` | `ON` | Compilar el editor Qt6 |
| `TALLER_LAUNCHER` | `OFF` | Compilar el launcher Qt6 de login/registro |
| `TALLER_TESTS` | `ON` | Compilar los tests unitarios (Google Test) |
| `TALLER_MAKE_WARNINGS_AS_ERRORS` | `ON` | Tratar warnings como errores |
| `CMAKE_BUILD_TYPE` | - | `Debug`, `Release`, o `RelWithDebInfo` |
| `ARGENTUM_SHARE_PATH` | - | Ruta donde se instalarán los assets |
| `ARGENTUM_CONFIG_PATH` | - | Ruta donde se instalará la configuración |

### Ejemplo: Compilar Solo el Servidor

```bash
cmake -S . -B build \
    -DTALLER_CLIENT=OFF \
    -DTALLER_EDITOR=OFF \
    -DTALLER_LAUNCHER=OFF \
    -DTALLER_TESTS=OFF
cmake --build build/
```

### Ejemplo: Compilar Solo el Cliente

```bash
cmake -S . -B build \
    -DTALLER_SERVER=OFF \
    -DTALLER_EDITOR=OFF \
    -DTALLER_LAUNCHER=OFF \
    -DTALLER_TESTS=OFF
cmake --build build/
```

---

## Estructura del Directorio build/

Después de compilar, encontrarás los siguientes archivos en `build/`:

```
build/
├── taller_client          # Cliente del juego
├── taller_server          # Servidor del juego
├── taller_editor          # Editor de mapas
├── taller_launcher        # Launcher (si está habilitado)
├── taller_tests           # Tests unitarios
└── lib/                   # Librerías compartidas
    └── taller_common/     # Librería compartida
```

---

## Ejecutar Tests

Para ejecutar la suite de tests unitarios:

```bash
# Usando Make
make test

# O manualmente
./build/taller_tests
```

Los tests verifican:
- Parsing de archivos TOML
- Serialización de mapas
- Carga de sprites
- Protocolo de red

---

## Compilation Flags Importantes

El proyecto usa C++20. Asegurate de que tu compilador soporte este estándar:

```bash
# Verificar versión de GCC
gcc --version

# Verificar versión de CMake
cmake --version
```

Si tu compilador no soporta C++20, necesitarás actualizarlo:

```bash
# Ubuntu/Debian: instalar GCC más reciente
sudo apt install gcc-13 g++-13
```

---

## Solución de Errores de Compilación

Si encontrás errores durante la compilación, consultá la sección de [Solución de Problemas](troubleshoot.md).

---

<div style="margin-top: 30px;">
  <a href="instalacion.md">← Instalación</a> | <a href="configuracion.md">Siguiente: Configuración →</a>
</div>
