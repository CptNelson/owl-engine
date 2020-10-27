#pragma once

#include "msg.h"
#include "screen.h"
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
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
    Screen::Screen(const std::shared_ptr<MessageBus> msgBus, const std::shared_ptr<Draw> draw, int x, int y, int w, int h,
                   std::string name, bool hasBorders)
        : BusNode(msgBus, name),
          draw{draw}, x{x}, y{y}, w{w}, h{h}, borders{hasBorders} {}

    ///Update is run every time the renderer updates the game window
    void Screen::update()
    {
        //If borders are enabled, create a viewport and draw borders
        // if a new viewport is not created here, the borders will appear around latest viewport that has been created
        if (borders)
        {
            draw->createViewport(texture.get(), x, y, w, h);
            draw->drawBox(x, y, w, h, foreground, borderWidth);
        }
    }

}; // namespace OWL
