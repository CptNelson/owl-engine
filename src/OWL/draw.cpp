#include "draw.h"
#include "globals.h"
#include "msg.h"

namespace OWL
{

    Draw::Draw(const std::shared_ptr<MessageBus> msgBus, SDL_Window *window)
        : BusNode(msgBus, "Draw"), font{TTF_OpenFont(defaultFont, 120)}, width{0}, height{0},
          renderer{sdl_shared(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))}
    {
        //font = TTF_OpenFont("OWL/hack-regular.ttf", 120);
        // font = TTF_OpenFont(defaultFont, 120);
        // width = 0;
        // height = 0;
        // renderer = sdl_shared(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
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
        return texture;
    };

    void Draw::createEmptyTexture(std::shared_ptr<SDL_Texture> texture, SDL_Color &c, int x, int y, int w, int h)
    {
        texture = OWL::sdl_shared(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1));
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
        fillTexture(texture.get(), c.r, c.g, c.b, c.a);
        SDL_SetRenderTarget(renderer.get(), NULL);
        createViewport(texture.get(), x, y, w, h);
    }

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
        auto textSurface = loadFromRenderedText(text, color);
        auto textTexture = createTextureFromSurface(textSurface);
        return textTexture;
        //render(textTexture, x, y, width, height);
    };

    void Draw::update()
    {
        SDL_RenderPresent(renderer.get());
        // set the default color back to black
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
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

    std::shared_ptr<SDL_Surface> Draw::loadFromRenderedText(std::string textureText, SDL_Color textColor)
    {
        if (!font)
        {
            printf("TTF_OpenFont2: %s\n", TTF_GetError());
            // handle error
        }

        //Render text surface
        std::shared_ptr<SDL_Surface> textSurface = OWL::sdl_shared(TTF_RenderText_Blended(font, textureText.c_str(), textColor));
        return textSurface;
    }

    void Draw::drawBox(int x, int y, int w, int h, SDL_Color c, int t)
    {

        SDL_Point points[5] = {
            {x, y},
            {w / t - 1, y},
            {w / t - 1, h / t - 1},
            {x, h / t - 1},
            {x, y}};
        SDL_SetRenderDrawColor(renderer.get(), c.r, c.g, c.b, c.a);
        SDL_RenderSetScale(renderer.get(), t, t);
        SDL_RenderDrawLines(renderer.get(), points, 5);
        reset();
    }
    void Draw::reset()
    {
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
        SDL_RenderSetScale(renderer.get(), 1, 1);
    }
} // namespace OWL