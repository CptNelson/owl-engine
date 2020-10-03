#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <memory>
#include <string>
#include "utils.h"
#include "text.h"
#include <vector>

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

        std::shared_ptr<SDL_Renderer> renderer = nullptr;
        void Update();
        void CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface, int x, int y);
        void WriteText(std::string text, SDL_Color color, int x, int y);
        void CreateViewport(SDL_Texture *texture, int x, int y, int w, int h);
        void fillTexture(SDL_Texture *texture, int r, int g, int b, int a);
        //SDL_Surface *loadSurface(std::string path)

    private:
        std::shared_ptr<LTexture> textTexture = nullptr;
        // LTexture textTexture;
        int width;
        int height;
    };

    Draw::Draw(SDL_Window *window)
    {
        width = 0;
        height = 0;
        renderer = sdl_shared(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    };

    void Draw::CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface, int x, int y)
    {
        auto texture = sdl_shared(SDL_CreateTextureFromSurface(renderer, surface));

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
        textTexture->render(texture, x, y, width, height, renderer);
    };

    void Draw::WriteText(std::string text, SDL_Color color, int x, int y)
    {
        textTexture = nullptr;
        textTexture = std::make_shared<LTexture>();
        // SDL_SetRenderDrawColor(renderer.get(), 50, 200, 30, 0xFF);
        auto textSurface = textTexture->loadFromRenderedText(text, color, renderer.get());
        CreateTextureFromSurface(renderer.get(), &textSurface, x, y);
    };

    void Draw::Update()
    {
        //SDL_RenderClear(renderer.get());
        SDL_RenderPresent(renderer.get());
    }

    void Draw::CreateViewport(SDL_Texture *texture, int x, int y, int w, int h)
    {
        //Top left corner viewport
        SDL_Rect viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.w = w;
        viewport.h = h;
        SDL_RenderSetViewport(renderer.get(), &viewport);

        //Render texture to screen
        SDL_RenderCopy(renderer.get(), texture, NULL, NULL);
    }
    void Draw::fillTexture(SDL_Texture *texture, int r, int g, int b, int a)
    {
        SDL_SetRenderTarget(renderer.get(), texture);
        SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer.get(), r, g, b, a);
        SDL_RenderFillRect(renderer.get(), NULL);
    }
} // namespace framework

//===================================================================================================================================

namespace game
{
    class Console : public BusNode
    {
    public:
        Console(const std::shared_ptr<MessageBus> msgBus, const std::shared_ptr<framework::Draw> draw) : BusNode(msgBus)
        {
            // this->messageBus = msgBus;
            this->draw = draw;
            this->messageBus = msgBus;
            createConsole();
        }

        void createConsole()
        {
            Message greeting("Hi!");
            send(greeting);
            send(greeting);
            send(greeting);
        }

        void openConsole()
        {
            if (!isOpen)
            {
                isOpen = true;
                texture = framework::sdl_shared(SDL_CreateTexture(draw->renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200, 180));
                SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
                draw->fillTexture(texture.get(), 100, 100, 100, 100);
                SDL_SetRenderTarget(draw->renderer.get(), NULL);
                // SDL_SetRenderDrawBlendMode(draw->renderer.get(), SDL_BLENDMODE_BLEND);
                //       SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 255);
                draw->CreateViewport(texture.get(), 0, 540, 1280, 180);

                int y = 0;
                for (int i = 0; i < msgArray.size(); i++)
                {
                    std::cout << "hi2 :" << msgArray[i] << std::endl;
                    draw->WriteText(msgArray[i], {0xFF, 0xFF, 0xFF}, 0, y);
                    y += 20;
                }
            }
            else
            {
                SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 0);
                SDL_RenderClear(draw->renderer.get());
                isOpen = false;
            }
        }

    private:
        bool isOpen = false;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<MessageBus> messageBus = nullptr;
        std::shared_ptr<framework::Draw> draw = nullptr;
        // std::unique_ptr<std::vector<Message>> msgArray = std::make_unique<std::vector<Message>>();
        std::vector<std::string> msgArray;

        void onNotify(Message msg)
        {
            msgArray.push_back(msg.getMessage());
        }
    };
} // namespace game