
#pragma once

#include <stdio.h>
#include <iostream>
#include "msg.h"
#include "input.h"
#include "utils.h"
#include "globals.h"

namespace OWL
{
    //TODO: add different methods for different game modes

    /**
     * Screens are used to create defined views
     * inside the game window. The user can see multiple screens
     * at the same time. They store data of what is shown, 
     * and when they are visible, tell the renderer what to draw.
     */
    Input::Input(const std::shared_ptr<MessageBus> msgBus)
        : BusNode(msgBus, "Input") {}

    ///Update is run every time the renderer updates the game window
    void Input::update()
    {

        //std::cout << inputText << std::endl;
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //frameStart = SDL_GetTicks();

            if (e.type == SDL_QUIT)
            {
                send({"quitgame"});
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_c:
                    if (lctrl)
                        send({"inputconsole", "open"});
                    break;

                case SDLK_BACKSPACE:
                    send({"inputconsole", "backspace"});
                    break;
                case SDLK_RETURN:
                    send({"inputconsole", "enter"});
                    break;

                //scroll up and down messages
                case SDLK_UP:
                    send({"inputconsole", "moveup"});
                    break;
                case SDLK_DOWN:
                    send({"inputconsole", "movedown"});
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
                send({"inputconsole", "text", e.text.text});
            }
        }
    }
}; // namespace OWL
