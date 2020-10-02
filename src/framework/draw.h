#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <memory>
#include <string>
#include "utils.h"
#include "text.h"

//==============================================================================
namespace framework
{
    class Draw
    {
    public:
        //==============================================================================
        Draw(SDL_Window *window);
        ~Draw(){};
        //==============================================================================
        void Update();
        void CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface);
        void WriteText(std::string text, SDL_Color color);
        //SDL_Surface *loadSurface(std::string path)

    private:
        std::shared_ptr<SDL_Renderer> renderer = nullptr;
        // std::shared_ptr<SDL_Texture> texture = nullptr;
        LTexture textTexture;
        int width;
        int height;
    };

    Draw::Draw(SDL_Window *window)
    {
        width = 0;
        height = 0;
        renderer = sdl_shared(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    };

    void Draw::CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface)
    {
        auto texture = sdl_shared(SDL_CreateTextureFromSurface(renderer, surface));

        //Create texture from surface pixels
        //SDL_CreateTextureFromSurface(renderer.get(), texture);
        if (texture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            width = surface->w;
            height = surface->h;
        }
        textTexture.render(texture, (1280 - textTexture.getWidth()) / 2, (720 - textTexture.getHeight()) / 2, width, height, renderer);

        //Get rid of old surface
        //SDL_FreeSurface(surface);
    };

    void Draw::WriteText(std::string text, SDL_Color color)
    {
        SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer.get());
        auto textSurface = textTexture.loadFromRenderedText(text, color, renderer.get());
        CreateTextureFromSurface(renderer.get(), &textSurface);
        //std::cout << width << std::endl;
    };

    void Draw::Update()
    {
        //Clear screen
        //std::cout << textTexture.getWidth() << std::endl;
        SDL_RenderPresent(renderer.get());
    }
} // namespace framework