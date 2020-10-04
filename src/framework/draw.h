#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
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
        std::shared_ptr<SDL_Texture> createTextureFromSurface(std::shared_ptr<SDL_Surface> surface, int x, int y);
        std::shared_ptr<SDL_Surface> loadFromFile(std::string path);
        void update();
        void render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        // wMod and hMod is used to make surfaces smaller, because text is too large
        void writeText(std::string text, SDL_Color color, int x, int y);
        void createViewport(SDL_Texture *texture, int x, int y, int w, int h);
        void fillTexture(SDL_Texture *texture, int r, int g, int b, int a);
        void drawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y);

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

    void Draw::render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
    {
        //Set rendering space and render to screen
        SDL_Rect renderQuad = {x, y, width, height};

        //Set clip rendering dimensions
        if (clip != NULL)
        {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        //Render to screen
        SDL_RenderCopyEx(renderer.get(), texture.get(), clip, &renderQuad, angle, center, flip);
    }

    std::shared_ptr<SDL_Texture> Draw::createTextureFromSurface(std::shared_ptr<SDL_Surface> surface, int x, int y)
    {
        auto texture = sdl_shared(SDL_CreateTextureFromSurface(renderer.get(), surface.get()));

        if (texture == nullptr)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        return texture;
        // textureObject->render(texture, x, y, width / wMod, height / hMod, renderer.get());
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

    void Draw::drawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y)
    {
        auto imageTexture = createTextureFromSurface(imageSurface, x, y);

        createViewport(imageTexture.get(), 1280/2-1280/4, 720/2, 1280/2, 720/2);
    }

    void Draw::writeText(std::string text, SDL_Color color, int x, int y)
    {
        auto textSurface = textureObject->loadFromRenderedText(text, color);
        //Get image dimensions
        int width = textSurface->w / 6;
        int height = textSurface->h / 6;
        auto textTexture = createTextureFromSurface(textSurface, x, y);
        render(textTexture, x, y, width, height);
    };

    void Draw::update()
    {
        SDL_RenderPresent(renderer.get());
    }

    void Draw::createViewport(SDL_Texture *texture, int x, int y, int w, int h)
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

    class Screen
    {
    public:
        Screen(const std::shared_ptr<framework::Draw> draw, int x,int y,int w, int h, bool hasBorders = false)
        {
            this->draw = draw;
            borders = hasBorders;
        };
        ~Screen(){};

        virtual void update()
        {
            if (borders)
            {
                SDL_Point points[5] = {
                                    {0, 0},
                                    {1279, 0},
                                    {1279, 719},
                                    {0, 719},
                                    {0, 0}
                                };

                SDL_SetRenderDrawColor(draw->renderer.get(), 255, 0, 0, SDL_ALPHA_OPAQUE);
                //SDL_RenderSetScale(draw->renderer.get(), 2, 2 );
                SDL_RenderDrawLines(draw->renderer.get(), points, 5);

            }

        }
    protected:
        std::shared_ptr<framework::Draw> draw = nullptr;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<SDL_Surface> surface = nullptr;
        bool borders = false;
    };

    class StartScreen : public Screen
    {
    public:
            StartScreen(const std::shared_ptr<framework::Draw> draw, int x, int y, int w, int h) : Screen(draw,x,y,w,h)
        {
            this->draw = draw;
            surface = draw->loadFromFile("framework/pixl.png");
            if (surface == nullptr)
            {
                std::cout << "error" << std::endl;
            }
        };
        ~StartScreen(){};

        void drawImage()
        {
            //draw->createViewport(image.get(), 0, 0, 1280, 720);
            //draw->writeText("THE OWL ENGINE", {255, 175, 46}, 1280/2, 100);
            draw->drawImageFromFile(surface, 0, 0);
        }

    private:
        //std::shared_ptr<SDL_Surface> image = nullptr;
    };
} // namespace game

//===================================================================================================================================

namespace game
{
    class Console : public BusNode, public Screen
    {
    public:
        Console(const std::shared_ptr<MessageBus> msgBus, const std::shared_ptr<framework::Draw> draw, int x, int y, int w, int h) : BusNode(msgBus),
                             Screen(draw,x,y,w,h)
        {
            // this->messageBus = msgBus;
            this->draw = draw;
            this->messageBus = msgBus;
            createConsole();
        }

        void createConsole()
        {
            Message greeting("The OWL Engine Console");
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
                draw->fillTexture(texture.get(), 100, 100, 100, 180);
                SDL_SetRenderTarget(draw->renderer.get(), NULL);
                SDL_SetRenderDrawBlendMode(draw->renderer.get(), SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 255);
                draw->createViewport(texture.get(), 0, 540, 1280, 180);

                int y = 0;
                for (int i = 0; i < msgArray.size(); i++)
                {
                    draw->writeText(msgArray[i], {255, 175, 46}, 0, y);
                    y += 20;
                }
            }
            else
            {
                SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 0);
                //SDL_RenderClear(draw->renderer.get());
            }
        }

    private:
        bool isOpen = false;
        std::shared_ptr<LTexture> textTexture = nullptr; // = std::make_shared<LTexture>();
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<MessageBus> messageBus = nullptr;
        //std::shared_ptr<framework::Draw> draw = nullptr;
        std::vector<std::string> msgArray;

        void onNotify(Message msg)
        {
            msgArray.push_back(msg.getMessage());
        }
    };
} // namespace game