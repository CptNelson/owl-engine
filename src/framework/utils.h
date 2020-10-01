#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

//==============================================================================

namespace framework
{
    class Utils
    {
    public:
        //======================================================================
        Utils();
        ~Utils();
        //======================================================================
        void loadImage(std::string path);
    };
}