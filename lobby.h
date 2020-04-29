#ifndef lobby_h
#define lobby_h

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"
#include "constants.h"

#define TEXTS 9

typedef struct lobby_type* Lobby;

//hosts a lobby and inserts player name in lobby waiting list
PUBLIC int hostLobby(SDL_Renderer* renderer, char playerName[]);

//starts the client side of lobby, sends playername and ip for connections to host. 
PUBLIC int clientLobby(SDL_Renderer* renderer, char playerName[], char playerIp[]);

#endif /*lobby_h*/