
#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <memory>
#include "msg.h"

namespace OWL
{
    /**
     * @brief Base class for Input. Inherits BusNode.
     * @param msgBus reference to MessageBus object
     */
    class Input : public BusNode
    {
    public:
        //==================================================================================================================================
        Input(const std::shared_ptr<MessageBus> msgBus);
        //: BusNode(msgBus) {}
        //==================================================================================================================================

        ///Update is run every time the renderer updates the game window
        virtual void update();

    protected:
        SDL_Event e;
        int lctrl{0}, rctrl{0};
        bool inputText{false};

        void onNotify(OWL::Message msg)
        {
            if (msg.getParameter(0) == "inputTextEnable")
                inputText = true;
            if (msg.getParameter(0) == "inputTextDisable")
                inputText = false;
        }
    };
} // namespace OWL