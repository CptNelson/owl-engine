
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
    };
} // namespace OWL