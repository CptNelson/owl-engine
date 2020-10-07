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

    std::shared_ptr<OWL::Draw> draw = std::make_shared<OWL::Draw>(window.get());
    std::shared_ptr<game::Console> console = std::make_shared<game::Console>(messageBus, draw, 0, 540, OWL::SCREEN_WIDTH, 180);
    std::shared_ptr<game::StartScreen> start = std::make_shared<game::StartScreen>(messageBus, draw, 0, 0, OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT);
    if (console == nullptr || start == nullptr)
    {
        std::cout << "error";
    }

    bool quit = false;
    SDL_Event e;
    auto screenSurface = SDL_GetWindowSurface(window.get());

    int lctrl = 0, rctrl = 0;
    bool inputText = false;

    while (!quit)
    {
        SDL_RenderClear(draw->renderer.get());
        SDL_RenderSetViewport(draw->renderer.get(), NULL);
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
                    if (lctrl)
                        console->openConsole(inputText);
                    break;

                case SDLK_BACKSPACE:
                    console->backSpace();
                    break;
                case SDLK_RETURN:
                    console->enter();
                    break;

                //scroll up and down messages
                case SDLK_UP:
                    console->moveUp();
                    break;
                case SDLK_DOWN:
                    console->moveDown();
                    break;

                case SDLK_RCTRL:
                    rctrl = 1;
                    break;
                case SDLK_LCTRL:
                    lctrl = 1;
                    break;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_RCTRL:
                    rctrl = 0;
                    break;
                case SDLK_LCTRL:
                    lctrl = 0;
                    break;
                }
            }

            else if (e.type == SDL_TEXTINPUT && inputText)
            {
                console->writeToConsole(e.text.text);
            }
        }
        start->update();
        //console->update();
        //messageBus->notify();
        //draw->update();
        SDL_Delay(40);
    }

    close();
    return 0;
}