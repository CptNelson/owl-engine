#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

//====================================================================================================

//Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture();
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path);

    //Creates image from font string
    SDL_Surface loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer *gRenderer);

    //Deallocates texture
    void free();

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Renderer *renderer, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    //Gets image dimensions
    int getWidth();
    int getHeight();

private:
    //The actual hardware texture
    SDL_Texture *texture;

    //Image dimensions
    int mWidth;
    int mHeight;
    TTF_Font *font = nullptr;
};

//====================================================================================================

LTexture::LTexture()
{
    //Initialize
    font = TTF_OpenFont("framework/hack-regular.ttf", 120);
    std::shared_ptr<SDL_Renderer> renderer = nullptr;
    texture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}

/*
bool LTexture::loadFromFile(std::string path)
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = NULL;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}
*/

SDL_Surface LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer *gRenderer)
{
    if (!font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }

    //Render text surface
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
    return *textSurface;
}

void LTexture::free()
{
    //Free texture if it exists
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    //Modulate texture rgb
    SDL_SetTextureColorMod(texture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(texture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(texture, alpha);
}

void LTexture::render(std::shared_ptr<SDL_Texture> texture, int x, int y, int width, int height, SDL_Renderer *renderer, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    //std::cout << width << std::endl;
    SDL_Rect renderQuad = {x, y, width / 8, height / 8};

    //Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(renderer, texture.get(), clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}