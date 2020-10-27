#pragma once

#include <stdio.h>
#include <iostream>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "OWL/globals.h"
#include "OWL/window.h"
#include "OWL/msg.h"
#include "OWL/draw.h"
#include "OWL/screen.h"
#include "screens.h"
#include "OWL/input.h"

//=========================================================

std::shared_ptr<OWL::MessageBus> messageBus = nullptr;
std::shared_ptr<SDL_Window> window = nullptr;

bool init()
{
    messageBus = std::make_shared<OWL::MessageBus>();
    if (messageBus == nullptr)
    {
        std::cout << "messageBus creation failed!" << std::endl;
        return false;
    }

    window = OWL::createWindow("game 23", OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT);
    if (window == nullptr)
    {
        std::cout << "Window creation failed!" << std::endl;
        return false;
    }

    TTF_Init();

    return true;
}

void close()
{
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

    std::shared_ptr<OWL::Draw> draw = std::make_shared<OWL::Draw>(messageBus, window.get());
    std::shared_ptr<game::Console> console = std::make_shared<game::Console>(messageBus, draw, 0, OWL::SCREEN_HEIGHT - OWL::SCREEN_HEIGHT / 4, OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT / 4);
    std::shared_ptr<game::StartScreen> start = std::make_shared<game::StartScreen>(messageBus, draw, 0, 0, OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT);
    std::shared_ptr<OWL::Input> input = std::make_shared<OWL::Input>(messageBus);

    if (console == nullptr || start == nullptr)
    {
        std::cout << "error";
    }

    bool quit = false;
    auto screenSurface = SDL_GetWindowSurface(window.get());
    int q = -200;

    while (!quit)
    {
        SDL_RenderClear(draw->renderer.get());
        SDL_RenderSetViewport(draw->renderer.get(), NULL);
        input->update();
        start->update();
        console->update();
        messageBus->notify();
        draw->update();
        SDL_Delay(40);
        q += 1;
        if (q >= 0)
            quit = true;
    }

    close();
    return 0;
}