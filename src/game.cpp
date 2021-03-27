#pragma once

#include "game.h"
#include <stdio.h>
#include <iostream>
#include <memory>
#include "OWL/window.h"

Game::Game(const std::shared_ptr<OWL::MessageBus> msgBus) : BusNode(msgBus)
{
}

bool Game::init()
{
    window = OWL::createWindow("game 23", OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT);
    draw = std::make_shared<OWL::Draw>(messageBus, window.get());
    console = std::make_shared<game::Console>(messageBus, draw, 0, OWL::SCREEN_HEIGHT - OWL::SCREEN_HEIGHT / 4, OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT / 4);
    start = std::make_shared<game::TestScreen>(messageBus, draw, 0, 0, OWL::SCREEN_WIDTH, OWL::SCREEN_HEIGHT);
    input = std::make_shared<OWL::Input>(messageBus);

    auto screenSurface = SDL_GetWindowSurface(window.get());

    return true;
}

void Game::run()
{
    if (!init())

    {
        std::cout << "Failed to initialize game!" << std::endl;
        return;
    }
    while (isRunning)
    {
        SDL_RenderClear(draw->renderer.get());
        SDL_RenderSetViewport(draw->renderer.get(), NULL);
        input->update();
        start->update();
        console->update();
        messageBus->notify();
        draw->update();
        SDL_Delay(40);
    }
}