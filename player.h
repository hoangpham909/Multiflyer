#ifndef player_h
#define player_h

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "constants.h"
#include "loadMedia.h"

//en ADT f�r att skapa en spelare

typedef struct playerType* Player;

//creates a player struct
PUBLIC Player createPlayer(int x, int y);

//renders player dead or alive
PUBLIC void renderPlayers(SDL_Renderer* renderer, Player playerList[], int playerFrame, int splashFrame, int* nrOfSoundEffects, int playerCount, LoadMedia media);

//creates a new player and adds to the list of players
PUBLIC void initPlayers(Player playerList[], int playerCount);

//returns adress of player position 
PUBLIC SDL_Rect* getPlayerPosAdr(Player aPlayer);

//returns player players status false: dead true: alive
PUBLIC bool getPlayerStatus(Player aPLayer);

//sets the status of player false: dead true: alive
PUBLIC void setPlayerStatus(Player aPlayer, bool deadOrAlive);

//sets cordinates of a players x:y:w:h, value represents the value to be set
PUBLIC void setPlayerPoint(Player aPlayer, char cord, int value);

//returns the value of player cordinate. cord: x:y:w:h
PUBLIC int getPlayerPoint(Player aPlayer, char cord);

//moves a player
PUBLIC void movePlayerUp(Player aPlayer, int speed);
PUBLIC void movePlayerDown(Player aPlayer, int speed);
PUBLIC void movePlayerLeft(Player aPlayer, int speed);
PUBLIC void movePlayerRight(Player aPlayer, int speed);

//frees player structs from heap, params: list of players and amount
PUBLIC void freePlayers(Player playerList[], int playerCount);

#endif /* player_h */