#pragma once

#include "Ltexture.hpp"
#include "HUD.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

const float floorheight = 700.0f;

// =====================================================
// SPRITE DETAILS
// =====================================================

struct sprite_details {

    std::string address;
    int frames;
    int width;
    int height;
};

sprite_details idle = {
    "StickmanPack/idle.png",
    8,
    6192,
    783
};

sprite_details walk = {
    "StickmanPack/walk-1.png",
    10,
    7740,
    783
};

sprite_details jump = {
    "StickmanPack/jump.png",
    10,
    9900,
    990
};

sprite_details death = {
    "StickmanPack/death.png",
    8,
    6192,
    783
};

sprite_details a = {
    "StickmanPack/punch.png",
    9,
    8910,
    990
};

sprite_details s = {
    "StickmanPack/sweepkick.png",
    8,
    7920,
    990
};

sprite_details d = {
    "StickmanPack/powerpunch.png",
    8,
    6192,
    783
};

sprite_details q = {
    "StickmanPack/uppercut.png",
    9,
    8910,
    990
};

sprite_details w = {
    "StickmanPack/throw.png",
    12,
    12312,
    1026
};

sprite_details e1 = {
    "StickmanPack/uppercut.png",
    9,
    8910,
    990
};



// =====================================================
// ANIMATION
// =====================================================

struct Animation {

    std::vector<SDL_FRect> Frames;

    int currentFrame = 0;
    int maxFrames = 0;

    Uint64 lastFrameTime = 0;
    Uint64 frameDelay = 100;



    Animation() {}

    Animation(SDL_FRect src, int NumFrms) {

        currentFrame = 0;
        maxFrames = NumFrms;

        float frameWidth =
            float(src.w) / float(NumFrms);

        for (int i = 0; i < NumFrms; i++) {

            SDL_FRect frame;

            frame.x = frameWidth * i;
            frame.y = 0;
            frame.w = frameWidth;
            frame.h = src.h;

            Frames.push_back(frame);
        }
    }



    SDL_FRect* GetFrame() {

        return &Frames.at(currentFrame);
    }



    void playAnimatoin() {

        Uint64 now = SDL_GetTicks();

        if (now > lastFrameTime + frameDelay) {

            currentFrame++;

            if (currentFrame >= maxFrames) {
                currentFrame = 0;
            }

            lastFrameTime = now;
        }
    }



    void Reset() {

        currentFrame = 0;
    }
};



// =====================================================
// ENTITY
// =====================================================

template<typename T>

class Entity {

public:

    LImagetexture character;

    T health;

    SDL_FRect* dest_rect;
    SDL_FRect* src_rect;

    SDL_FPoint* center;

    float VelX;
    float VelY;

    float gravity;
    float friction;

    Animation* lAnimation;

    SDL_FlipMode flipmode;

    double angle;

    bool direction;
    bool isjumping;
    bool ishit;



    Entity(SDL_Renderer* mRenderer,
           const char* path,
           T health)
    {

        dest_rect = new SDL_FRect{0, 0, 300, 300};

        src_rect = new SDL_FRect{
            0,
            0,
            float(idle.width) / idle.frames,
            float(idle.height)
        };

        center = new SDL_FPoint{
            dest_rect->w / 2,
            dest_rect->h / 2
        };

        lAnimation = new Animation(
            SDL_FRect{
                0,
                0,
                float(idle.width),
                float(idle.height)
            },
            idle.frames
        );

        this->health = health;

        flipmode = SDL_FLIP_NONE;
        angle = 0;

        character.LoadTextureFromImage(mRenderer, path);

        direction = true;
        ishit = false;
        isjumping = false;

        VelX = 12;
        VelY = 0;

        friction = 5.0;
        gravity = 0.8f;
    }



    virtual ~Entity() {

        delete dest_rect;
        delete src_rect;
        delete center;
        delete lAnimation;
    }



    void setPos(float x, float y) {

        dest_rect->x = x;
        dest_rect->y = y;
    }



    void setDim(float w, float h) {

        dest_rect->w = w;
        dest_rect->h = h;
    }

    Entity<T>& operator-=(T damageAmount) //overloading operator for taking damage
    {
        this->health -= damageAmount;
        if (this->health < 0) this->health = 0;
        return *this; 
    }

    virtual void Update() {

        VelY += gravity;

        setPos(dest_rect->x, dest_rect->y + VelY);

        //handling the floor
        if (dest_rect->y >= floorheight) {

            setPos(dest_rect->x, floorheight);

            VelY = 0;
            isjumping = false;
        }

        if (lAnimation != nullptr) {

            src_rect = lAnimation->GetFrame();
            lAnimation->playAnimatoin();
        }
    }

    virtual void Render(SDL_Renderer* mRenderer) {

        character.Render(
            mRenderer,
            src_rect,
            dest_rect,
            angle,
            center,
            flipmode
        );
    }



    virtual T getHealth() {

        return health;
    }
};



// =====================================================
// PLAYER
// =====================================================

template<typename T>

class Player : public Entity<T> {

private:

    HUD playerHUD;

    std::string currentAnimation = "idle";



public:

    Uint32 hitTime = 0;
    bool invincible = false;



    Player(SDL_Renderer* mRenderer,
           const char* path,
           T health)

        : Entity<T>(mRenderer, path, health),
          playerHUD(mRenderer, health)
    {}



    void ChangeAnimation(SDL_Renderer* mRenderer,
                         sprite_details anim,
                         std::string animName)
    {
        if (currentAnimation == animName)
            return;

        currentAnimation = animName;

        this->character.LoadTextureFromImage(
            mRenderer,
            anim.address
        );

        delete this->lAnimation;

        this->lAnimation = new Animation(
            SDL_FRect{
                0,
                0,
                float(anim.width),
                float(anim.height)
            },
            anim.frames
        );

        this->src_rect = this->lAnimation->GetFrame();
    }



    void KeyPress(SDL_Event &e, SDL_Renderer* mRenderer) {

        if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat) {

            if (e.key.key == SDLK_LEFT) {

                ChangeAnimation(mRenderer, walk, "walk");

                this->flipmode = SDL_FLIP_NONE;
                this->direction = false;
            }

            if (e.key.key == SDLK_RIGHT) {

                ChangeAnimation(mRenderer, walk, "walk");

                this->flipmode = SDL_FLIP_HORIZONTAL;
                this->direction = true;
            }

            if (e.key.key == SDLK_UP) {

                ChangeAnimation(mRenderer, jump, "jump");

                if (!this->isjumping) {

                    this->VelY = -12;
                    this->isjumping = true;
                }
            }
        }
    }



    void Update(SDL_Renderer* mRenderer) {

        this->VelY += this->gravity;

        this->setPos(this->dest_rect->x, this->dest_rect->y + this->VelY);

        //hnadling the floor
        if (this->dest_rect->y >= floorheight) {

            this->setPos(this->dest_rect->x, floorheight);

            this->VelY = 0;
            this->isjumping = false;
        }

        const bool* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_LEFT]) {
            this->setPos(this->dest_rect->x - this->VelX,
                         this->dest_rect->y);
        }

        if (keys[SDL_SCANCODE_RIGHT]) {
            this->setPos(this->dest_rect->x + this->VelX,
                         this->dest_rect->y);
        }

        if (invincible) {

            if (SDL_GetTicks() - hitTime >= 4000) {

                invincible = false;

                std::cout << "Immunity Ended\n";
            }
        }
        if(this->health == 0){
            ChangeAnimation(mRenderer, death, "death");
        }

        Entity<T>::Update();

        playerHUD.Update(mRenderer, this->health);
    }



    void Render(SDL_Renderer* mRenderer) {

        this->character.Render(
            mRenderer,
            this->src_rect,
            this->dest_rect,
            0.0,
            this->center,
            this->flipmode
        );

        playerHUD.Render(mRenderer);
    }
};



// =====================================================
// ENEMY
// =====================================================

template<typename T>

class Enemy : public Entity<T> {

    float damage = 5;



public:

    Enemy(SDL_Renderer* mRenderer,
          const char* path,
          T health)

        : Entity<T>(mRenderer, path, health)
    {
        this->VelX = 4;
    }



    void KeyPress(SDL_Event &e,
                  Player<T>* player,
                  SDL_Renderer *mRenderer)
    {
        if (e.type == SDL_EVENT_KEY_DOWN &&
            !e.key.repeat &&
            abs(player->dest_rect->x - this->dest_rect->x) < this->dest_rect->w &&
            !this->ishit)
        {
            if (e.key.key == SDLK_Q) {
                player->ChangeAnimation(mRenderer, q, "q");
                this->health -= 20;
                this->ishit = true;

                if(!player->direction && !this->isjumping){
                    this->VelY = -18;
                    this->isjumping = true;

                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)
                    this->setPos(this->dest_rect->x-this->VelX*15, this->dest_rect->y);
                }
            }

            else if (e.key.key == SDLK_W) {
                player->ChangeAnimation(mRenderer, w, "w");
                this->health -= 25;
                this->angle = 90;
                this->ishit = true;

                if(!this->isjumping && !this->direction){
                    this->angle = 90;
                    this->VelY = -20;
                    this->isjumping = true;
                }else if(!this->isjumping && this->direction){
                    this->angle = -90;
                    this->VelY = -20;
                    this->isjumping = true;
                }


            }

            else if (e.key.key == SDLK_E) {
                player->ChangeAnimation(mRenderer, e1, "e");
                this->health -= 20;
                this->ishit = true;

                if(player->direction && !this->isjumping){
                    this->VelY = -5;
                    this->isjumping = true;

                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)
                    this->setPos(this->dest_rect->x+this->VelX*15, this->dest_rect->y);
                }
            }

            else if (e.key.key == SDLK_D) {
                player->ChangeAnimation(mRenderer, d, "d");
                this->health -= 20;
                this->ishit = true;
                if(player->direction){
                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)

                    this->setPos(this->dest_rect->x+this->VelX*40, this->dest_rect->y);
                }
                else if(!player->direction){
                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)

                    this->setPos(this->dest_rect->x-this->VelX*40, this->dest_rect->y);
                }
            }

            else if (e.key.key == SDLK_S) {
                player->ChangeAnimation(mRenderer, s, "s");
                this->health -= 15;
                this->angle = 90;
                this->ishit = true;

                if(!this->isjumping && !player->direction){
                    this->angle = 90;
                    this->VelY = -6;
                    this->setPos(this->dest_rect->x-this->VelX*20, this->dest_rect->y);
                    this->isjumping = true;
                }else if(!this->isjumping && player->direction){
                    this->angle = -90;
                    this->VelY = -6;
                    this->setPos(this->dest_rect->x-this->VelX*40, this->dest_rect->y-this->dest_rect->w*10/8);
                    this->isjumping = true;
                }
            }

            else if (e.key.key == SDLK_A) {
                player->ChangeAnimation(mRenderer, a, "a");
                this->health -= 10;
                this->ishit = true;
                if(player->direction){
                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)

                    this->setPos(this->dest_rect->x+this->VelX*20, this->dest_rect->y);
                }
                else if(!player->direction){
                    if(abs(player->dest_rect->x - this->dest_rect->x) <= this->dest_rect->w*3/2)

                    this->setPos(this->dest_rect->x-this->VelX*20, this->dest_rect->y);
                }
            }
        }
    }



    void Update(Player<T>* player,SDL_Renderer* mRenderer) {

        this->VelY += this->gravity;

        this->setPos(this->dest_rect->x,
                      this->dest_rect->y + this->VelY);

        //handling the floor
        if (this->dest_rect->y >= floorheight) {

            this->setPos(this->dest_rect->x, floorheight);
            this->angle = 0;
            this->VelY = 0;
            this->isjumping = false;
        }

        if(this->health > 0){



            if (player->dest_rect->x > this->dest_rect->x && this->dest_rect->y >=ScreenHeight/2.0-this->dest_rect->h) {
                this->setPos(this->dest_rect->x + this->VelX,
                             this->dest_rect->y);
            }

            else if (player->dest_rect->x < this->dest_rect->x && this->dest_rect->y >= ScreenHeight/2.0-this->dest_rect->h ) {
                this->setPos(this->dest_rect->x - this->VelX,
                             this->dest_rect->y);
            }

            if (abs(player->dest_rect->x - this->dest_rect->x) < this->dest_rect->w / 6 &&
                player->health > 0)
            {

                if (!player->invincible && this->ishit) {

                    //player->health -= damage;
                    *player -= damage;

                    player->invincible = true;
                    player->hitTime = 0;
                    player->hitTime = SDL_GetTicks();

                    std::cout << player->health << "\n";
                }
                this->ishit = false;
            }

            if (this->dest_rect->x - player->dest_rect->x >= player->dest_rect->w / 8) {
                this->flipmode = SDL_FLIP_HORIZONTAL;
                this->direction = false;
            }
            else {
                this->flipmode = SDL_FLIP_NONE;
                this->direction = true;
            }

            if (this->lAnimation != nullptr) {

                this->src_rect = this->lAnimation->GetFrame();
                this->lAnimation->playAnimatoin();
            }
        }
        else{
            this->src_rect = new SDL_FRect{
                5418,
                0,
                6192/8.0,
                783
            };
         this->character.LoadTextureFromImage(mRenderer, death.address);
        }
    }



    void Render(SDL_Renderer* mRenderer) {

        this->character.Render(
            mRenderer,
            this->src_rect,
            this->dest_rect,
            this->angle,
            this->center,
            this->flipmode
        );
    }
};
