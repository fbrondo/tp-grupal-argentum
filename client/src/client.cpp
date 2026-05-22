#include "client/includes/client.h"

#include <exception>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/SDLImage.hh>

using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::Window;
using SDL2pp::SDLImage;

Client::Client() = default;

void Client::run() {
    try {
        // Initialize SDL library
        SDL sdl(SDL_INIT_VIDEO);

        // Create main window: 640x480 dimensions, resizable, "SDL2pp demo" title
        Window window("Argentum Online", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
                      SDL_WINDOW_RESIZABLE);

        // Create accelerated video renderer with default driver
        Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Clear screen
        renderer.Clear();

        // Show rendered frame
        renderer.Present();

        // 5 second delay
        SDL_Delay(5000);

        // Here all resources are automatically released and library deinitialized

    } catch (std::exception& e) {
        // If case of error, print it and exit with error
        std::cerr << e.what() << std::endl;
    }
}

void Client::launch() {
    try {
        // 1. Núcleo base de SDL (Cargamos el subsistema de Video)
        SDL sdl(SDL_INIT_VIDEO);

        // 2. Extensiones (Inicializamos el soporte para PNG y JPG)

        SDLImage image_context(IMG_INIT_PNG | IMG_INIT_JPG);

        // 3. La Ventana (Ancho: 640, Alto: 480)
        Window window("Argentum Online",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              640, 480, SDL_WINDOW_RESIZABLE);

        // 4. El Renderer (Atado a la ventana instalada arriba y usando aceleración por hardware)
        Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

        // 5. Recursos de juego/app (A partir de acá cargás tus Texturas, Fuentes, etc.)
        // Texture mi_textura(renderer, "assets/heroe.png");

        std::cout << "¡Todo el entorno gráfico se levantó en el orden correcto!" << std::endl;

        // Tu bucle principal (Game Loop / Event Loop)...

    } catch (SDL2pp::Exception& e) {
        std::cerr << "Error crítico al inicializar los gráficos: " << e.what() << std::endl;
    }
}