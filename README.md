# TP Grupal - Argentum Online 

## Estructura del repositorio

```
client/
server/
editor/
common/
```

## Compilar
Ejecutar en la raiz del repositorio 

`make`

Para correr los tests

`make test`

### Launcher

El launcher (Qt6) está deshabilitado por defecto. Para compilarlo se requiere Qt6 instalado:

```bash
sudo apt install qt6-base-dev
```

Luego compilar habilitando el target:

```bash
make EXTRA_GENERATE="-DTALLER_LAUNCHER=ON"
```

## Dependencias
Se deben instalar las siguientes dependencias
```
sudo apt install \
  cmake \
  build-essential \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-mixer-dev \
  libsdl2-ttf-dev \
  libopusfile-dev \
  libxmp-dev \
  libjack-jackd2-dev \
  libfluidsynth-dev \
  fluidsynth \
  libwavpack-dev \
  libmodplug-dev \
  libvorbis-dev \
  libflac-dev \
  libmpg123-dev \
  libogg-dev \
  libfreetype6-dev \
  libharfbuzz-dev \
  libasound2-dev \
  libpulse-dev
```

## Pre-commit hooks

El repositorio usa [pre-commit](https://pre-commit.com/) para correr formatters y linters antes de cada commit:

- **clang-format** — formatea el código según `.clang-format`
- **cpplint** — verifica el estilo según `CPPLINT.cfg`
- **cppcheck** — análisis estático

### Instalación

```bash
pip install pre-commit
pre-commit install
```

### Uso manual

```bash
# Correr todos los hooks sobre todos los archivos
pre-commit run --all-files

# Aplicar solo el formatter
pre-commit run clang-format --all-files
```

## Docker

## CI/CD

## Links
[TOML C++ Marzer](https://marzer.github.io/tomlplusplus/)
