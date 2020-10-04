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
        // wMod and hMod is used to make surfaces smaller, because text is too large
        void CreateTextureFromSurface(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<SDL_Surface> surface, int x, int y, int wMod = 1, int hMod = 1);
        void WriteText(std::string text, SDL_Color color, int x, int y);
        void CreateViewport(SDL_Texture *texture, int x, int y, int w, int h);
        void fillTexture(SDL_Texture *texture, int r, int g, int b, int a);
        void DrawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y);
        std::shared_ptr<SDL_Surface> loadFromFile(std::string path);

    private:
        std::shared_ptr<LTexture> textureObject = std::make_shared<LTexture>();
        int width;
        int height;
    };

    Draw::Draw(SDL_Window *window)
    {
        width = 0;
        height = 0;
        renderer = sdl_shared(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        int imgFlags = IMG_INIT_PNG;
    };

    void Draw::CreateTextureFromSurface(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<SDL_Surface> surface, int x, int y, int wMod, int hMod)
    {
        auto texture = sdl_shared(SDL_CreateTextureFromSurface(renderer.get(), surface.get()));

        if (texture == nullptr)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            width = surface->w;
            height = surface->h;
        }
        textureObject->render(texture, x, y, width / wMod, height / hMod, renderer.get());
    };

    //TODO: move to utils!
    std::shared_ptr<SDL_Surface> Draw::loadFromFile(std::string path)
    {
        std::shared_ptr<SDL_Surface> loadedSurface = sdl_shared(IMG_Load(path.c_str()));
        if (loadedSurface == nullptr)
        {
            printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        }
        //Color key image
        SDL_SetColorKey(loadedSurface.get(), SDL_TRUE, SDL_MapRGB(loadedSurface.get()->format, 0, 0xFF, 0xFF));
        return loadedSurface;
    }

    void Draw::DrawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y)
    {
        CreateTextureFromSurface(renderer, imageSurface, x, y);
    }

    void Draw::WriteText(std::string text, SDL_Color color, int x, int y)
    {
        auto textSurface = textureObject->loadFromRenderedText(text, color);
        CreateTextureFromSurface(renderer, textSurface, x, y, 8, 8);
    };

    void Draw::Update()
    {
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
    class StartScreen
    {
    public:
        StartScreen(const std::shared_ptr<framework::Draw> draw)
        {
            this->draw = draw;
            image = draw->loadFromFile("framework/img.png");
            if (image == nullptr)
            {
                std::cout << "errror" << std::endl;
            }
        };
        ~StartScreen(){};

        void drawImage()
        {
            //draw->CreateViewport(image.get(), 0, 0, 1280, 720);
            draw->DrawImageFromFile(image, 0, 0);
        }

    private:
        std::shared_ptr<framework::Draw> draw = nullptr;
        std::shared_ptr<SDL_Surface> image = nullptr;
    };
} // namespace game

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
        }

        void openConsole()
        {
            if (!isOpen)
            {
                isOpen = true;
            }
            else
            {
                isOpen = false;
            }
        }

        void update()
        {
            if (isOpen)
            {
                texture = framework::sdl_shared(SDL_CreateTexture(draw->renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200, 180));
                SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
                draw->fillTexture(texture.get(), 100, 100, 100, 100);
                SDL_SetRenderTarget(draw->renderer.get(), NULL);
                SDL_SetRenderDrawBlendMode(draw->renderer.get(), SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 255);
                draw->CreateViewport(texture.get(), 0, 540, 1280, 180);

                int y = 0;
                for (int i = 0; i < msgArray.size(); i++)
                {
                    draw->WriteText(msgArray[i], {0xFF, 0xFF, 0xFF}, 0, y);
                    y += 20;
                }
            }
            else
            {
                SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 0);
                SDL_RenderClear(draw->renderer.get());
            }
        }

    private:
        bool isOpen = false;
        std::shared_ptr<LTexture> textTexture = nullptr; // = std::make_shared<LTexture>();
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<MessageBus> messageBus = nullptr;
        std::shared_ptr<framework::Draw> draw = nullptr;
        std::vector<std::string> msgArray;

        void onNotify(Message msg)
        {
            msgArray.push_back(msg.getMessage());
        }
    };
} // namespace game