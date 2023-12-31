#include "world.h"

typedef enum  {
	upRight,
	downRight,
	downLeft,
	upLeft
} Direction;

struct powerUp_type {
	SDL_Rect powerPos;
	Direction direction;
	PowerType powerType; //powerTypes in player.h 
	Uint32 startTimer;
};

//PRIVATE FUNCTION DECLARATIONS

//checks if powerUp collided with window limits
PRIVATE void powerUpWorldCollision(PowerUp aPowerUp, int screenWidth, int screenHeight);



//******************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************
//*********************************************************************************    WORLD    ********************************************************************************************************
//******************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************

void worldCollision(SDL_Rect* aPlayerPos, Player aPlayer, int screenWidth, int screenHeight) {
	int pixelX = 2;
	int pixelY = 13;
	int pixelW = 4;
	int pixelH = 2;
	if (getPlayerStatus(aPlayer) == true) {
		if (aPlayerPos->x + pixelX <= 0) {
			aPlayerPos->x = 0 - pixelX;
		}
		if ((aPlayerPos->x + aPlayerPos->w - pixelW) >= screenWidth) {
			aPlayerPos->x = screenWidth - aPlayerPos->w + pixelW;
		}
		if (aPlayerPos->y + pixelY <= 0) {
			aPlayerPos->y = 0 - pixelY;
		}
		if ((aPlayerPos->y + aPlayerPos->h - pixelH) >= screenHeight) {
			aPlayerPos->y = screenHeight - aPlayerPos->h + pixelH;
		}
	}
}

void scrollBackground(LoadMedia aMedia, int *aOffset, int w, int h) {
	(*aOffset) -= 1;
	if ((*aOffset) <= -w) {
		(*aOffset) = 0;
	}
	aMedia->scrollingBackground[0].x = (*aOffset);
	aMedia->scrollingBackground[0].y = 0;
	aMedia->scrollingBackground[0].w = w;
	aMedia->scrollingBackground[0].h = h;

	aMedia->scrollingBackground[1].x = (*aOffset) + w;
	aMedia->scrollingBackground[1].y = 0;
	aMedia->scrollingBackground[1].w = w;
	aMedia->scrollingBackground[1].h = h;
}

//******************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************
//*********************************************************************************    powerUps    *****************************************************************************************************
//******************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************

PUBLIC PowerUp initPowerUp(void) {
	PowerUp powerUp = malloc(sizeof(struct powerUp_type));;
	powerUp->powerPos.x = 0;
	powerUp->powerPos.y = 0;
	powerUp->powerPos.w = 0;
	powerUp->powerPos.h = 0;

	powerUp->powerType = none;
	powerUp->direction = 0;
	powerUp->startTimer = 0;

	return powerUp;
}


PUBLIC PowerUp serverSpawnPowerUp(int screenWidth, int screenHeight, PowerUp oldPowerUp) {
	PowerUp powerUp = realloc(oldPowerUp, sizeof(struct powerUp_type));
	int rndX = (rand() % screenWidth - 0) + 1;
	int rndY = (rand() % screenHeight - 0) + 1;
	int rndType = (rand() % 4 - 0);
	int rndDir = (rand() % 4 - 0);

	powerUp->powerPos.x = rndX;
	powerUp->powerPos.y = rndY;
	powerUp->powerPos.w = 64;
	powerUp->powerPos.h = 64;

	powerUp->powerType = rndType;
	powerUp->direction = rndDir;
	
	powerUp->startTimer = SDL_GetTicks();

	return powerUp;
}

PUBLIC PowerUp clientSpawnPowerUp(SDL_Rect powerUpRect, int powerUpDir, int powerUpType) {
	PowerUp newPowerUp = malloc(sizeof(struct powerUp_type));
	newPowerUp->powerPos = powerUpRect;
	newPowerUp->powerType = powerUpType;
	newPowerUp->direction = powerUpDir;

	return newPowerUp;
}

PUBLIC SDL_Rect getPowerUpRect(PowerUp aPowerUp) {
	return aPowerUp->powerPos;
}

PUBLIC int getPowerUpDir(PowerUp aPowerUp) {
	return aPowerUp->direction;
}

PUBLIC int getPowerUpType(PowerUp aPowerUp) {
	return aPowerUp->powerType;
}

PUBLIC void setPowerUpTimer(PowerUp aPowerUp, Uint32 startTimer) {
	aPowerUp->startTimer = startTimer;
}

PUBLIC Uint32 getPowerUpTimer(PowerUp aPowerUp) {
	return aPowerUp->startTimer;
}


PRIVATE void powerUpWorldCollision(PowerUp aPowerUp, int screenWidth, int screenHeight) {
	int pixelX = 1;
	int pixelY = 1;
	int pixelW = 1;
	int pixelH = 1;

	//changes direction if wall is hit
	if (aPowerUp->powerPos.x + pixelX <= 0) {
		switch (aPowerUp->direction) {
		case downLeft:
			aPowerUp->direction = downRight;
			break;
		case upLeft:
			aPowerUp->direction = upRight;
			break;
		}
	}
	else if ((aPowerUp->powerPos.x + aPowerUp->powerPos.w - pixelW) >= screenWidth)
	{
		switch (aPowerUp->direction) {
		case downRight:
			aPowerUp->direction = downLeft;
			break;
		case upRight:
			aPowerUp->direction = upLeft;
			break;
		}
	}
	else if (aPowerUp->powerPos.y + pixelY <= 0)
	{
		switch (aPowerUp->direction) {
		case upRight:
			aPowerUp->direction = downRight;
			break;
		case upLeft:
			aPowerUp->direction = downLeft;
			break;
		}
	}
	else if ((aPowerUp->powerPos.y + aPowerUp->powerPos.h - pixelH) >= screenHeight)
	{
		switch (aPowerUp->direction) {
		case downLeft:
			aPowerUp->direction = upLeft;
			break;
		case downRight:
			aPowerUp->direction = upRight;
			break;
		}
	}
}


PUBLIC void powerUpTick(PowerUp aPowerUp, int screenWidth, int screenHeight) {

	powerUpWorldCollision(aPowerUp, screenWidth, screenHeight);

	switch (aPowerUp->direction) {
	case upRight:
		aPowerUp->powerPos.y -= POWERUP_SPEED;
		aPowerUp->powerPos.x += POWERUP_SPEED;
		break;
	case downRight:
		aPowerUp->powerPos.y += POWERUP_SPEED;
		aPowerUp->powerPos.x += POWERUP_SPEED;
		break;
	case upLeft:
		aPowerUp->powerPos.y -= POWERUP_SPEED;
		aPowerUp->powerPos.x -= POWERUP_SPEED;
		break;
	case downLeft:
		aPowerUp->powerPos.y += POWERUP_SPEED;
		aPowerUp->powerPos.x -= POWERUP_SPEED;
		break;
	}
}

PUBLIC void renderPowerUp(SDL_Renderer* renderer, PowerUp aPowerUp, LoadMedia media, int *coinFrames) {

	switch (aPowerUp->powerType) {
	case life:
		SDL_RenderCopy(renderer, media->PowerUpTex[0], NULL, &aPowerUp->powerPos);
		break;
	case shield:
		SDL_RenderCopy(renderer, media->PowerUpTex[1], NULL, &aPowerUp->powerPos);
		break;
	case attack:
		SDL_RenderCopy(renderer, media->PowerUpTex[2], NULL, &aPowerUp->powerPos);
		break;
	case coin:
		SDL_RenderCopyEx(renderer, media->coinTex, &media->coinSprites[(*coinFrames) / COIN_FRAMES], &aPowerUp->powerPos, 0, NULL, SDL_FLIP_NONE);
		(*coinFrames)++;
		if ((*coinFrames) / 6 == COIN_FRAMES) {
			(*coinFrames) = 0;
		}
		break;
	}
}


PUBLIC int powerUpConsumed(Player playerList[], PowerUp aPowerUp, int playerCount, Uint32 *powerDurationTimer) {
	for (int i = 0; i < playerCount; i++) {
		if (SDL_HasIntersection(getPlayerPosAdr(playerList[i]), &aPowerUp->powerPos) && (aPowerUp->powerType != none) && getPlayerStatus(playerList[i])) {
			setPlayerPower(playerList[i], aPowerUp->powerType);
			if (aPowerUp->powerType == shield) {
				*powerDurationTimer = SDL_GetTicks();
			}
			aPowerUp->powerType = none;
			return 1;
		}
	}
	return 0;
}

PUBLIC bool powerUpExpired(PowerUp aPowerUp) {
	if (SDL_GetTicks() >= aPowerUp->startTimer + POWERUP_EXPIRED && aPowerUp->powerType != none) {
		aPowerUp->powerType = none;
		aPowerUp->startTimer = SDL_GetTicks() + POWERUP_EXPIRED; // felhantering s� en ny powerUp hinner skapas
		return true;
	}
	else {
		return false;
	}
}

PUBLIC bool singelPlayerPowers(Game_Route* aGameRoute, PowerUp aPowerUp) {
	if ((*aGameRoute == singlePlayerRoute && aPowerUp->powerType == attack) || (*aGameRoute == singlePlayerRoute && aPowerUp->powerType == shield)) {
		aPowerUp->powerType = none;
		return true;
	}
	return false;
}