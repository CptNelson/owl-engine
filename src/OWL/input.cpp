
#pragma once

#include <stdio.h>
#include <iostream>
#include "msg.h"
#include "input.h"
#include "utils.h"
#include "globals.h"

namespace OWL
{
    /**
     * Screens are used to create defined views
     * inside the game window. The user can see multiple screens
     * at the same time. They store data of what is shown, 
     * and when they are visible, tell the renderer what to draw.
     */
    Input::Input(const std::shared_ptr<MessageBus> msgBus)
        : BusNode(msgBus, "Input") {}
    SDL_Event e;
    int lctrl = 0, rctrl = 0;
    bool inputText = false;

    ///Update is run every time the renderer updates the game window
    void Input::update()
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //frameStart = SDL_GetTicks();

            if (e.type == SDL_QUIT)
            {
                send("quit_game");
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_c:
                    if (lctrl)
                        send("inputconsole", {"open"});
                    // console->openConsole(inputText);
                    break;

                case SDLK_BACKSPACE:
                    send("inputconsole_backspace");
                    //console->backSpace();
                    break;
                case SDLK_RETURN:
                    send("inputconsole_enter");
                    //  console->enter();
                    break;

                //scroll up and down messages
                case SDLK_UP:
                    send("inputconsole_moveup");
                    // console->moveUp();
                    break;
                case SDLK_DOWN:
                    send("inputconsole_movedown");
                    //console->moveDown();
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
                std::string text = "inputconsole";
                std::vector<std::string> params{e.text.text};
                send(text, params);
                // console->writeToConsole(e.text.text);
            }
        }
    }

}; // namespace OWL
