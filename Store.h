#ifndef STORE_H
#define STORE_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <SDL2/SDL_mixer.h>
#include "loadMedia.h"
#include "constants.h"
#include "player.h"
#include "Network.h"

#define OPTION_STORE 4
#define STORE_SURFACES 9

//struct for store
struct store
{
    bool done;
    bool renderText;
    bool purchasedSkin[OPTION_STORE];
    SkinChoices skinChoice;

    int playerCoins;
    int selectedRect;

    char coins[NAME_LENGTH];
    char headLine[NAME_LENGTH];
    char bank[NAME_LENGTH];
    char purchased[NAME_LENGTH];
    char backToMenu[NAME_LENGTH];
    char price[OPTION_STORE][NAME_LENGTH];

    SDL_Surface* surfaces[STORE_SURFACES];
    SDL_Texture* coins_Tex;
    SDL_Texture* headLine_Tex;
    SDL_Texture* bank_Tex;
    SDL_Texture* purchased_Tex;
    SDL_Texture* backToMenu_Tex;
    SDL_Texture* price_Tex[OPTION_STORE];

    SDL_Rect coins_Rect;
    SDL_Rect headLine_Rect;
    SDL_Rect bank_Rect;
	SDL_Rect purchased_Rect;
    SDL_Rect backToMenu_Rect;
    SDL_Rect price_Rect[OPTION_STORE];
    SDL_Rect skins[OPTION_STORE];
    SDL_Rect skinBackgroundRect[OPTION_STORE];

    SDL_Color white;
    SDL_Color green;
};
typedef struct store* Store;

//Initiation of store
void initStore(Store status);

//Store function
void store(SDL_Renderer* renderer, LoadMedia media, Fonts fonts, Store status);

#endif // !STORE_H