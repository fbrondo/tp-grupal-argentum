# Qt6 Setup — Ubuntu 24.04

## Instalar dependencias

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libgl-dev qt6-base-dev qt6-tools-dev qt6-tools-dev-tools qtchooser
```

| Paquete | Descripción |
|---|---|
| `build-essential` | GCC, g++, make |
| `cmake` | Sistema de build (requiere 3.24+) |
| `libgl-dev` | Headers de OpenGL, requeridos por Qt6 GUI |
| `qt6-base-dev` | Headers de Qt6 Core/Gui/Widgets + herramientas `moc`, `uic`, `rcc` + config de CMake |
| `qt6-tools-dev` | Librería de Qt6 Designer |
| `qt6-tools-dev-tools` | Binario `designer` para edición visual de archivos `.ui` |

### QtCreator IDE

```bash
sudo apt-get install -y qtcreator
```

## Editar la UI visualmente

```bash
# Con el editor
designer editor/mainwindow.ui # O cualquier otro .ui

# Con QtCreator: abrir el archivo CMakeLists.txt raíz en QtCreator
```