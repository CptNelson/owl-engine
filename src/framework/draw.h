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
        std::shared_ptr<SDL_Texture> createTextureFromSurface(std::shared_ptr<SDL_Surface> surface);
        std::shared_ptr<SDL_Surface> loadFromFile(std::string path);
        void update();
        void reset();
        void render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        // wMod and hMod is used to make surfaces smaller, because text is too large
        std::shared_ptr<SDL_Texture> writeText(std::string text, SDL_Color color, int x, int y);
        void createViewport(SDL_Texture *texture, int x, int y, int w, int h);
        void fillTexture(SDL_Texture *texture, int r, int g, int b, int a);
        void drawImageFromFile(std::shared_ptr<SDL_Surface> imageSurface, int x, int y);
        void drawBox(int x, int y, int w, int h, int r, int g, int b, int a, int thickness);

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

    std::shared_ptr<SDL_Texture> Draw::createTextureFromSurface(std::shared_ptr<SDL_Surface> surface)
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
    }

    std::shared_ptr<SDL_Texture> Draw::writeText(std::string text, SDL_Color color, int x, int y)
    {
        auto textSurface = textureObject->loadFromRenderedText(text, color);
        auto textTexture = createTextureFromSurface(textSurface);
        return textTexture;
        //render(textTexture, x, y, width, height);
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

    void Draw::drawBox(int x, int y, int w, int h, int r, int g, int b, int a, int t)
    {

        SDL_Point points[5] = {
            {x, y},
            {w / t - 1, y},
            {w / t - 1, h / t - 1},
            {x, h / t - 1},
            {x, y}};
        SDL_SetRenderDrawColor(renderer.get(), r, g, b, a);
        SDL_RenderSetScale(renderer.get(), t, t);
        SDL_RenderDrawLines(renderer.get(), points, 5);
        reset();
    }
    void Draw::reset()
    {
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
        SDL_RenderSetScale(renderer.get(), 1, 1);
    }
} // namespace framework

//===================================================================================================================================

namespace game
{

    class Screen
    {
    public:
        Screen(const std::shared_ptr<framework::Draw> draw, int x, int y, int w, int h, bool hasBorders = true)
        {
            this->draw = draw;
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
            borders = hasBorders;

            texture = framework::sdl_shared(SDL_CreateTexture(draw->renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200, 180));
            SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
            draw->fillTexture(texture.get(), 0, 0, 0, 0);
            SDL_SetRenderTarget(draw->renderer.get(), NULL);
            SDL_SetRenderDrawBlendMode(draw->renderer.get(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(draw->renderer.get(), 0, 0, 0, 0);
            draw->createViewport(texture.get(), x, y, w, h);
        };
        ~Screen(){};

        virtual void update()
        {
            if (borders)
            {
                draw->createViewport(texture.get(), x, y, w, h);
                draw->drawBox(x, y, w, h, 255, 175, 46, 255, 2);
            }
        }

    protected:
        std::shared_ptr<framework::Draw> draw = nullptr;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<SDL_Surface> surface = nullptr;
        bool borders = true;
        int x, y, w, h;
    };

    class StartScreen : public Screen
    {
    public:
        StartScreen(const std::shared_ptr<framework::Draw> draw, int x, int y, int w, int h) : Screen(draw, x, y, w, h)
        {
            //this->draw = draw;
            surface = draw->loadFromFile("framework/pixl.png");
            if (surface == nullptr)
            {
                std::cout << "error" << std::endl;
            }
        };
        ~StartScreen(){};

        void update()
        {
            int w, h;
            //draw->drawImageFromFile(surface, 1280 / 4, 720 / 2);
            auto imageTexture = draw->createTextureFromSurface(surface);
            SDL_QueryTexture(imageTexture.get(), NULL, NULL, &w, &h);
            draw->createViewport(imageTexture.get(), 1280 - w / 4, 720 - h / 4, w / 4, h / 4);

            auto text = draw->writeText("THE OWL ENGINE", {255, 175, 46}, 1280 / 2, 100);
            SDL_QueryTexture(text.get(), NULL, NULL, &w, &h);
            draw->createViewport(text.get(), 1280 / 2 - w / 4, 100, w / 2, h / 2);
            draw->render(text, 0, 0, w / 2, h / 2);
            Screen::update();
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
                                                                                                                                     Screen(draw, x, y, w, h)
        {
            // this->messageBus = msgBus;
            this->draw = draw;
            this->messageBus = msgBus;
            createConsole();
        }
        bool isOpen = false;

        void createConsole()
        {
            Message greeting("===The OWL Engine Console===");
            send(greeting);
            send(greeting);
            send(greeting);
            send(greeting);
            send(greeting);
            send(greeting);
        }

        void openConsole(bool &inputTextEnabled)
        {
            if (!isOpen)
            {
                isOpen = true;
                inputTextEnabled = true;
                SDL_StartTextInput();
            }
            else
            {
                isOpen = false;
                inputTextEnabled = false;
                SDL_StopTextInput();
            }
        }

        void writeToConsole(char *t)
        {
            inputText += t;
        }
        void backSpace()
        {
            if (inputText.length() > 0)
            {
                inputText.pop_back();
            }
        }
        void enter()
        {
            if (inputText.length() > 0)
            {
                inputText = "";
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

                // Console input #
                auto hashtag = draw->writeText("# ", {255, 175, 46}, 5, 30);
                int w, h;
                SDL_QueryTexture(hashtag.get(), NULL, NULL, &w, &h);
                draw->render(hashtag, 5, 180 - h / 6, w / 6, h / 6);

                int y = 0;
                for (int i = 0; i < msgArray.size(); i++)
                {
                    auto text = draw->writeText(msgArray[i], {255, 175, 46}, 0, y);
                    SDL_QueryTexture(text.get(), NULL, NULL, &w, &h);
                    draw->render(text, 5, y, w / 6, h / 6);
                    y += 20;
                }
                if (inputText != "")
                {
                    //Render new text
                    auto text = draw->writeText(inputText, {255, 175, 46}, 5, 30);
                    int w, h;
                    SDL_QueryTexture(text.get(), NULL, NULL, &w, &h);
                    draw->render(text, 24, 180 - h / 6, w / 6, h / 6);
                }
            }
        }

    private:
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::shared_ptr<MessageBus> messageBus = nullptr;
        std::vector<std::string> msgArray;
        std::string inputText = "";

        void onNotify(Message msg)
        {
            msgArray.push_back(msg.getMessage());
        }
    };
} // namespace game

// ===GUI Elements====================================================================================================================

class Button
{
private:
    //The attributes of the button
    SDL_Rect box;

    //The part of the button sprite sheet that will be shown
    // SDL_Rect *clip;

public:
    //Initialize the variables
    Button(int x, int y, int w, int h);

    //Handles events and set the button's sprite region
    void handle_events();

    //Shows the button on the screen
    void show();
};