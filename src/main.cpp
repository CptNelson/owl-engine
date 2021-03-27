#pragma once

#include <stdio.h>
#include <iostream>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "OWL/globals.h"
#include "OWL/msg.h"

//=========================================================

std::shared_ptr<Game> gameInstance = nullptr;
std::shared_ptr<OWL::MessageBus> messageBus = nullptr;

bool init()
{
    messageBus = std::make_shared<OWL::MessageBus>();
    if (messageBus == nullptr)
    {
        std::cout << "messageBus creation failed!" << std::endl;
        return false;
    }
    gameInstance = std::make_shared<Game>(messageBus);

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

    gameInstance->run();

    close();
    return 0;
}