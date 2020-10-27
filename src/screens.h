#include <iostream>
#include "OWL/draw.h"
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
            : OWL::Screen(msgBus, draw, x, y, w, h, "ConsoleScreen") {}

        bool isOpen = false;

        void callMethod(std::vector<std::string> params)
        {
            if (params[1] == "open")
                openConsole();
            if (params[1] == "backspace")
                backSpace();
            if (params[1] == "enter")
                enter();
            if (params[1] == "moveup")
                moveUp();
            if (params[1] == "movedown")
                moveDown();
            if (params[1] == "text")
                writeToConsole(params[2]);
        }
        /**
         * @brief Open and close the console.
         *
         * @param inputTextEnabled Reference to bool to change if inputting is enabled or not.
         */
        void openConsole() //bool &inputTextEnabled)
        {
            if (!isOpen)
            {
                isOpen = true;
                send({"inputTextEnable"});
                SDL_StartTextInput();
            }
            else
            {
                isOpen = false;
                send({"inputTextDisable"});
                SDL_StopTextInput();
            }
        }

        //===Basic console typing & manipulation functions=== === === === ===

        void writeToConsole(std::string c)
        {
            inputText += c;
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

                send({inputText});
                inputText = "";
            }
        }
        void moveUp()
        {
            scrolling = true;
            // TODO: Doesn't work right
            scroll -= 1;
            if (scroll < 0)
                scroll = 0;
            std::cout << scroll << std::endl;
        }
        void moveDown()
        {
            scroll += 1;
            if (scroll > msgArray.size() - 5)
            {
                scroll = msgArray.size() - 5;
                scrolling = false;
            }
            std::cout << scroll << std::endl;
        }
        //============================================================================

        /**
         * @brief Draw the console and write messages to it.
         */
        void update()
        {
            //    std::cout << isOpen << std::endl;
            if (isOpen)
            {
                draw->createEmptyTexture(texture, color, x, y, w, h);
                int ty = 0;

                //=== Write message array into console
                for (int i = scroll; i < msgArray.size(); i++)
                {
                    std::string s;
                    for (const auto test : msgArray[i].getParameters())
                        s += test;
                    std::string msgText = std::to_string(msgArray[i].getTime() / 10) + ": " + s;
                    auto text = draw->writeText(msgText, foreground, 0, ty);
                    // get the text size
                    SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
                    draw->render(text, 5, ty, tw / 6, th / 6);
                    // move to next line. If the console is full, only draw most recent messages.
                    ty += th / 6;
                    if (ty >= 160 && !scrolling)
                        scroll += 1;
                }

                //=== Console input ===
                auto hashtag = draw->writeText("> ", foreground, 5, 30);
                SDL_QueryTexture(hashtag.get(), NULL, NULL, &tw, &th);
                draw->render(hashtag, 5, h - tw / 6, tw / 6, th / 6);

                // if user has written something, render it to the console bottom
                if (inputText != "")
                {
                    //Render new text input
                    auto text = draw->writeText(inputText, foreground, 5, 30);
                    SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
                    draw->render(text, 24, h - th / 6, tw / 6, th / 6);
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
        bool scrolling = false;

        // when message is received, push it to messageArray
        void onNotify(OWL::Message msg)
        {
            auto params = msg.getParameters();
            if (msg.getParameter(0) == "inputconsole")
                callMethod(params);
            //TODO: hide open and close console messages
            else
                msgArray.push_back(msg);
        }
    };

    class StartScreen : public OWL::Screen
    {
    public:
        StartScreen(const std::shared_ptr<OWL::MessageBus> msgBus, const std::shared_ptr<OWL::Draw> draw, int x, int y, int w, int h)
            : Screen(msgBus, draw, x, y, w, h, "StartScreen"), surface{draw->loadFromFile("../src/OWL/pixl.png")} {}

        void update()
        {
            //  draw->drawImageFromFile(surface, OWL::SCREEN_WIDTH / 4, OWL::SCREEN_HEIGHT / 2);
            imageTexture = draw->createTextureFromSurface(surface);
            SDL_QueryTexture(imageTexture.get(), NULL, NULL, &tw, &th);
            draw->createViewport(imageTexture.get(), OWL::SCREEN_WIDTH - tw / 4, OWL::SCREEN_HEIGHT - th / 4, tw / 4, th / 4);

            auto text = draw->writeText("THE OWL ENGINE", {255, 175, 46}, OWL::SCREEN_WIDTH / 2, 100);
            SDL_QueryTexture(text.get(), NULL, NULL, &tw, &th);
            draw->createViewport(text.get(), OWL::SCREEN_WIDTH / 2 - tw / 4, 100, tw / 2, th / 2);
            draw->render(text, 0, 0, tw / 2, th / 2);
            Screen::update();
        }

    private:
        std::shared_ptr<SDL_Surface> surface = nullptr;
        std::shared_ptr<SDL_Texture> imageTexture = nullptr;
        int tw, th; // texture width and height
    };
} // namespace game