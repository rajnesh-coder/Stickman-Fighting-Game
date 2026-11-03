#pragma once
#include<SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdexcept>
#include<string>
#include<exception>

class LImagetexture{
    private:

    SDL_Texture* mTexture;
    int w,h;

    public:

    LImagetexture():mTexture(nullptr),w(0),h(0){}

      bool LoadTextureFromImage(SDL_Renderer* mRenderer, std::string path)
      {
      SDL_Surface* image = IMG_Load(path.c_str());
      const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(image->format);
      SDL_Palette* palette =  SDL_GetSurfacePalette(image);
      int Colorkey = SDL_MapRGB(details, palette, 0x00, 0x00, 0x00);
      SDL_SetSurfaceColorKey(image, true, Colorkey);
      if(!image){
          SDL_Log(("IMG at path:"+path+" couldn't be loaded").c_str(), SDL_GetError());
          throw std::invalid_argument("Invalid path! image not loaded");
      }
      mTexture = SDL_CreateTextureFromSurface(mRenderer, image);
      w = image->w;
      h = image->h;
      SDL_DestroySurface(image);
      return mTexture != nullptr;
    }
    bool Render(SDL_Renderer* mRenderer, SDL_FRect* src_rect, SDL_FRect* dest_rect, double angle, const SDL_FPoint *center,SDL_FlipMode flip){
        //SDL_RenderTexture(mRenderer, mTexture, src_rect, dest_rect);
       bool render =  SDL_RenderTextureRotated(mRenderer, mTexture, src_rect, dest_rect, angle, center, flip);
        return render;
    }
};

class LTexttexture{ //updated the Ltexttexture class
private:
    TTF_Font* mFont;
    SDL_Texture* mTextTexture;
    int w, h;
public:
    LTexttexture(): mFont(nullptr), mTextTexture(nullptr), w(0), h(0) {};

    bool LoadText(SDL_Renderer* mRenderer, std::string font_path, std::string text, SDL_Color color, float fontSize = 28.0f){
        if (!mFont) //if no font already loaded then it loads a font
        {
            mFont = TTF_OpenFont(font_path.c_str(), fontSize);
        }
        if (!mFont) return false; //if still no font loaded it will stop the code

        SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, text.c_str(), 0, color);

        if(mTextTexture)//if a texture already exists it destroys it
        {
            SDL_DestroyTexture(mTextTexture);
        }

        mTextTexture  = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        w = textSurface->w;
        h = textSurface->h;

        SDL_DestroySurface(textSurface);
        return mTextTexture != nullptr;
    }

    bool Render(SDL_Renderer* mRenderer, SDL_FRect* dest_rect){
        return SDL_RenderTexture(mRenderer, mTextTexture, nullptr, dest_rect);
    }
};
