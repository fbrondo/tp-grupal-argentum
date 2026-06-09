#pragma once

#include <SDL2pp/SDL2pp.hh>

// Interfaz mínima de ventana usada por el cliente
class Window {
public:
    virtual ~Window() = default;

    // Limpia la pantalla (preparar para dibujar)
    virtual void clear() = 0;

    // Presenta el frame dibujado
    virtual void present() = 0;
};
