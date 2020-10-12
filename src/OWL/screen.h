#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <memory>
#include "draw.h"
#include "msg.h"

namespace OWL
{
    /**
     * @brief Base class for Screens. Inherits BusNode.
     * @param msgBus reference to MessageBus object
     * @param draw reference to the Draw object
     * @param x,y the coordinates of top left corner
     * @param w,h the width and height of Screen
     * @param hasBorders bool for Screen border lines
     */
    class Screen : public BusNode
    {
    public:
        //==================================================================================================================================
        Screen(const std::shared_ptr<MessageBus> msgBus, const std::shared_ptr<Draw> draw, int x, int y, int w, int h, bool hasBorders = true);
        //: BusNode(msgBus) {}
        //==================================================================================================================================

        ///Update is run every time the renderer updates the game window
        virtual void update(); 

    protected:
        std::shared_ptr<Draw> draw{nullptr};
        std::shared_ptr<SDL_Texture> texture{nullptr};
        SDL_Color foreground{255, 175, 46, 255};
        SDL_Color background{0, 0, 0, 255};
        bool borders;
        int borderWidth{2};
        int x, y, w, h;
    };
} // namespace OWL