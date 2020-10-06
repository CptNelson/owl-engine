#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include "draw.h"
#include "utils.h"

namespace OWL
{
    /**
     * @brief Base class for Screens. Inherits BusNode.
     *
     * @details Screens are used to create defined views
     * inside the game window. The user can see multiple screens
     * at the same time. They store data of what is shown, 
     * and when they are visible, tell the renderer what to draw.
     * 
     * @param msgBus reference to MessageBus object
     * @param draw reference to the Draw object
     * @param x,y the coordinates of top left corner
     * @param w,h the width and height of Screen
     * @param hasBorders bool for Screen border lines
     */
    class Screen : public BusNode
    {
    public:
        Screen(const std::shared_ptr<MessageBus> msgBus, const std::shared_ptr<Draw> draw, int x, int y, int w, int h, bool hasBorders = false)
            : BusNode(msgBus),
              draw{draw}, x{x}, y{y}, w{w}, h{h}, borders{hasBorders} {}

        ///Update is run every time the renderer updates the game window
        virtual void update()
        {
            //If borders are enabled, create a viewport and draw borders
            // if a new viewport is not created here, the borders will appear around latest viewport that has been created
            if (borders)
            {
                draw->createViewport(texture.get(), x, y, w, h);
                draw->drawBox(x, y, w, h, 255, 175, 46, 255, 2);
            }
        }

    protected:
        std::shared_ptr<Draw> draw = nullptr;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        bool borders = true;
        int x, y, w, h;
    };

} // namespace OWL

//===================================================================================================================================

namespace game
{
    /**
     * @brief In-game console. Inherits Screen and BusNode classes. 
     *
     * @details Console can be opened and closed with keyboard command.
     * Because it inherits BusNode, it can see all the Messages
     * that are send inside the engine. It will show those
     * messages with timestamp made of game tick/10
     * 
     * It takes text input, and can run written commands.
     *
     * @param  msgBus reference to the MessageBus object
     * @param  draw reference to the Draw object
     * @param x,y the coordinates of top left corner
     * @param w,h the width and height of Screen
     * 
     */
    class Console : public OWL::Screen
    {
    public:
        Console(const std::shared_ptr<OWL::MessageBus> msgBus, const std::shared_ptr<OWL::Draw> draw, int x, int y, int w, int h)
            : Screen(msgBus, draw, x, y, w, h) {}

        bool isOpen = false;

        /**
         * @brief Open and close the console.
         *
         * @param inputTextEnabled Reference to bool to change if inputting is enabled or not.
         */
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

        //===Basic console typing & manipulation functions===

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
                OWL::Message msg(inputText);
                send(msg);
                inputText = "";
            }
        }
        void moveUp()
        {
            // TODO: Doesn't work right
            scroll -= 1;
            if (scroll < 0)
                scroll = 0;
        }
        void moveDown()
        {
            scroll += 1;
            if (scroll > msgArray.size() - 2)
                scroll = msgArray.size() - 2;
        }
        //=====================================================

        /**
         * @brief Draw the console and write messages to it.
         */
        void update()
        {
            if (isOpen)
            {
                draw->createEmptyTexture(texture, color, x, y, w, h);

                // Console input >
                auto hashtag = draw->writeText("> ", {255, 175, 46}, 5, 30);
                int w, h;
                SDL_QueryTexture(hashtag.get(), NULL, NULL, &w, &h);
                draw->render(hashtag, 5, 180 - h / 6, w / 6, h / 6);

                int y = 0;
                for (int i = scroll; i < msgArray.size(); i++)
                {
                    std::string msgText = std::to_string(msgArray[i].getTime() / 10) + ": " + msgArray[i].getMessage();
                    auto text = draw->writeText(msgText, {255, 175, 46}, 0, y);
                    SDL_QueryTexture(text.get(), NULL, NULL, &w, &h);
                    draw->render(text, 5, y, w / 6, h / 6);
                    y += 20;
                    if (y >= 160)
                        scroll += 1;
                }
                if (inputText != "")
                {
                    //Render new text
                    auto text = draw->writeText(inputText, {255, 175, 46}, 5, 30);
                    int w, h;
                    SDL_QueryTexture(text.get(), NULL, NULL, &w, &h);
                    draw->render(text, 24, 180 - h / 6, w / 6, h / 6);
                }
                //call base class update method
                Screen::update();
            }
        }

    private:
        std::shared_ptr<SDL_Surface> surface = nullptr;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::vector<OWL::Message> msgArray;
        std::string inputText = "";
        int scroll = 0;
        OWL::Color color = {100, 100, 100, 180};

        void onNotify(OWL::Message msg)
        {
            msgArray.push_back(msg);
        }
    };

    class StartScreen : public OWL::Screen
    {
    public:
        StartScreen(const std::shared_ptr<OWL::MessageBus> msgBus, const std::shared_ptr<OWL::Draw> draw, int x, int y, int w, int h)
            : Screen(msgBus, draw, x, y, w, h)
        {
            surface = draw->loadFromFile("OWL/pixl.png");
            if (surface == nullptr)
            {
                std::cout << "error" << std::endl;
            }
        };

        void update()
        {
            int w, h;
            draw->drawImageFromFile(surface, 1280 / 4, 720 / 2);
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
        std::shared_ptr<SDL_Surface> surface = nullptr;
    };
} // namespace game