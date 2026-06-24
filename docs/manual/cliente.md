---
layout: default
title: Cliente
---

# Ejecutar el Cliente

El cliente es la aplicación gráfica que te permite jugar Argentum Online. Muestra el mundo del juego, la interfaz, y envía tus acciones al servidor.

---

## Requisitos

- El proyecto compilado (ver [Compilación](compilacion.md))
- **El servidor debe estar corriendo** (ver [Servidor](servidor.md))
- Pantalla con mínimo 960x540 de resolución

---

## Ejecución

### Modo Desarrollo (desde build/)

```bash
./build/taller_client localhost 8080
```

### Modo Instalado

```bash
taller_client localhost 8080
```

### Parámetros

| Parámetro | Descripción | Ejemplo |
|-----------|-------------|---------|
| `<host>` | Dirección IP o hostname del servidor | `localhost`, `192.168.1.100` |
| `<puerto>` | Puerto del servidor | `8080` |

---

## Pantalla de Login

Al iniciar el cliente, verás la pantalla de login:

![Pantalla de Login](assets/screenshots/login.png){: .screenshot }

### Crear una Cuenta

1. Hacé click en **"Registrarse"** o presioná la tecla correspondiente
2. Completá los campos:
   - **Usuario**: Nombre de usuario único
   - **Contraseña**: Contraseña segura
3. Hacé click en **"Crear Cuenta"**

### Iniciar Sesión

1. Ingresá tu **usuario** y **contraseña**
2. Hacé click en **"Iniciar Sesión"** o presioná Enter

### Seleccionar Personaje

Si es tu primera vez, crearás un nuevo personaje. Si ya tenés uno, seleccionalo de la lista.

---

## Crear un Personaje

Al crear un personaje, elegís:

### Raza

| Raza | Bonificación | Descripción |
|------|--------------|-------------|
| **Humano** | Equilibrado | Stats balanceados, buena adaptabilidad |
| **Elfo** | Mana +20% | Excelentes magos, recuperación rápida |
| **Enano** | Vida +30% | Tanques naturales, resistencia física |
| **Gnomo** | Mana +30%, Vida -10% | Magos puros, alto daño mágico |

### Clase

| Clase | Bonificación | Estilo de Juego |
|-------|--------------|-----------------|
| **Mago** | Daño mágico +50%, Vida -30% | Ataque a distancia, alto daño, frágil |
| **Clérigo** | Curación +20%, Vida +10% | Soporte, curación, equilibrado |
| **Paladín** | Vida +20%, Daño físico +10% | Tanque ofensivo, buena defensa |
| **Guerrero** | Daño físico +50%, Mana -30% | Tanque puro, máximo daño cuerpo a cuerpo |

### Nombre

Escribí un nombre único para tu personaje (máximo 20 caracteres).

---

## Interfaz del Juego

Una vez en el mundo, verás la siguiente interfaz:

![Interfaz del Juego](assets/screenshots/gameplay.png){: .screenshot }

### Zona Superior

- **Barra de Vida**: Muestra tu salud actual
- **Barra de Mana**: Muestra tu energía mágica
- **Nivel y Experiencia**: Tu nivel actual y progreso

### Zona Inferior - Inventario

![Inventario](assets/screenshots/inventario.png){: .screenshot }

- **Slots de Items**: 20 espacios para equipamiento y objetos
- **Equipamiento**: Zona para armas, armaduras, cascos, escudos

### Consola de Chat

![Chat](assets/screenshots/chat.png){: .screenshot }

- Mensajes del sistema
- Chat con otros jugadores
- Comandos especiales

---

## Controles

| Tecla / Acción | Función |
|----------------|---------|
| **Flechas** o **WASD** | Mover al personaje |
| **Click Izquierdo** | Atacar / Seleccionar |
| **Click Derecho** | Interactuar con NPC |
| **I** | Abrir/Cerrar inventario |
| **Enter** | Abrir chat |
| **Escape** | Menú / Cerrar ventana |

Para más detalles, ver [Cómo Jugar](jugando.md).

---

## Conexión con el Servidor

### Conexión Local

Si el servidor está en tu misma computadora:

```bash
taller_client localhost 8080
```

### Conexión Remota

Si el servidor está en otra máquina:

```bash
taller_client 192.168.1.100 8080
```

### Verificar Conexión

Si el cliente no se conecta:

1. Verificá que el servidor esté corriendo
2. Verificá que la IP y puerto sean correctos
3. Verificá que no haya un firewall bloqueando
4. Consultá [Solución de Problemas](troubleshoot.md)

---

## Audio

El cliente reproduce efectos de sonido y música de fondo. Para controlar el volumen:

- **Subir volumen**: Flecha arriba
- **Bajar volumen**: Flecha abajo
- **Silenciar**: M

---

## Salir del Juego

Para salir del cliente:

1. Presioná **Escape**
2. Seleccioná **"Salir"**
3. O simplemente cerrá la ventana

---

<div style="margin-top: 30px;">
  <a href="servidor.md">← Servidor</a> | <a href="editor.md">Siguiente: Editor →</a>
</div>
