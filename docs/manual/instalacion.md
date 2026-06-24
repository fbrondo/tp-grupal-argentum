---
layout: default
title: Instalación
---

# Instalación

Argentum Online ofrece dos métodos de instalación: un **instalador automático** (recomendado) y una **instalación manual**.

---

## Instalación Automática (Recomendado)

El instalador se encarga de todo: instalar dependencias, compilar el proyecto y colocar los archivos en su lugar.

### Paso 1: Clonar el Repositorio

Abrí una terminal y ejecutá:

```bash
git clone https://github.com/fbrondo/tp-grupal-argentum.git
cd tp-grupal-argentum
```

### Paso 2: Ejecutar el Instalador

```bash
chmod +x installer.sh
./installer.sh
```

Verás el siguiente menú:

```
=================================================
=          ARGENTUM ONLINE INSTALLER            =
=================================================

Opciones de instalación:
  d  instalar dependencias del sistema (SDL, Qt6, etc.)
  i  compilar e instalar el juego (requiere dependencias previas)
  a  todo: dependencias + compilar + instalar

Opciones de desinstalación:
  u  desinstalar el juego

Otras opciones:
  h  esta ayuda
  q  salir
```

### Paso 3: Seleccionar Opción

Para una instalación completa, escribí **`a`** y presioná Enter:

```
> Ingrese una opción: a
```

El instalador realizará los siguientes pasos automáticamente:

1. **Instala dependencias del sistema** (SDL2, Qt6, etc.)
2. **Compila el proyecto** (cliente, servidor, editor, launcher)
3. **Copia los binarios** a `~/.local/bin/`
4. **Copia los assets** a `~/.local/share/argentum/`
5. **Copia la configuración** a `~/.config/argentum/`

### Paso 4: Verificar la Instalación

Al finalizar, verás un mensaje como este:

```
=== INSTALACIÓN COMPLETA ===

  >> Ejecutar el servidor: taller_server <puerto>
  >> Ejecutar el editor:   taller_editor
  >> Ejecutar el cliente:  taller_client <host> <puerto>
  >> (Asegurese de que /home/usuario/.local/bin esté en su PATH)
```

### Paso 5: Agregar al PATH (si es necesario)

Si `taller_client` no se ejecuta, asegurate de que `~/.local/bin` esté en tu PATH:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Para hacerlo permanente, agregá la línea a tu `~/.bashrc` o `~/.zshrc`.

---

## Instalación Manual

Si preferís instalar paso a paso, seguí estas instrucciones.

### Paso 1: Instalar Dependencias del Sistema

En Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y \
    cmake build-essential git ca-certificates \
    libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
    libopusfile-dev libxmp-dev \
    libjack-jackd2-dev libfluidsynth-dev fluidsynth \
    libwavpack-dev libmodplug-dev libvorbis-dev libflac-dev \
    libmpg123-dev libogg-dev libfreetype6-dev libharfbuzz-dev \
    libasound2-dev libpulse-dev \
    libgl-dev qt6-base-dev qt6-tools-dev
```

### Paso 2: Clonar el Repositorio

```bash
git clone https://github.com/fbrondo/tp-grupal-argentum.git
cd tp-grupal-argentum
```

### Paso 3: Compilar

**Opción A**: Usando el Makefile (recomendado):

```bash
make
```

**Opción B**: Usando CMake directamente:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build/ -j$(nproc)
```

### Paso 4: Ejecutar (modo desarrollo)

Sin instalar, podés ejecutar directamente desde la carpeta `build/`:

```bash
# Terminal 1: Servidor
./build/taller_server 8080

# Terminal 2: Cliente
./build/taller_client localhost 8080
```

---

## Desinstalación

Para desinstalar Argentum Online:

```bash
./installer.sh
# Seleccionar opción 'u'
```

Esto eliminará:
- Binarios en `~/.local/bin/`
- Assets en `~/.local/share/argentum/`
- Configuración en `~/.config/argentum/`

---

## Estructura de Archivos Instalados

```
~/.local/
├── bin/
│   ├── taller_client
│   ├── taller_server
│   ├── taller_editor
│   └── taller_launcher
├── lib/
│   └── *.so (librerías compartidas)
└── share/argentum/
    ├── client/assets/
    └── common/
        ├── assets/
        └── data/

~/.config/argentum/
└── server/config/
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

---

<div style="margin-top: 30px;">
  <a href="requisitos.md">← Requisitos</a> | <a href="compilacion.md">Siguiente: Compilación →</a>
</div>
