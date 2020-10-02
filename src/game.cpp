#include <stdio.h>
#include <memory>
#include "framework/window.h"
#include "msg.h"
#include "framework/texture.h"
#include <SDL2/SDL_ttf.h>

//=========================================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
std::unique_ptr<game::MessageBus> messageBus = nullptr;
std::shared_ptr<SDL_Window> window = nullptr;
SDL_Renderer *gRenderer = nullptr;

bool init()
{
    messageBus = std::make_unique<game::MessageBus>();
    if (messageBus == nullptr)
    {
        std::cout << "messageBus creation failed!" << std::endl;
        return false;
    }

    window = framework::createWindow("game 23", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (window == nullptr)
    {
        std::cout << "Window creation failed!" << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Init();

    return true;
}

void close()
{

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    if (!init())
    {
        std::cout << "Failed to initialize!" << std::endl;
        return -1;
    }

    bool quit = false;
    SDL_Event e;
    auto screenSurface = SDL_GetWindowSurface(window.get());

    while (!quit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        SDL_Color textColor = {0, 0, 0};

        //Fill the surface white
        //SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

        LTexture gTextTexture;
        gTextTexture.loadFromRenderedText("Testing Testing", textColor, gRenderer);

        //Update the surface
        //SDL_UpdateWindowSurface(window.get());

        //Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        //Render current frame
        gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2, gRenderer);

        //Update screen
        SDL_RenderPresent(gRenderer);
    }

    close();
    return 0;
}