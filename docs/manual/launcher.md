---
layout: default
title: Launcher
---

# Launcher Qt6 (Login/Registro)

El launcher es una aplicación gráfica que simplifica el proceso de login, registro de cuentas y selección de personajes. Está deshabilitado por defecto y debe habilitarse durante la compilación.

---

## Habilitar el Launcher

El launcher se desactiva por defecto porque requiere Qt6. Para habilitarlo:

### Paso 1: Instalar Qt6

```bash
sudo apt install qt6-base-dev qt6-tools-dev
```

### Paso 2: Compilar con Launcher Habilitado

```bash
# Usando Make
make EXTRA_GENERATE="-DTALLER_LAUNCHER=ON"

# O con CMake directamente
cmake -S . -B build -DTALLER_LAUNCHER=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build/
```

### Verificar Compilación

Después de compilar, verificá que exista el binario:

```bash
ls -la build/taller_launcher
```

---

## Ejecución

### Modo Desarrollo (desde build/)

```bash
./build/taller_launcher
```

### Modo Instalado

```bash
taller_launcher
```

---

## Pantallas del Launcher

### Pantalla de Login

![Login del Launcher](assets/screenshots/launcher-login.png){: .screenshot }

**Campos:**

- **Usuario**: Tu nombre de usuario
- **Contraseña**: Tu contraseña

**Botones:**

- **Iniciar Sesión**: Conecta al servidor
- **Registrarse**: Abre la pantalla de registro
- **Salir**: Cierra el launcher

### Pantalla de Registro

![Registro del Launcher](assets/screenshots/launcher-signup.png){: .screenshot }

**Campos:**

- **Usuario**: Nombre de usuario único (3-20 caracteres)
- **Contraseña**: Contraseña segura (mínimo 6 caracteres)
- **Confirmar Contraseña**: Repetir la contraseña

**Validaciones:**

- El usuario no puede contener espacios ni caracteres especiales
- Las contraseñas deben coincidir
- La contraseña debe tener al menos 6 caracteres

**Botones:**

- **Crear Cuenta**: Registra el nuevo usuario
- **Volver**: Regresa a la pantalla de login

### Pantalla de Selección de Personaje

![Selección de Personaje](assets/screenshots/launcher-character.png){: .screenshot }

Si ya tenés un personaje creado, aparecerá en esta pantalla.

**Opciones:**

- **Seleccionar**: Entra al juego con el personaje elegido
- **Crear Nuevo**: Abre el diálogo de creación de personaje
- **Eliminar**: Elimina el personaje seleccionado (con confirmación)

### Creación de Personaje

**Campos:**

- **Nombre**: Nombre único del personaje
- **Raza**: Selección de raza (Humano, Elfo, Enano, Gnomo)
- **Clase**: Selección de clase (Mago, Clérigo, Paladín, Guerrero)

**Información mostrada:**

- Stats base de la raza
- Bonificaciones de la clase
- Descripción del estilo de juego

---

## Conexión con el Servidor

El launcher se conecta automáticamente al servidor configurado. Por defecto:

- **Host**: `localhost`
- **Puerto**: `8080`

### Cambiar Servidor

Si el servidor está en otra máquina, editá la configuración o pasá los parámetros por línea de comandos:

```bash
./build/taller_launcher --host 192.168.1.100 --port 9090
```

---

## Flujo Normal de Uso

1. **Abrir el launcher**
2. **Crear cuenta** (primera vez) o **iniciar sesión**
3. **Seleccionar personaje** o **crear nuevo**
4. **Jugar** - El launcher abre el cliente automáticamente

---

## Estilos Visuales

El launcher usa un stylesheet QSS personalizado para su aspecto visual:

- Tema oscuro con acentos dorados
- Tipografías decorativas
- Efectos hover en botones
- Bordes redondeados

---

## Solución de Problemas

| Problema | Solución |
|----------|----------|
| "No se encuentra taller_launcher" | Verificá que compilaste con `-DTALLER_LAUNCHER=ON` |
| "Error de Qt6" | Instalá `qt6-base-dev` y recompilá |
| "No conecta al servidor" | Verificá que el servidor esté corriendo |
| "Nombre de usuario ya existe" | Elegí otro nombre de usuario |

---

<div style="margin-top: 30px;">
  <a href="editor.md">← Editor</a> | <a href="jugando.md">Siguiente: Cómo Jugar →</a>
</div>
