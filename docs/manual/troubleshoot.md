---
layout: default
title: Solución de Problemas
---

# Solución de Problemas

Esta sección cubre los errores más comunes y cómo resolverlos.

---

## Errores de Compilación

### "CMake no encontrado" o "versión de CMake insuficiente"

**Síntoma:**
```
CMake Error: Could not find CMAKE_MINIMUM_REQUIRED
```

**Solución:**
```bash
# Verificar versión actual
cmake --version

# Instalar CMake más reciente
sudo apt install cmake
# O descargar desde https://cmake.org/download/
```

**Requisito:** CMake 3.24 o superior.

---

### "Compilador no soporta C++20"

**Síntoma:**
```
error: 'concept' is not a member of 'std'
error: 'requires' keyword not found
```

**Solución:**
```bash
# Verificar versión de GCC
gcc --version

# Instalar GCC 13 o superior
sudo apt install gcc-13 g++-13

# Configurar para usar GCC 13
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
```

---

### "No se encuentra SDL2" o "SDL2 not found"

**Síntoma:**
```
Could not find SDL2
Missing: SDL2_INCLUDE_DIR SDL2_LIBRARY
```

**Solución:**
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

---

### "Qt6 no encontrado"

**Síntoma:**
```
Could not find a package configuration file provided by "Qt6"
```

**Solución:**
```bash
sudo apt install qt6-base-dev qt6-tools-dev
```

**Nota:** Qt6 solo es necesario para el editor y launcher. Si no los necesitás, compilá sin ellos:

```bash
cmake -S . -B build -DTALLER_EDITOR=OFF -DTALLER_LAUNCHER=OFF
```

---

### "Error de permisos al instalar"

**Síntoma:**
```
Permission denied
E: Could not open lock file
```

**Solución:**
```bash
# Usar sudo para las dependencias
sudo apt install ...

# O ejecutar el instalador con sudo
sudo ./installer.sh
```

---

### "No se encuentran archivos .so"

**Síntoma:**
```
error while loading shared libraries: libtaller_common.so
```

**Solución:**
```bash
# Copiar librerías a la ubicación correcta
cp build/lib/taller_common/*.so ~/.local/lib/

# O ejecutar con LD_LIBRARY_PATH
LD_LIBRARY_PATH=build/lib/taller_common ./build/taller_client localhost 8080
```

---

## Errores de Ejecución

### "Puerto ya en uso"

**Síntoma:**
```
Error: Address already in use
bind: Failed to bind
```

**Solución:**
```bash
# Ver qué proceso usa el puerto
lsof -i :8080

# Matar el proceso
kill -9 <PID>

# O usar otro puerto
./build/taller_server 9090
```

---

### "No se encuentran assets"

**Síntoma:**
```
Error: Could not load texture
Error: Asset not found
```

**Solución:**

Verificá las rutas de assets:

```bash
# Modo desarrollo: assets deben estar相对于 el ejecutable
ls client/assets/
ls common/assets/

# Modo instalado: verificar ~/.local/share/argentum/
ls ~/.local/share/argentum/client/assets/
ls ~/.local/share/argentum/common/assets/
```

Si faltan, compilá de nuevo o copialos manualmente.

---

### "Servidor no responde"

**Síntoma:**
El cliente no se conecta al servidor.

**Solución:**
1. Verificá que el servidor esté corriendo
2. Verificá la IP y puerto
3. Verificá el firewall:

```bash
# Verificar que el puerto esté abierto
netstat -tlnp | grep 8080

# Abrir puerto en firewall (Ubuntu)
sudo ufw allow 8080/tcp
```

---

### "Error de conexión con el servidor"

**Síntoma:**
```
Error: Connection refused
Error: Could not connect to server
```

**Solución:**
1. Verificá que el servidor esté corriendo
2. Verificá que la IP sea correcta
3. Verificá que no haya un firewall bloqueando
4. Probá con `localhost` para conexión local

---

### "No se carga el mapa"

**Síntoma:**
```
Error: Map file not found
Error: Invalid map format
```

**Solución:**
1. Verificá que los archivos `mapa-*.toml` estén en `server/config/data/`
2. Verificá el formato TOML del mapa
3. Regenerá el mapa con el [Editor](editor.md)

---

## Errores del Cliente

### "Ventana no aparece"

**Síntoma:**
El cliente se ejecuta pero no se ve la ventana.

**Solución:**
```bash
# Verificar drivers de video
glxinfo | grep "OpenGL version"

# Instalar drivers
sudo apt install mesa-utils

# Probar con variable de entorno
SDL_VIDEODRIVER=x11 ./build/taller_client localhost 8080
```

---

### "No hay sonido"

**Síntoma:**
El juego funciona pero no se escuchan efectos.

**Solución:**
```bash
# Verificar PulseAudio
pulseaudio --check

# Verificar volúmenes
pactl list sinks

# Probar con ALSA
SDL_AUDIODRIVER=alsa ./build/taller_client localhost 8080
```

---

### "FPS bajo o juego lento"

**Síntoma:**
El juego se ve lento o con bajo framerate.

**Solución:**
1. Cerrá otros programas
2. Bajá la resolución en `config.toml`:
   ```toml
   [window]
   width = 800
   height = 600
   ```
3. Compilá en modo Release:
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   ```

---

## Errores del Editor

### "Editor no compila"

**Síntoma:**
```
Qt6 not found
```

**Solución:**
```bash
sudo apt install qt6-base-dev qt6-tools-dev libgl-dev
```

---

### "No se guardan los cambios"

**Síntoma:**
Los cambios en el mapa no se guardan.

**Solución:**
1. Verificá permisos de escritura en `server/config/data/`
2. Usá **Archivo → Guardar Como** para verificar la ruta
3. Verificá el espacio en disco

---

## Errores del Launcher

### "Launcher no inicia"

**Síntoma:**
```
qt.qpa.plugin: Could not load the Qt platform plugin "xcb"
```

**Solución:**
```bash
# Instalar dependencias de Qt
sudo apt install libxcb-xinerama0 libxcb-cursor0

# O probar con otro driver
QT_QPA_PLATFORM=offscreen ./build/taller_launcher
```

---

## Errores de Persistencia

### "Datos de jugador perdidos"

**Síntoma:**
El progreso del jugador no se guarda.

**Solución:**
1. Verificá que `server/config/data/` tenga permisos de escritura
2. Verificá que el servidor no se cerró abruptamente (Ctrl+C está bien)
3. Verificá el espacio en disco

---

## Errores de Red

### "Timeout de conexión"

**Síntoma:**
La conexión tarda mucho o falla.

**Solución:**
1. Verificá la velocidad de la red
2. Verificá que no haya paquetes perdidos: `ping localhost`
3. Aumentá el timeout si es necesario

---

## Obtener Ayuda

Si no encontrás solución a tu problema:

1. Revisá los [Issues en GitHub](https://github.com/fbrondo/tp-grupal-argentum/issues)
2. Creá un nuevo issue con:
   - Descripción del problema
   - Mensajes de error
   - Versión del SO y compilador
   - Pasos para reproducir

---

<div style="margin-top: 30px;">
  <a href="jugando.md">← Cómo Jugar</a> | <a href="index.md">Volver al Inicio</a>
</div>
