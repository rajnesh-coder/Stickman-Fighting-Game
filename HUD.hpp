#include "Ltexture.hpp"
#include<iostream>
#include<cmath>
using namespace std;

const float ScreenWidth = 1920;
const float ScreenHeight = 1080;

class HUD
{
    private:
    SDL_FRect playerHealthRect;
    LTexttexture playerHealthText;
    LImagetexture healthBar;
    SDL_FRect healthBarRect;
    double playerHealth;


    public:
    HUD(SDL_Renderer* mRenderer, double health) : playerHealth(health)
    {
        healthBarRect = {50.0f,ScreenHeight/2 + 430.0f, 600.0f, 30.0f};
        healthBar.LoadTextureFromImage(mRenderer, "Healthbar/full.png");
        playerHealthRect = {70.0f,ScreenHeight/2 + 433.0f, 50.0f, 30.0f};
        playerHealthText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf",to_string((int)playerHealth),SDL_Color{0,0,0,0},32.0f);
    }

    void setHealth(double nhealth)
    {
        playerHealth = nhealth;
    }

    void Update(SDL_Renderer* mRenderer, double health)
    {
        setHealth(health); //whenever the player gets hit, the health is changed to reflect it
        playerHealthText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf",to_string((int)playerHealth),SDL_Color{0,0,0,0},32.0f);
        if(playerHealth <= 80 && playerHealth > 70)
        {
            healthBar.LoadTextureFromImage(mRenderer, "Healthbar/1.png");
        }
        if(playerHealth <= 70 && playerHealth > 60)
        {
            healthBar.LoadTextureFromImage(mRenderer, "Healthbar/2.png");
        }
        if(playerHealth <= 60 && playerHealth > 50)
        {
            healthBar.LoadTextureFromImage(mRenderer, "Healthbar/3.png");
        }
        if(playerHealth <= 50 && playerHealth > 30)
        {
            healthBar.LoadTextureFromImage(mRenderer, "Healthbar/4.png");
        }
        if(playerHealth <= 30 && playerHealth > 15)
        {
            healthBar.LoadTextureFromImage(mRenderer, "Healthbar/5.png");
        }

    }

    void Render(SDL_Renderer* mRenderer)
    {
        healthBar.Render(mRenderer, nullptr, &healthBarRect, 0.0, nullptr, SDL_FLIP_NONE);
        playerHealthText.Render(mRenderer, &playerHealthRect);
    }

};
