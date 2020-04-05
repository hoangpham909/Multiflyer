#include "game_engine.h"
#include "player.h"
#include "obstacle.h"

#define nrOfFrames 3 //Antal frames i spritesheet
#define TIME_DELAY 200



bool loadMedia(SDL_Renderer* renderer);

static SDL_Texture* flyTrapTex = NULL;
static SDL_Surface* flyTrapSurface = NULL;
static SDL_Texture* flyTex = NULL;
static SDL_Surface* flySurface = NULL;
static SDL_Rect spriteClips[nrOfFrames];


bool startGame(SDL_Renderer* renderer, int w, int h) {

    int frame = 0; //Den frame som ska visas
    int delay = TIME_DELAY;

    // struct to hold the position and size of the sprite
    Obstacle obstacles = createObstacle(w, h); //innitate start node

    Player player = createPlayer(50, 50);
    SDL_Rect playerPos = { getPlayerPositionX(player), getPlayerPositionY(player), getPlayerHeight(player), getPlayerWidth(player) };

    //Create Envoirment
    bool running = true;
    SDL_Event event;

    //Laddar in spritesheet och skapar de olika frames:en f�r spritesheetet
    if (!loadMedia(renderer)) {
        running = false; //Spelet st�ngs ner ifall det skett ett error i loadMedia
    }

    //Starting game engine
    while (running)
    {
        //polling events
        while (SDL_PollEvent(&event)) //N�r n�got h�nder
        {
            switch (event.type)
            {
            case SDL_QUIT: //Om du trycker p� X:et
                running = false;
                break;
            case SDL_KEYDOWN: //Trycker p� en knapp
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    playerPos.y -= 5;
                    break;
                case SDLK_DOWN:
                    playerPos.y += 5;
                    break;
                case SDLK_LEFT:
                    playerPos.x -= 5;
                    break;
                case SDLK_RIGHT:
                    playerPos.x += 5;
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
                break;
            }
        }
        //updating positions,inputs,multiplayer sends and receives.

        //Uppdaterar frames:en, kodblocket skapar en liten delay i bytet mellan frames:en
        frame++;
        if (frame / 3 == nrOfFrames) {
            frame = 0;
        }

        //Creates a new obstacle every time delay hits 0
        delay--;
        if (delay <= 0) {
            newObstacle(obstacles, w, h);
            delay = TIME_DELAY;
        }

        //advance obstacles
        obsteclesTick(obstacles);

        //free used obstacles
        if (destroyObstacle(obstacles)) {
            printf("destroyed\n");
        }

        // clear the window and render updates
        SDL_RenderClear(renderer);
        renderObstacles(obstacles, renderer, flyTrapTex);
        SDL_RenderCopyEx(renderer, flyTex, &spriteClips[frame / 3], &playerPos, 0, NULL, SDL_FLIP_NONE); //Visar spriten
        SDL_RenderPresent(renderer);
    }
    return true;
}

bool loadMedia(SDL_Renderer* renderer) {
    bool noError = true;
    flySurface = IMG_Load("bilder/flySpriteSheet.png"); //Laddar in spritesheet
    flyTrapSurface = IMG_Load("bilder/flytrapSpriteSheet.png"); //Laddar in spritesheet
    if (flySurface == NULL) {
        printf("Unable to load image. Error: %s", SDL_GetError());  //Kollar efter error vid IMG_Load
        noError = false;
    }
    else if  (flyTrapSurface == NULL) {
        printf("Unable to load image. Error: %s", SDL_GetError());  //Kollar efter error vid IMG_Load
        noError = false;
        }
    else {
        flyTex = SDL_CreateTextureFromSurface(renderer, flySurface); //skapar en texture fr�n spritesheet
        flyTrapTex = SDL_CreateTextureFromSurface(renderer, flyTrapSurface);
        if (flyTex == NULL)
        {
            printf("Unable to create texture from surface. Error: %s", SDL_GetError()); //Kollar efter error vid SDL_CreateTextureFromSurface
            noError = false;
        }
        else if (flyTrapTex == NULL) {
            printf("Unable to create texture from surface. Error: %s", SDL_GetError()); //Kollar efter error vid SDL_CreateTextureFromSurface
            noError = false;
        }
        else {
            spriteClips[0].x = 0;   //Skapar de olika rect f�r frames:en i spritesheet, v�r spritesheet har 3 frames d�rav 3 rects
            spriteClips[0].y = 0;
            spriteClips[0].w = 117;
            spriteClips[0].h = 150;

            spriteClips[1].x = 117;
            spriteClips[1].y = 0;
            spriteClips[1].w = 117;
            spriteClips[1].h = 150;

            spriteClips[2].x = 234;
            spriteClips[2].y = 0;
            spriteClips[2].w = 117;
            spriteClips[2].h = 150;
        }
    }
    return noError; //Returnerar errorchecken
}