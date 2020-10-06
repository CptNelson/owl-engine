#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "utils.h"
#include <stdio.h>
#include <memory>
#include <string>

namespace OWL
{

    std::shared_ptr<SDL_Window> createWindow(const char *name, int width, int height)
    {
        std::shared_ptr<SDL_Window> window;
        //Initialization flag
        bool success = true;

        //Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            window = sdl_shared(SDL_CreateWindow(name,
                                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                 width, height, SDL_WINDOW_SHOWN));

            if (window == nullptr)
            {
                printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
        }
        return window;
    }

} // namespace framework
