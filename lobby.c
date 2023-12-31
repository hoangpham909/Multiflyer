#include "lobby.h"

#pragma warning( disable : 6385 )

struct lobby_type {
	char lobbyText[NAME_LENGTH];
	char startGame[NAME_LENGTH];
	char waitingForHost[NAME_LENGTH];
	char slots[4][8];
	char playerNames[4][NAME_LENGTH];
	int playerCount;
	bool renderText;

	SDL_Texture* textures[TEXTS];
	SDL_Texture* startGameTexture;
	SDL_Texture* waitingForHostTex;
	SDL_Rect rects[TEXTS];
	SDL_Rect startGameRect;
	SDL_Rect waitingForHostRect;
	SDL_Event event;

	SDL_Color lobbyTextColor;
	SDL_Color playerListColor;

	SDL_Surface* surfaces[11];
};


PRIVATE void renderLobby(SDL_Renderer* renderer, bool hostOrClient, Lobby aLobby);
PRIVATE void closeLobbyTTF(Lobby aLobby);
PRIVATE Lobby createLobby(SDL_Renderer* renderer, Fonts fonts);
PRIVATE void playerJoined(SDL_Renderer* renderer, Lobby aLobby, Fonts fonts, char name[]);


// ************************************************** HOST CODE ******************************************************************************************************
PUBLIC int hostLobby(SDL_Renderer* renderer, char playerName[], Game_State current, Network_Config setup, Fonts fonts, Game_Route* aGameroute) {

	Lobby hostLobby;
	hostLobby = createLobby(renderer, fonts);
	int x, y;
	SDL_Color selected = { 77 , 255, 0, 0 };
	TCP_Communication communication = malloc(sizeof(struct TCP_Communication_Type));
	initGamestate(current);
	initTCPCom(communication);

	//initiates with name
	strcpy(current->playerNames[current->nrOfPlayers], playerName);
	current->nrOfPlayers++;
	current->localPlayerNr++;
	current->lobbyRunningFlag = 1;
	playerJoined(renderer, hostLobby, fonts, playerName);

	SDL_CreateThread(serverLobbyConnection, "Connection_Thread", current);


	while (current->lobbyRunningFlag) {


		while (SDL_PollEvent(&hostLobby->event))
		{
			if (hostLobby->event.type == SDL_QUIT)
			{
				*aGameroute = quitRoute;
				closeLobbyTTF(hostLobby);
				closeServer(current, communication, setup);
				return 0;
			}
			else if (hostLobby->event.type == SDL_KEYDOWN && hostLobby->event.key.keysym.sym == SDLK_ESCAPE) 
			{
				*aGameroute = menuRoute;
				closeLobbyTTF(hostLobby);

				closeServer(current, communication, setup);
				return 0;
			}
			else if (hostLobby->event.type == SDL_MOUSEMOTION) {
				x = hostLobby->event.motion.x;
				y = hostLobby->event.motion.y;
				if (x >= hostLobby->startGameRect.x && x <= hostLobby->startGameRect.x + hostLobby->startGameRect.w && y > hostLobby->startGameRect.y && y <= hostLobby->startGameRect.y + hostLobby->startGameRect.h)
				{
					SDL_DestroyTexture(hostLobby->startGameTexture);
					SDL_Surface* temp = TTF_RenderText_Solid(fonts->ka1_60, hostLobby->startGame, selected);
					hostLobby->startGameTexture = SDL_CreateTextureFromSurface(renderer, temp);
					SDL_FreeSurface(temp);
					hostLobby->renderText = true;
				}
				else
				{
					SDL_DestroyTexture(hostLobby->startGameTexture);
					SDL_Surface* temp = TTF_RenderText_Solid(fonts->ka1_60, hostLobby->startGame, hostLobby->lobbyTextColor);
					hostLobby->startGameTexture = SDL_CreateTextureFromSurface(renderer, temp);
					SDL_FreeSurface(temp);
					hostLobby->renderText = true;
				}
			}
			else if (hostLobby->event.type == SDL_MOUSEBUTTONDOWN)
			{
				x = hostLobby->event.button.x;
				y = hostLobby->event.button.y;
				if (x >= hostLobby->startGameRect.x && x <= hostLobby->startGameRect.x + hostLobby->startGameRect.w && y > hostLobby->startGameRect.y && y <= hostLobby->startGameRect.y + hostLobby->startGameRect.h)
				{

					for (int i = 0; current->nrOfPlayers - 1 > i; i++){
						communication->startGame = 1;
						sendToClient(communication, setup->playerIp[i], current);
					}
					closeLobbyTTF(hostLobby);
					current->lobbyRunningFlag = 0;
					return 1;
				}
			}
		}
		if (current->newPlayerFlag) {
			playerJoined(renderer, hostLobby, fonts, current->playerNames[current->nrOfPlayers - 1]);
			strncpy(setup->playerIp[current->nrOfPlayers - 2], current->ipAdressCache, IP_LENGTH);
		

			for (int i = 0; current->nrOfPlayers - 2 > i; i++) {
				strncpy(communication->playerName, current->playerNames[current->nrOfPlayers - 1], NAME_LENGTH);
				sendToClient(communication, setup->playerIp[i], current);
			}

			current->newPlayerFlag = 0;
		}

		if (current->disconnectionCache != 0 && current->nrOfPlayers>1){
			removePlayerFromLobby(current, setup, current->disconnectionCache);
			hostLobby->playerCount = 0;
			
			for (int i = 0; current->nrOfPlayers+1 > i; i++) {
				playerJoined(renderer, hostLobby, fonts, current->playerNames[i]);
			}

			hostLobby->playerCount = hostLobby->playerCount - 1;
			communication->localPlayerNr = current->disconnectionCache;
			communication->leftGame = 1;

			for (int i = 0; current->nrOfPlayers - 1 > i; i++) {
				sendToClient(communication, setup->playerIp[i], current);
			}
			
			communication->leftGame = 0;
			current->disconnectionCache = 0;

		}

		if (hostLobby->renderText) {
			renderLobby(renderer, true, hostLobby);
		}
	}

	printf("lobby crashed");
	return 0;
}



// ************************************************** CLIENT CODE ****************************************************************************************************
PUBLIC int clientLobby(SDL_Renderer* renderer, char playerName[], char playerIp[], Game_State current, Fonts fonts, Game_Route* aGameroute) {

	Lobby clientLobby;
	clientLobby = createLobby(renderer, fonts);
	Uint32 loadingCounter = SDL_GetTicks();
	int loadingDots = 3;
	initGamestate(current);

	//initiates with name
	strcpy(current->ipAdressCache , playerIp);

	if (clientLobbyConnection(playerIp, playerName, current)) {
		*aGameroute = menuRoute;
		closeLobbyTTF(clientLobby);
		current->nrOfPlayers = 0;
		printf("Couldn't connect to server\n");
		return 0;
	}

	for (int i = 0; current->nrOfPlayers > i; i++) {
		playerJoined(renderer, clientLobby, fonts, current->playerNames[i]);
	}

	SDL_CreateThread(clientLobbyWait, "Client_Wait_Thread", current);

	while (current->lobbyRunningFlag) {

		//makes dots on waiting for host count down
		if (SDL_GetTicks() >= loadingCounter + 1000) {
			clientLobby->waitingForHost[strlen(clientLobby->waitingForHost) - 1] = '\0';
			if (loadingDots <= 0) {
				strcpy(clientLobby->waitingForHost, "Waiting for host...");
				loadingDots = 3;
			}
			else {
				loadingDots--;
			}
			SDL_DestroyTexture(clientLobby->waitingForHostTex);
			SDL_Surface* temp = TTF_RenderText_Solid(fonts->cuvert_48, clientLobby->waitingForHost, clientLobby->lobbyTextColor);
			clientLobby->waitingForHostTex = SDL_CreateTextureFromSurface(renderer, temp);
			SDL_QueryTexture(clientLobby->waitingForHostTex, NULL, NULL, &clientLobby->waitingForHostRect.w, &clientLobby->waitingForHostRect.h);
			SDL_FreeSurface(temp);

			clientLobby->renderText = true;
			loadingCounter = SDL_GetTicks();
		}

		while (SDL_PollEvent(&clientLobby->event))
		{
			if (clientLobby->event.type == SDL_QUIT)
			{
				*aGameroute = quitRoute;
				closeLobbyTTF(clientLobby);
				disconnectFromServer(playerIp, current);
				return 0;
			}
			else if(clientLobby->event.type == SDL_KEYDOWN && clientLobby->event.key.keysym.sym == SDLK_ESCAPE)
			{
				*aGameroute = menuRoute;
				closeLobbyTTF(clientLobby);
				disconnectFromServer(playerIp, current);
				return 0;
			}
		}
		if (current->newPlayerFlag) {
			playerJoined(renderer, clientLobby, fonts, current->playerNames[current->nrOfPlayers - 1]);
			current->newPlayerFlag = 0;
		}

		if (current->disconnectionCache != 0 && current->nrOfPlayers > 1) {
			removePlayerFromLobby(current, NULL, current->disconnectionCache);
			clientLobby->playerCount = 0;

			for (int i = 0; current->nrOfPlayers + 1 > i; i++) {
				playerJoined(renderer, clientLobby, fonts, current->playerNames[i]);
			}

			clientLobby->playerCount = clientLobby->playerCount - 1;
			if (current->disconnectionCache < current->localPlayerNr) {
				current->localPlayerNr = current->localPlayerNr - 1;
			}
			current->disconnectionCache = 0;
			
		}

		if (current->serverConnection) {
			*aGameroute = menuRoute;
			closeLobbyTTF(clientLobby);
			current->lobbyRunningFlag = 0;
			current->nrOfPlayers = 0;
			printf("Host disconnected\n");
			return 0;
		}

		if (clientLobby->renderText) {
			renderLobby(renderer, false, clientLobby);
		}
	}

	closeLobbyTTF(clientLobby);
	return 1;
}




// ************************************************** PRIVATE LOBBY FUNCTIONS *****************************************************************************************
PRIVATE Lobby createLobby(SDL_Renderer* renderer, Fonts fonts) {
	Lobby newLobby = malloc(sizeof(struct lobby_type));

	strcpy(newLobby->lobbyText, "Lobby");
	strcpy(newLobby->startGame, "Start Game");
	strcpy(newLobby->waitingForHost, "Waiting for host...");
	strcpy(newLobby->slots[0], "slot 1:");
	strcpy(newLobby->slots[1], "slot 2:");
	strcpy(newLobby->slots[2], "slot 3:");
	strcpy(newLobby->slots[3], "slot 4:");

	strcpy(newLobby->playerNames[0], "*empty*");
	strcpy(newLobby->playerNames[1], "*empty*");
	strcpy(newLobby->playerNames[2], "*empty*");
	strcpy(newLobby->playerNames[3], "*empty*");

	newLobby->renderText = true;
	newLobby->playerCount = 0;

	newLobby->lobbyTextColor.r = 255;
	newLobby->lobbyTextColor.g = 255;
	newLobby->lobbyTextColor.b = 255;
	newLobby->lobbyTextColor.a = 0;

	newLobby->playerListColor.r = 160;
	newLobby->playerListColor.g = 237;
	newLobby->playerListColor.b = 189;
	newLobby->playerListColor.a = 0;

	//create surfaces from TTF
	newLobby->surfaces[0] = TTF_RenderText_Solid(fonts->cuvert_48, newLobby->lobbyText, newLobby->lobbyTextColor);

	for (int i = 1; i < 5; i++)
		newLobby->surfaces[i] = TTF_RenderText_Solid(fonts->cuvert_28, newLobby->slots[i - 1], newLobby->playerListColor);

	for (int i = 5; i < 9; i++)
		newLobby->surfaces[i] = TTF_RenderText_Solid(fonts->cuvert_28, newLobby->playerNames[i - 5], newLobby->playerListColor);

	newLobby->surfaces[9] = TTF_RenderText_Solid(fonts->ka1_60, newLobby->startGame, newLobby->lobbyTextColor);
	newLobby->surfaces[10] = TTF_RenderText_Solid(fonts->cuvert_48, newLobby->waitingForHost, newLobby->lobbyTextColor);

	//create textures from surfaces
	for (int i = 0; i < 9; i++)
		newLobby->textures[i] = SDL_CreateTextureFromSurface(renderer, newLobby->surfaces[i]);

	newLobby->startGameTexture = SDL_CreateTextureFromSurface(renderer, newLobby->surfaces[9]);
	newLobby->waitingForHostTex = SDL_CreateTextureFromSurface(renderer, newLobby->surfaces[10]);

	//destroy unnessesary surfaces 
	for (int i = 0; i < 11; i++)
		SDL_FreeSurface(newLobby->surfaces[i]);

	//lobby rect
	newLobby->rects[0].x = 390;
	newLobby->rects[0].y = 10;

	//slot rects
	newLobby->rects[1].x = 100;
	newLobby->rects[1].y = 100;
	newLobby->rects[2].x = 100;
	newLobby->rects[2].y = 200;
	newLobby->rects[3].x = 100;
	newLobby->rects[3].y = 300;
	newLobby->rects[4].x = 100;
	newLobby->rects[4].y = 400;

	//playerlist rects
	newLobby->rects[5].x = 280;
	newLobby->rects[5].y = 100;
	newLobby->rects[6].x = 280;
	newLobby->rects[6].y = 200;
	newLobby->rects[7].x = 280;
	newLobby->rects[7].y = 300;
	newLobby->rects[8].x = 280;
	newLobby->rects[8].y = 400;


	newLobby->startGameRect.x = 250;
	newLobby->startGameRect.y = 490;
	SDL_QueryTexture(newLobby->startGameTexture, NULL, NULL, &newLobby->startGameRect.w, &newLobby->startGameRect.h);

	newLobby->waitingForHostRect.x = 230;
	newLobby->waitingForHostRect.y = 490;
	SDL_QueryTexture(newLobby->waitingForHostTex, NULL, NULL, &newLobby->waitingForHostRect.w, &newLobby->waitingForHostRect.h);

	for (int i = 0; i < TEXTS; i++) {
		SDL_QueryTexture(newLobby->textures[i], NULL, NULL, &newLobby->rects[i].w, &newLobby->rects[i].h);
	}

	return newLobby;
}




PRIVATE void renderLobby(SDL_Renderer* renderer, bool hostOrClient, Lobby aLobby) {
	SDL_RenderClear(renderer);

	if (hostOrClient) {
		SDL_RenderCopy(renderer, aLobby->startGameTexture, NULL, &aLobby->startGameRect);
	}
	else {
		SDL_RenderCopy(renderer, aLobby->waitingForHostTex, NULL, &aLobby->waitingForHostRect);
	}

	for (int i = 0; i < TEXTS; i++) {
		SDL_RenderCopy(renderer, aLobby->textures[i], NULL, &aLobby->rects[i]);
	}

	SDL_RenderPresent(renderer);
	aLobby->renderText = false;
}



PRIVATE void closeLobbyTTF(Lobby aLobby) {
	for (int i = 0; i < TEXTS; i++)
		SDL_DestroyTexture(aLobby->textures[i]);
	SDL_DestroyTexture(aLobby->startGameTexture);
	free(aLobby);
}


PRIVATE void playerJoined(SDL_Renderer* renderer, Lobby aLobby, Fonts fonts, char name[]) {
	aLobby->playerCount++;
	SDL_DestroyTexture(aLobby->textures[4 + (aLobby->playerCount)]);
	SDL_Surface* temp = TTF_RenderText_Solid(fonts->cuvert_28, name, aLobby->playerListColor);
	aLobby->textures[4 + (aLobby->playerCount)] = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_QueryTexture(aLobby->textures[4 + (aLobby->playerCount)], NULL, NULL, &aLobby->rects[4 + (aLobby->playerCount)].w, &aLobby->rects[4 + (aLobby->playerCount)].h);
	SDL_FreeSurface(temp);
	aLobby->renderText = true;
}