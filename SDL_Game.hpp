#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_error.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <fstream>

#include "Entity.hpp"
#include "Ltexture.hpp"

enum class GameState  //constants to track the state of the game
{
    MENU, //for main menu
    LEVEL_SELECT, //level select screen
    PLAYING, //for when in game
    PAUSED, //the pause menu
    LEVEL_COMPLETE
};

class SDL_Game{
private:
SDL_Window* mWindow = nullptr;
SDL_Renderer* mRenderer = nullptr;

MIX_Mixer* mMixer = nullptr;
MIX_Track* mMusicTrack = nullptr;
MIX_Track* mEffectTrack = nullptr;

MIX_Audio* gScratchAudio{ nullptr };
MIX_Audio* gHighAudio{ nullptr };
MIX_Audio* gMediumAudio{ nullptr };
MIX_Audio* gLowAudio{ nullptr };

//--- START MENU ---
SDL_FRect playButtonRect;
LTexttexture playButtonText;

SDL_FRect level1ButtonRect;
LTexttexture level1ButtonText;

SDL_FRect quitButtonRect;
LTexttexture quitButtonText;


//--- LEVEL SELECT MENU ---
SDL_FRect level1TextRect;
LTexttexture level1Text;

SDL_FRect level2TextRect;
LTexttexture level2Text;

SDL_FRect level3TextRect;
LTexttexture level3Text;

SDL_FRect level4TextRect;
LTexttexture level4Text;

SDL_FRect lsBackButtonRect;
LTexttexture lsBackButtonText;

SDL_FRect lvl1ButtonRect;
LImagetexture lvl1Button;

SDL_FRect lvl2ButtonRect;
LImagetexture lvl2Button;

SDL_FRect lvl3ButtonRect;
LImagetexture lvl3Button;

SDL_FRect lvl4ButtonRect;
LImagetexture lvl4Button;

LTexttexture completeStatusText;
SDL_FRect complete1;
SDL_FRect complete2;
SDL_FRect complete3;
SDL_FRect complete4;

//--- PAUSE MENU ---
SDL_FRect resumeButtonRect;
LTexttexture resumeButtonText;

SDL_FRect quitToMenuButtonRect;
LTexttexture quitToMenuButtonText;


//--- LEVEL COMPLETE MENU ---
SDL_FRect youWinTextRect;
LTexttexture youWinText;

SDL_FRect nextLevelButtonRect;
LTexttexture nextLevelButtonText;

SDL_FRect lcQuitToMenuButtonRect;
LTexttexture lcQuitToMenuButtonText;

int selectedLevel = 1; //tracking which level is selected
SDL_FRect bgRect; //rectangle for background

Player<double>* player;
Enemy<double>* enemy;
Enemy<double>* enemy2;
Enemy<double>* enemy3;
// Entity<double>* enemy_2;
bool running = true;
GameState currentstate;

int ScreenWidth = 1900;
int ScreenHeight = 1080;

bool levelCompleted[4] = {false, false, false, false};

public:
SDL_Game(const char* title){
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        SDL_Log("SDL failed to initialize %s\n",SDL_GetError());
    }

    mWindow = SDL_CreateWindow(title,ScreenWidth,ScreenHeight,SDL_WINDOW_RESIZABLE);
    if(mWindow == nullptr){
        SDL_Log("Window could not be created");
    }
    mRenderer = SDL_CreateRenderer(mWindow,nullptr);
    if(mRenderer == nullptr){
        SDL_Log("Renderer could not be created");
    }
    if(!TTF_Init()){
        SDL_Log("Not able to initialite TTF%s\n", SDL_GetError());
    }
    if(!MIX_Init()){
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError() );
    }else{
        if(mMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr); mMixer == nullptr)
            SDL_Log("SDL_mixer could not create mixer! SDL_mixer Error%s\n", SDL_GetError());
    }

    // gScratchAudio = MIX_LoadAudio(mMixer, "Audio/scratch.wav", true);
    // gHighAudio = MIX_LoadAudio(mMixer, "Audio/high.wav", true);
    // gMediumAudio = MIX_LoadAudio(mMixer, "Audio/medium.wav", true);
    // gLowAudio  = MIX_LoadAudio(mMixer, "Audio/low.wav",true);

    mEffectTrack = MIX_CreateTrack(mMixer);

    if(mEffectTrack == nullptr){
        SDL_Log("Failed to create sound effect track DSL_mixer Error %s\n", SDL_GetError());
    }
    if(MIX_Audio* musicAudio = MIX_LoadAudio(mMixer, "Audio/beat.wav", false); musicAudio == nullptr){

    } else{
        MIX_SetTrackAudio(mMusicTrack,musicAudio);
    }

    LoadGame(); //loads your save data

    //level init "test for now"
    player = new Player<double>(mRenderer,"StickmanPack/idle.png",100.0);
    enemy = new Enemy<double>(mRenderer,"StickmanPack/walk-1.png",100.0);
    enemy2 = new Enemy<double>(mRenderer, "StickmanPack/walk-1.png",100.0);
    enemy3 = new Enemy<double>(mRenderer,"StickmanPack/walk-1.png",100.0);
    float posX = ScreenWidth/2.0 - 384/6.0, posY = ScreenHeight/2.0;
    cout << "lol\n";
    player->setPos(posX, posY);
    player->setDim(384.0/3.0, 64*2.0);
    cout << "lol1\n";
    enemy->setPos(0, posY);
    enemy->setDim(384.0/3.0, 64*2.0);
    enemy2->setDim(384.0/3.0, 64*2.0);
    enemy3->setDim(384.0/3.0, 64*2.0);
    cout << "lol2\n";

    bgRect = {0, 130.0f, (float)ScreenWidth, (float)ScreenHeight - 300};

    //###############################
    //########### MENUS #############
    //###############################

    //start menu
    playButtonRect = {ScreenWidth/2.0f - 90.0f, ScreenHeight/2.0f - 145.0f, 150.0f, 65.0f};
    playButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Play",SDL_Color{0,0,0,0},32.0f);

    level1ButtonRect = {ScreenWidth/2.0f - 110.0f, ScreenHeight/2.0f - 75.0f, 220.0f, 65.0f};
    level1ButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Level Select",SDL_Color{0,0,0,0},32.0f);

    quitButtonRect = {ScreenWidth/2.0f - 90.0f, ScreenHeight/2.0f - 5.0f, 150.0f, 65.0f};
    quitButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Quit",SDL_Color{0,0,0,0},32.0f);

    try
    {
        lvl1Button.LoadTextureFromImage(mRenderer, "lvldat/lvl1.png");
        lvl2Button.LoadTextureFromImage(mRenderer, "lvldat/lvl2.png");
        lvl3Button.LoadTextureFromImage(mRenderer, "lvldat/lvl3.png");
        lvl4Button.LoadTextureFromImage(mRenderer, "lvldat/lvl4.png");
    }
    catch(exception& e)
    {
        cout<<"Exception caught: "<<e.what();
    }
    //level select menu
    level1TextRect = {ScreenWidth/2.0f - 680.0f, ScreenHeight/2.0f + 95.0f, 150.0f, 45.0f};
    level1Text.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Level 1",SDL_Color{0,0,0,0},32.0f);

    level2TextRect = {ScreenWidth/2.0f - 280.0f, ScreenHeight/2.0f + 95.0f, 150.0f, 45.0f};
    level2Text.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Level 2",SDL_Color{0,0,0,0},32.0f);

    level3TextRect = {ScreenWidth/2.0f + 120.0f, ScreenHeight/2.0f + 95.0f, 150.0f, 45.0f};
    level3Text.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Level 3",SDL_Color{0,0,0,0},32.0f);

    level4TextRect = {ScreenWidth/2.0f + 520.0f, ScreenHeight/2.0f + 95.0f, 150.0f, 45.0f};
    level4Text.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Level 4",SDL_Color{0,0,0,0},32.0f);

    lsBackButtonRect = {ScreenWidth/2.0f - 90.0f, ScreenHeight/2.0f + 200.0f, 150.0f, 65.0f};
    lsBackButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Back",SDL_Color{0,0,0,0},32.0f);

    lvl1ButtonRect = {ScreenWidth/2.0f - 750.0f, ScreenHeight/2.0f - 75.0f, 300.0f, 150.0f};

    lvl2ButtonRect = {ScreenWidth/2.0f - 350.0f, ScreenHeight/2.0f - 75.0f, 300.0f, 150.0f};

    lvl3ButtonRect = {ScreenWidth/2.0f + 50.0f, ScreenHeight/2.0f - 75.0f, 300.0f, 150.0f};

    lvl4ButtonRect = {ScreenWidth/2.0f + 450.0f, ScreenHeight/2.0f - 75.0f, 300.0f, 150.0f};

    //destination rects for the "complete!" text that will show when a level is complete
    completeStatusText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf", "Complete!", SDL_Color{34, 139, 34, 255}, 36.0f); // Green text
    complete1 = {ScreenWidth/2.0f - 675.0f, ScreenHeight/2.0f - 120.0f, 150.0f, 40.0f};
    complete2 = {ScreenWidth/2.0f - 275.0f, ScreenHeight/2.0f - 120.0f, 150.0f, 40.0f};
    complete3 = {ScreenWidth/2.0f + 125.0f, ScreenHeight/2.0f - 120.0f, 150.0f, 40.0f};
    complete4 = {ScreenWidth/2.0f + 525.0f, ScreenHeight/2.0f - 120.0f, 150.0f, 40.0f};

    //pause menu
    resumeButtonRect = {ScreenWidth/2.0f - 90.0f, ScreenHeight/2.0f - 75.0f, 150.0f, 65.0f};
    resumeButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Resume",SDL_Color{0,0,0,0},32.0f);

    quitToMenuButtonRect = {ScreenWidth/2.0f - 90.0f, ScreenHeight/2.0f - 5.0f, 150.0f, 65.0f};
    quitToMenuButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Quit To Menu",SDL_Color{0,0,0,0},32.0f);


    //level complete menu
    youWinTextRect = {ScreenWidth/2.0f - 100.0f, ScreenHeight/2.0f - 150.0f, 200.0f, 65.0f};
    youWinText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","You Win!",SDL_Color{0,0,0,0},48.0f);

    nextLevelButtonRect = {ScreenWidth/2.0f - 110.0f, ScreenHeight/2.0f - 20.0f, 220.0f, 65.0f};
    nextLevelButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Next Level",SDL_Color{0,0,0,0},32.0f);

    lcQuitToMenuButtonRect = {ScreenWidth/2.0f - 110.0f, ScreenHeight/2.0f + 60.0f, 220.0f, 65.0f};
    lcQuitToMenuButtonText.LoadText(mRenderer, "Fonts/MorrisRomanBlack.ttf","Quit To Menu",SDL_Color{0,0,0,0},32.0f);

    //game state on startup will always be in the menu
    currentstate = GameState::MENU;
}
bool flip = true;
//change where enemies look based on player postion
//

void SaveGame() //for saving the game
{
    ofstream saveFile("SaveGame/savegame.txt");
    if (saveFile.is_open())
    {
        for (int i = 0; i < 4; i++)
        {
            saveFile << levelCompleted[i] << " ";
        }
        saveFile.close();
        cout << "Game Saved Successfully!" << endl;
    }
    else
    {
        cout << "Error: Unable to save game data." << endl;
    }
}

void LoadGame() //for loading the game
{
    ifstream loadFile("SaveGame/savegame.txt");
    if (loadFile.is_open())
    {
        for (int i = 0; i < 4; i++)
        {
            loadFile >> levelCompleted[i];
        }
        loadFile.close();
        cout << "Save Data Loaded." << endl;
    }
    else
    {
        cout << "No save file found. Starting fresh." << endl;
    }
}

//completely resets the player and enemy when called
void ResetGame()
{
    if(selectedLevel == 1)
    {
        float posX = ScreenWidth/2.0 - 384/6.0, posY = ScreenHeight/2.0;
        // Reset Player
        player->health = 100.0;
        player->setPos(posX+700, posY+200);
        player->VelX = 12;
        player->VelY = 0;
        player->isjumping = false;
        player->invincible = false;
        player->flipmode = SDL_FLIP_NONE;
        player->direction = true;
        player->ChangeAnimation(mRenderer, idle, "idle");

        // Reset Enemy
        enemy->health = 100.0;
        enemy->setPos(0, posY+200);
        enemy->VelX = 4;
        enemy->VelY = 0;
        enemy->isjumping = false;
        enemy->ishit = false;
        enemy->flipmode = SDL_FLIP_NONE;
        enemy->direction = true;
        enemy->angle = 0;
        //reload the texture if the enemy is dead
        enemy->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy->lAnimation) {
            enemy->lAnimation->Reset();
        }
    }
    else if(selectedLevel == 2)
    {
        float posX = ScreenWidth/2.0 - 384/6.0, posY = ScreenHeight/2.0;
        // Reset Player
        player->health = 100.0;
        player->setPos(posX, posY+200);
        player->flipmode = SDL_FLIP_NONE;
        player->ChangeAnimation(mRenderer, idle, "idle");

        // Reset Enemy
        enemy->health = 200.0; //increased health for level 2
        enemy->setPos(0, posY+200);
        //reload the texture if the enemy is dead
        enemy->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy->lAnimation) {
            enemy->lAnimation->Reset();
        }

        enemy2->health = 200.0; //increased health for level 2
        enemy2->setPos(posX+700, posY+200);
        //reload the texture if the enemy is dead
        enemy2->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy2->lAnimation) {
            enemy2->lAnimation->Reset();
        }

    }

    else if(selectedLevel == 3)
    {
        float posX = ScreenWidth/2.0 - 384/6.0, posY = ScreenHeight/2.0;
        // Reset Player
        player->health = 100.0;
        player->setPos(posX, posY+200);
        player->flipmode = SDL_FLIP_NONE;
        player->ChangeAnimation(mRenderer, idle, "idle");

        // Reset Enemy
        enemy->health = 350.0; //increased health for level 3
        enemy->setPos(0, posY+200);
        //reload the texture if the enemy is dead
        enemy->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy->lAnimation) {
            enemy->lAnimation->Reset();
        }

        enemy2->health = 350.0; //increased health for level 3
        enemy2->setPos(posX+700, posY+200);
        //reload the texture if the enemy is dead
        enemy2->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy2->lAnimation) {
            enemy2->lAnimation->Reset();
        }
    }

    else if(selectedLevel == 4)
    {
        float posX = ScreenWidth/2.0 - 384/6.0, posY = ScreenHeight/2.0;
        // Reset Player
        player->health = 100.0;
        player->setPos(posX, posY+200);
        player->flipmode = SDL_FLIP_NONE;
        player->ChangeAnimation(mRenderer, idle, "idle");

        // Reset Enemy
        enemy->health = 350.0;
        enemy->setPos(posX+500, posY+200);
        //reload the texture if the enemy is dead
        enemy->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy->lAnimation) {
            enemy->lAnimation->Reset();
        }

        enemy2->health = 350.0;
        enemy2->setPos(0, posY+200);
        //reload the texture if the enemy is dead
        enemy2->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy2->lAnimation) {
            enemy2->lAnimation->Reset();
        }

        enemy3->health = 350.0;
        enemy3->setPos(150, posY+200);
        //reload the texture if the enemy is dead
        enemy3->character.LoadTextureFromImage(mRenderer, "StickmanPack/walk-1.png");
        if (enemy3->lAnimation) {
            enemy3->lAnimation->Reset();
        }
    }
}

void SceneData(){

    // float posX = ScreenWidth/2.0 - 576/12.0, posY = ScreenHeight/2.0;
    // player->setPos(posX, posY);
    // player->setDim(576/6.0, 64);

    // enemy2->setPos(ScreenWidth/2.0-ScreenWidth/4.0 - 576/12.0, ScreenHeight/2.0);
    // enemy2->setDim(576/6.0, 64);

    // if(enemy2->center->x - player->center->x > 0.0){

    // }
    // if(enemy2->center->x - player->center->x > 0.0){

    // }

    // enemy_2->setPos(ScreenWidth/2.0+ScreenWidth/4.0 - 576/12.0, ScreenHeight/2.0);
    // enemy_2->setDim(576/6.0, 64);

    // if(enemy2->center->x - player->center->x < 0.0){

    // }
    // if(enemy2->center->x - player->center->x > 0.0){

    // }

}

//change the path and number of prames based on which key is pressed
void Input(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
    if(e.type == SDL_EVENT_QUIT)
    {
        running = false;
    }

    if (currentstate == GameState::MENU) {
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                //mouse coords
                float mouseX = e.button.x;
                float mouseY = e.button.y;

                //checks collision of mouse with playbutton
                if (mouseX >= playButtonRect.x && mouseX <= (playButtonRect.x + playButtonRect.w) && mouseY >= playButtonRect.y && mouseY <= (playButtonRect.y + playButtonRect.h))
                {
                    ResetGame();
                    currentstate = GameState::PLAYING;
                }

                //checks collision with level select button
                if (mouseX >= level1ButtonRect.x && mouseX <= (level1ButtonRect.x + level1ButtonRect.w) && mouseY >= level1ButtonRect.y && mouseY <= (level1ButtonRect.y + level1ButtonRect.h))
                {
                    currentstate = GameState::LEVEL_SELECT;
                }

                //checks collision with quit button
                if (mouseX >= quitButtonRect.x && mouseX <= (quitButtonRect.x + quitButtonRect.w) && mouseY >= quitButtonRect.y && mouseY <= (quitButtonRect.y + quitButtonRect.h))
                {
                    //if clicked, game quits
                    running = false;
                }
            }
        }

    else if(currentstate == GameState::LEVEL_SELECT)
    {
        if(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
        float mouseX = e.button.x;
        float mouseY = e.button.y;

        //checks collision with back button
        if (mouseX >= lsBackButtonRect.x && mouseX <= (lsBackButtonRect.x + lsBackButtonRect.w) && mouseY >= lsBackButtonRect.y && mouseY <= (lsBackButtonRect.y + lsBackButtonRect.h))
        {
            currentstate = GameState::MENU;
        }

        //checks collision with level buttons and selects the level based on that
        if (mouseX >= lvl1ButtonRect.x && mouseX <= (lvl1ButtonRect.x + lvl1ButtonRect.w) && mouseY >= lvl1ButtonRect.y && mouseY <= (lvl1ButtonRect.y + lvl1ButtonRect.h))
        {
            selectedLevel = 1;
            ResetGame();
            currentstate = GameState::PLAYING;
        }
        if (mouseX >= lvl2ButtonRect.x && mouseX <= (lvl2ButtonRect.x + lvl2ButtonRect.w) && mouseY >= lvl2ButtonRect.y && mouseY <= (lvl2ButtonRect.y + lvl2ButtonRect.h))
        {
            selectedLevel = 2;
            ResetGame();
            currentstate = GameState::PLAYING;
        }
        if (mouseX >= lvl3ButtonRect.x && mouseX <= (lvl3ButtonRect.x + lvl3ButtonRect.w) && mouseY >= lvl3ButtonRect.y && mouseY <= (lvl3ButtonRect.y + lvl3ButtonRect.h))
        {
            selectedLevel = 3;
            ResetGame();
            currentstate = GameState::PLAYING;
        }
        if (mouseX >= lvl4ButtonRect.x && mouseX <= (lvl4ButtonRect.x + lvl4ButtonRect.w) && mouseY >= lvl4ButtonRect.y && mouseY <= (lvl4ButtonRect.y + lvl4ButtonRect.h))
        {
            selectedLevel = 4;
            ResetGame();
            currentstate = GameState::PLAYING;
        }

        }
        if(e.type == SDL_EVENT_KEY_DOWN)
        {
            if(e.key.key == SDLK_ESCAPE)
            {
                currentstate = GameState::MENU;
            }
        }
    }

    else if(currentstate == GameState::PAUSED)
    {
        if(e.type == SDL_EVENT_KEY_DOWN)
        {
            if(e.key.key == SDLK_ESCAPE)
            {
                currentstate = GameState::PLAYING;
            }
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            //mouse coords
            float mouseX = e.button.x;
            float mouseY = e.button.y;

            //checks collision of mouse with resume button
            if (mouseX >= resumeButtonRect.x && mouseX <= (resumeButtonRect.x + resumeButtonRect.w) && mouseY >= resumeButtonRect.y && mouseY <= (resumeButtonRect.y + resumeButtonRect.h))
            {
                //if clicked, game loop starts
                currentstate = GameState::PLAYING;
            }

            //checks collision with "quit to menu" button
            if (mouseX >= quitToMenuButtonRect.x && mouseX <= (quitToMenuButtonRect.x + quitToMenuButtonRect.w) && mouseY >= quitToMenuButtonRect.y && mouseY <= (quitToMenuButtonRect.y + quitToMenuButtonRect.h))
            {
                //if clicked, you quit to menu
                currentstate = GameState::MENU;
            }
        }
    }

    else if(currentstate == GameState::LEVEL_COMPLETE)
    {
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            float mouseX = e.button.x;
            float mouseY = e.button.y;

            // Clicked "Next Level"
            if (mouseX >= nextLevelButtonRect.x && mouseX <= (nextLevelButtonRect.x + nextLevelButtonRect.w) && mouseY >= nextLevelButtonRect.y && mouseY <= (nextLevelButtonRect.y + nextLevelButtonRect.h))
            {
                if(selectedLevel < 4)
                {
                    selectedLevel++;
                } else
                {
                    selectedLevel = 1; //resets if youre on the final level
                }
                ResetGame();
                currentstate = GameState::PLAYING;
            }

            // Clicked "Quit To Menu"
            if (mouseX >= lcQuitToMenuButtonRect.x && mouseX <= (lcQuitToMenuButtonRect.x + lcQuitToMenuButtonRect.w) && mouseY >= lcQuitToMenuButtonRect.y && mouseY <= (lcQuitToMenuButtonRect.y + lcQuitToMenuButtonRect.h))
            {
                currentstate = GameState::MENU;
            }
        }
    }

    else if(currentstate == GameState::PLAYING)
    {
        if(e.type == SDL_EVENT_KEY_DOWN)
        {
        player->KeyPress(e,mRenderer);
        enemy->KeyPress(e, player,mRenderer);
        enemy2->KeyPress(e, player,mRenderer);
        enemy3->KeyPress(e, player,mRenderer);
        if(e.key.key == SDLK_Q ||e.key.key == SDLK_E){
            MIX_SetTrackAudio(mEffectTrack,gHighAudio);
            MIX_PlayTrack( mEffectTrack, 0 );
            cout << 1;
        }
        else if(e.key.key == SDLK_A ||e.key.key == SDLK_D){
            MIX_SetTrackAudio(mEffectTrack,gMediumAudio);
            MIX_PlayTrack( mEffectTrack, 0 );
            cout << 2;
        }
        else if(e.key.key == SDLK_S){
            MIX_SetTrackAudio(mEffectTrack,gLowAudio);
            MIX_PlayTrack( mEffectTrack, 0 );
            cout << 3;
        }
        else if(e.key.key == SDLK_W){
            MIX_SetTrackAudio(mEffectTrack,gScratchAudio);
            MIX_PlayTrack( mEffectTrack, 0 );
            cout << 4;
        }

        if(e.key.key == SDLK_9){
            if(MIX_TrackPlaying(mMusicTrack) == false &&  MIX_TrackPaused(mMusicTrack) == false){
                SDL_PropertiesID props = SDL_CreateProperties();
                SDL_SetNumberProperty( props, MIX_PROP_PLAY_LOOPS_NUMBER, -1 );
                MIX_PlayTrack( mMusicTrack, props );
                SDL_DestroyProperties( props );
                }
            else{
                if(MIX_TrackPaused(mMusicTrack) == true){
                    MIX_ResumeTrack(mMusicTrack);
                }
                else{
                    MIX_PauseTrack( mMusicTrack );
                    }
                }
            cout << 9;
            }
        if(e.key.key == SDLK_0){
            MIX_StopTrack(mMusicTrack, 0);
            cout << 0;
        }

        if(e.key.key == SDLK_ESCAPE) //if escape key pressed, pause the game
        {
            currentstate = GameState::PAUSED;
        }

        }
    }
    }

}

void Update(){

    SceneData();
    player->Update(mRenderer);
    enemy->Update(player,mRenderer);
    if(selectedLevel>=2)
        enemy2->Update(player, mRenderer);
    if(selectedLevel==4)
        enemy3->Update(player,mRenderer);

    bool levelBeaten = false;
        if(selectedLevel == 1 && enemy->health <= 0) {
            levelBeaten = true;
        }
        else if(selectedLevel == 2 && enemy->health <= 0 && enemy2->health <= 0) {
            levelBeaten = true;
        }
        else if(selectedLevel == 3 && enemy->health <= 0 && enemy2->health <= 0) {
            levelBeaten = true;
        }
        else if(selectedLevel == 4 && enemy->health <= 0 && enemy2->health <= 0 && enemy3->health <= 0) {
            levelBeaten = true;
        }

    if (levelBeaten)
        {
            levelCompleted[selectedLevel-1] = true; //makes the status of that level as completed
            SaveGame(); //saves it to our save game
            currentstate = GameState::LEVEL_COMPLETE;
        }
}

void Render(){
    // 1. Set background color based on the current game state
    if (currentstate == GameState::PLAYING) {
        // Black background in-game
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    } else {
        // White background for menus (MENU, LEVEL_SELECT, PAUSED)
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    // 2. Clear the screen with the color we just set
    SDL_RenderClear(mRenderer);

    // 3. Render the correct items based on the state
    if(currentstate == GameState::MENU)
    {
        playButtonText.Render(mRenderer, &playButtonRect);
        level1ButtonText.Render(mRenderer, &level1ButtonRect);
        quitButtonText.Render(mRenderer, &quitButtonRect);
    }

    else if(currentstate == GameState::LEVEL_SELECT)
    {
        lvl1Button.Render(mRenderer, nullptr, &lvl1ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
        lvl2Button.Render(mRenderer, nullptr, &lvl2ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
        lvl3Button.Render(mRenderer, nullptr, &lvl3ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
        lvl4Button.Render(mRenderer, nullptr, &lvl4ButtonRect,0.0,nullptr,SDL_FLIP_NONE);

        level1Text.Render(mRenderer, &level1TextRect);
        level2Text.Render(mRenderer, &level2TextRect);
        level3Text.Render(mRenderer, &level3TextRect);
        level4Text.Render(mRenderer, &level4TextRect);

        if (levelCompleted[0]) completeStatusText.Render(mRenderer, &complete1);
        if (levelCompleted[1]) completeStatusText.Render(mRenderer, &complete2);
        if (levelCompleted[2]) completeStatusText.Render(mRenderer, &complete3);
        if (levelCompleted[3]) completeStatusText.Render(mRenderer, &complete4);

        lsBackButtonText.Render(mRenderer, &lsBackButtonRect);
    }

    else if(currentstate == GameState::PAUSED)
    {
        resumeButtonText.Render(mRenderer, &resumeButtonRect);
        quitToMenuButtonText.Render(mRenderer, &quitToMenuButtonRect);
    }

    else if(currentstate == GameState::LEVEL_COMPLETE)
        {
            youWinText.Render(mRenderer, &youWinTextRect);
            nextLevelButtonText.Render(mRenderer, &nextLevelButtonRect);
            lcQuitToMenuButtonText.Render(mRenderer, &lcQuitToMenuButtonRect);
        }

    else if(currentstate == GameState::PLAYING)
    {
        // Render the background image FIRST
        if(selectedLevel == 1)
            lvl1Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
        else if(selectedLevel == 2)
            lvl2Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
        else if(selectedLevel == 3)
            lvl3Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
        else if(selectedLevel == 4)
            lvl4Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);

        // Render entities ON TOP of the background
        player->Render(mRenderer);
        enemy->Render(mRenderer);
        if(selectedLevel>=2)
            enemy2->Render(mRenderer);
        if(selectedLevel==4)
            enemy3->Render(mRenderer);
    }

    // 4. Actually draw everything to the screen
    SDL_RenderPresent(mRenderer);
}

void tick(){

    Input();

    if(currentstate == GameState::PLAYING)
    {
    Update();
    }

    Render();
}

void play(){
    while(running){
        tick();
        SDL_Delay(42);
    }
}

~SDL_Game(){
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);

    MIX_DestroyTrack(mMusicTrack);
    MIX_DestroyTrack(mEffectTrack);
    MIX_DestroyMixer(mMixer);

    MIX_DestroyAudio(gHighAudio);
    MIX_DestroyAudio(gMediumAudio);
    MIX_DestroyAudio(gLowAudio);
    MIX_DestroyAudio(gScratchAudio);

    SDL_Quit();
    TTF_Quit();
    MIX_Quit();
}

};

//graveyard

// void Render(){
//     // SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//     if (currentstate == GameState::PLAYING)
//     {
//         // Black background in-game
//         SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
//     }
//     else
//     {
//         // Light blue background for menus (MENU, LEVEL_SELECT, PAUSED)
//         SDL_SetRenderDrawColor(mRenderer, 135, 206, 235, 255);
//     }
//     SDL_RenderClear(mRenderer);
//     if(currentstate == GameState::MENU) //renders the menu when you're in the menu
//     {
//         playButtonText.Render(mRenderer, &playButtonRect);
//         level1ButtonText.Render(mRenderer, &level1ButtonRect);
//         quitButtonText.Render(mRenderer, &quitButtonRect);
//     }

//     if(currentstate == GameState::LEVEL_SELECT) // render level select
//     {
//         lvl1Button.Render(mRenderer, nullptr, &lvl1ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
//         lvl2Button.Render(mRenderer, nullptr, &lvl2ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
//         lvl3Button.Render(mRenderer, nullptr, &lvl3ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
//         lvl4Button.Render(mRenderer, nullptr, &lvl4ButtonRect,0.0,nullptr,SDL_FLIP_NONE);
//         level1Text.Render(mRenderer, &level1TextRect);
//         level2Text.Render(mRenderer, &level2TextRect);
//         level3Text.Render(mRenderer, &level3TextRect);
//         level4Text.Render(mRenderer, &level4TextRect);
//         lsBackButtonText.Render(mRenderer, &lsBackButtonRect);
//     }

//     if(currentstate == GameState::PAUSED) //renders the pause menu when you're paused
//     {
//         resumeButtonText.Render(mRenderer, &resumeButtonRect);
//         quitToMenuButtonText.Render(mRenderer, &quitToMenuButtonRect);
//     }

//     SDL_RenderClear(mRenderer);

//     if(currentstate == GameState::PLAYING) //starts the game loop if you're in game
//     {
//         //renders the level based on which one is selected
//         if(selectedLevel == 1)
//             lvl1Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
//         else if(selectedLevel == 2)
//             lvl2Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
//         else if(selectedLevel == 3)
//             lvl3Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);
//         else if(selectedLevel == 4)
//             lvl4Button.Render(mRenderer, nullptr, &bgRect, 0.0, nullptr, SDL_FLIP_NONE);

//         player->Render(mRenderer);
//         enemy->Render(mRenderer);
//     }

//     // enemy2->Render(mRenderer);
//     // enemy_2->Render(mRenderer);

//     SDL_RenderPresent(mRenderer);
// }
