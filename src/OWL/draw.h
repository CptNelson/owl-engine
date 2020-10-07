#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include "utils.h"

//==============================================================================
namespace OWL
{
    /**
     * @brief class for all draw&render functions
     * @details
     * @param window reference to SDL_Window instace
     */
    class Draw
    {
    public:
        //==============================================================================
        Draw(SDL_Window *window);
        ~Draw(){};
        //==============================================================================

        void update();
        void reset();
        std::shared_ptr<SDL_Renderer> renderer = nullptr;
        std::shared_ptr<SDL_Texture> createTextureFromSurface(std::shared_ptr<SDL_Surface> surface);
        std::shared_ptr<SDL_Surface> loadFromFile(std::string path);
        std::shared_ptr<SDL_Texture> writeText(std::string text, SDL_Color color, int x, int y);
        std::shared_ptr<SDL_Surface> loadFromRenderedText(std::string textureText, SDL_Color textColor);
        void render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void createViewport(SDL_Texture *texture, int x, int y, int w, int h);
        void fillTexture(SDL_Texture *texture, int r, int g, int b, int a);
        void drawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y);
        void drawBox(int x, int y, int w, int h, SDL_Color c, int thickness);
        void createEmptyTexture(std::shared_ptr<SDL_Texture> texture, SDL_Color &c, int x, int y, int w, int h);

    private:
        TTF_Font *font = nullptr;
        int width;
        int height;
    };

} // namespace OWL

//===================================================================================================================================
