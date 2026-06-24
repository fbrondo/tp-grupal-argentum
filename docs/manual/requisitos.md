---
layout: default
title: Requisitos del Sistema
---

# Requisitos del Sistema

Antes de instalar Argentum Online, verificá que tu cumpla con los siguientes requisitos.

---

## Sistema Operativo

| Sistema | Versión Mínima | Estado |
|---------|----------------|--------|
| **Ubuntu** | 22.04 LTS | Recomendado |
| **Debian** | 12 (Bookworm) | Soportado |
| **Linux Mint** | 21+ | Soportado |

> **Nota**: El juego fue desarrollado y probado en Ubuntu 24.04. Otras distribuciones Linux pueden funcionar pero no están garantizadas.

---

## Herramientas de Compilación

| Herramienta | Versión Mínima | Propósito |
|-------------|----------------|-----------|
| **CMake** | 3.24 | Sistema de build |
| **GCC** | 13.0 | Compilador C++ (soporte C++20) |
| **Git** | 2.0 | Control de versiones |
| **Make** | 4.0 | Build automation |

---

## Dependencias del Sistema

### Gráficos y Audio (SDL2)

| Librería | Versión | Propósito |
|----------|---------|-----------|
| `libsdl2-dev` | 2.30.x | Ventanas, eventos, rendering |
| `libsdl2-image-dev` | 2.8.x | Carga de imágenes PNG |
| `libsdl2-mixer-dev` | 2.8.x | Mezcla y reproducción de audio |
| `libsdl2-ttf-dev` | 2.22.x | Renderizado de fuentes TrueType |

### Codecs de Audio

| Librería | Propósito |
|----------|-----------|
| `libopusfile-dev` | Codec Opus |
| `libxmp-dev` | Tracker de módulos |
| `libvorbis-dev` | Codec Vorbis |
| `libflac-dev` | Codec FLAC |
| `libmpg123-dev` | Codec MP3 |
| `libogg-dev` | Contenedor Ogg |
| `libwavpack-dev` | Codec WavPack |
| `libmodplug-dev` | Codecs MOD/S3M/XM |

### Audio del Sistema

| Librería | Propósito |
|----------|-----------|
| `libasound2-dev` | Backend ALSA |
| `libpulse-dev` | Backend PulseAudio |
| `libjack-jackd2-dev` | Backend JACK |

### Renderizado de Fuentes

| Librería | Propósito |
|----------|-----------|
| `libfreetype6-dev` | Renderizado de fuentes |
| `libharfbuzz-dev` | Motor de layout de texto |

### Sintetizador MIDI (Opcional)

| Paquete | Propósito |
|---------|-----------|
| `libfluidsynth-dev` | Sintetizador FluidSynth |
| `fluidsynth` | Servidor FluidSynth |

### Editor y Launcher (Opcional - Qt6)

| Librería | Versión | Propósito |
|----------|---------|-----------|
| `qt6-base-dev` | 6.4+ | Framework Qt6 para editor y launcher |
| `qt6-tools-dev` | 6.4+ | Herramientas Qt6 (designer, etc.) |

### Herramientas de Desarrollo (Opcional)

| Paquete | Propósito |
|---------|-----------|
| `libgl-dev` | OpenGL (para SDL2) |

---

## Dependencias Descargadas Automáticamente

Estas librerías se descargan y compilan automáticamente al ejecutar `make`:

| Librería | Versión | Propósito |
|----------|---------|-----------|
| **libSDL2pp** | latest | Binding C++ para SDL2 |
| **toml++** | 3.4.0 | Parser de archivos TOML |
| **Google Test** | latest | Framework de tests unitarios |

---

## Recomendaciones

- **RAM**: Mínimo 2 GB disponibles para compilación
- **Disco**: ~500 MB para el código fuente + dependencias compiladas
- **Internet**: Necesaria para descargar dependencias首次
- **Resolución**: 1280x720 o superior para el cliente

---

<div style="margin-top: 30px;">
  <a href="index.md">← Volver al índice</a> | <a href="instalacion.md">Siguiente: Instalación →</a>
</div>
