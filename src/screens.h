#include <iostream>
#include "OWL/screen.h"
#include "OWL/msg.h"
#include "OWL/globals.h"

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
     * @param msgBus reference to the MessageBus object
     * @param draw reference to the Draw object
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
                int ty = 0;

                //=== Write message array into console
                for (int i = scroll; i < msgArray.size(); i++)
                {
                    std::string msgText = std::to_string(msgArray[i].getTime() / 10) + ": " + msgArray[i].getMessage();
                    auto text = draw->writeText(msgText, foreground, 0, ty);
                    // get the text size
                    SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
                    draw->render(text, 5, ty, tw / 6, th / 6);
                    // move to next line. If the console is full, only draw most recent messages.
                    ty += th / 6;
                    if (ty >= 160)
                        scroll += 1;
                }

                //=== Console input ===
                auto hashtag = draw->writeText("> ", foreground, 5, 30);
                SDL_QueryTexture(hashtag.get(), NULL, NULL, &tw, &th);
                draw->render(hashtag, 5, 180 - th / 6, tw / 6, th / 6);

                // if user has written something, render it to the console bottom
                if (inputText != "")
                {
                    //Render new text input
                    auto text = draw->writeText(inputText, foreground, 5, 30);
                    SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
                    draw->render(text, 24, 180 - th / 6, tw / 6, th / 6);
                }
                //call base class update method
                Screen::update();
            }
        }

    private:
        std::shared_ptr<SDL_Surface> surface = nullptr;
        std::shared_ptr<SDL_Texture> texture = nullptr;
        std::vector<OWL::Message> msgArray;     // store all messages from system
        std::string inputText = "";             // text user is currently inputting
        SDL_Color color = {100, 100, 100, 180}; // console background color
        int scroll = 0;                         // starting index for messageArray
        int tw, th;                             // text width and height

        // when message is received, push it to messageArray
        void onNotify(OWL::Message msg)
        {
            msgArray.push_back(msg);
        }
    };

    class StartScreen : public OWL::Screen
    {
    public:
        StartScreen(const std::shared_ptr<OWL::MessageBus> msgBus, const std::shared_ptr<OWL::Draw> draw, int x, int y, int w, int h)
            : Screen(msgBus, draw, x, y, w, h), surface{draw->loadFromFile("../src/OWL/pixl.png")} {}

        void update()
        {
            if (surface == nullptr)
            {
                std::cout << "error" << std::endl;
            }
            if (draw == nullptr)
            {
                std::cout << "error1" << std::endl;
            }
            else
            {
                std::cout << "draw" << std::endl;
            }
            draw->drawImageFromFile(surface, OWL::SCREEN_WIDTH / 4, OWL::SCREEN_HEIGHT / 2);
            // auto imageTexture = draw->createTextureFromSurface(surface);
            //SDL_QueryTexture(imageTexture.get(), NULL, NULL, &tw, &th);
            // draw->createViewport(imageTexture.get(), OWL::SCREEN_WIDTH - tw / 4, OWL::SCREEN_HEIGHT - th / 4, tw / 4, th / 4);

            // auto text = draw->writeText("THE OWL ENGINE", {255, 175, 46}, OWL::SCREEN_WIDTH / 2, 100);
            // SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
            // draw->createViewport(text.get(), OWL::SCREEN_WIDTH / 2 - tw / 4, 100, tw / 2, th / 2);
            // draw->render(text, 0, 0, tw / 2, th / 2);
            // Screen::update();
        }

    private:
        std::shared_ptr<SDL_Surface> surface = nullptr;
        int tw, th; // texture width and height
    };
} // namespace game