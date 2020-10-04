#include <stdio.h>
#include <memory>
#include "framework/window.h"
#include "msg.h"
#include "framework/draw.h"
//#include "framework/text.h"
#include <SDL2/SDL_ttf.h>

//=========================================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
std::shared_ptr<game::MessageBus> messageBus = nullptr;
std::shared_ptr<SDL_Window> window = nullptr;
//SDL_Renderer *gRenderer = nullptr;

bool init()
{
    messageBus = std::make_shared<game::MessageBus>();
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

    //gRenderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Init();

    return true;
}

void close()
{

    //Destroy window
    //SDL_DestroyRenderer(gRenderer);
    //gRenderer = nullptr;

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

    std::shared_ptr<framework::Draw> draw = std::make_shared<framework::Draw>(window.get());
    std::shared_ptr<game::Console> console = std::make_shared<game::Console>(messageBus, draw);
    std::shared_ptr<game::StartScreen> start = std::make_shared<game::StartScreen>(draw);
    if (console == nullptr || start == nullptr)
    {
        std::cout << "error";
    }

    bool quit = false;
    SDL_Event e;
    auto screenSurface = SDL_GetWindowSurface(window.get());

    while (!quit)
    {
        SDL_RenderClear(draw->renderer.get());
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //frameStart = SDL_GetTicks();

            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_c:
                    console->openConsole();
                }
            }
        }
        console->update();
        start->drawImage();
        messageBus->notify();
        draw->Update();
    }

    close();
    return 0;
}