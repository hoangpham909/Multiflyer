#include "LoadMenu.h"


struct menu {
    char menuChoices[NUM_MENU][NAME_LENGTH];
    SDL_Color color;
    SDL_Surface* menuSurface[NUM_MENU];
    SDL_Rect pos[NUM_MENU];
    SDL_Texture* textures[NUM_MENU];
};
typedef struct menu Menu;

PRIVATE Menu createMenu(SDL_Renderer* renderer, Fonts fonts);

int LoadMenu(SDL_Renderer* renderer, SDL_Window* window, int w, int h, bool* hostOrClient, char name[], char ip[], LoadMedia media, Fonts fonts, Game_State current, UDP_Client_Config setup)
{
    //Initalize for loading image
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    Mix_PlayMusic(media->menuMusic, -1); //Plays background music

    Menu newMenu1;
    newMenu1 = createMenu(renderer, fonts);
    bool running = true;
    int x, y;
    SDL_Color selected = { 77 , 255, 0, 0 };
    SDL_Event event;


    //SDL_Event event;
    while (running)
    {
        // Process events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEMOTION) {
                x = event.motion.x;
                y = event.motion.y;
                for (int i = 0; i < NUM_MENU; i++)
                {
                    // if focus change text to green
                    if (x >= newMenu1.pos[i].x && x <= newMenu1.pos[i].x + newMenu1.pos[i].w && y > newMenu1.pos[i].y && y <= newMenu1.pos[i].y + newMenu1.pos[i].h)
                    {
                        SDL_DestroyTexture(newMenu1.textures[i]);
                        SDL_Surface* temp = TTF_RenderText_Solid(fonts->magical_45, newMenu1.menuChoices[i], selected);
                        newMenu1.textures[i] = SDL_CreateTextureFromSurface(renderer, temp);
                        SDL_FreeSurface(temp);
                    }
                    else
                    {
                        SDL_DestroyTexture(newMenu1.textures[i]);
                        SDL_Surface* temp = TTF_RenderText_Solid(fonts->magical_45, newMenu1.menuChoices[i], newMenu1.color);
                        newMenu1.textures[i] = SDL_CreateTextureFromSurface(renderer, temp);
                        SDL_FreeSurface(temp);
                    }
                }
            }
            // if click!
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                x = event.button.x;
                y = event.button.y;
                // Quit
                if (x >= newMenu1.pos[3].x && x <= newMenu1.pos[3].x + newMenu1.pos[3].w && y > newMenu1.pos[3].y && y <= newMenu1.pos[3].y + newMenu1.pos[3].h)
                {
                    running = false;
                }
                //Controls
                else if (x >= newMenu1.pos[2].x && x <= newMenu1.pos[2].x + newMenu1.pos[2].w && y > newMenu1.pos[2].y && y <= newMenu1.pos[2].y + newMenu1.pos[2].h)
                {
                    control(renderer, media);
                }
                //Multiplayer
                else if (x >= newMenu1.pos[1].x && x <= newMenu1.pos[1].x + newMenu1.pos[1].w && y > newMenu1.pos[1].y && y <= newMenu1.pos[1].y + newMenu1.pos[1].h)
                {
                    getHostOrClient(renderer, media, hostOrClient);
                    if (*hostOrClient) {
                        enterName(renderer, media, fonts, name);
                        if (hostLobby(renderer, name, current, setup, fonts)) {
                            running = false;
                            return 1;
                        }
                    }
                    else {
                        enterName(renderer, media, fonts, name);
                        enterIp(renderer, media, fonts, ip);
                        if (clientLobby(renderer, name, ip, current, fonts)) {
                            running = false;
                            return 1;
                        }
                    }
                }
                //Start
                else if (x >= newMenu1.pos[0].x && x <= newMenu1.pos[0].x + newMenu1.pos[0].w && y > newMenu1.pos[0].y && y <= newMenu1.pos[0].y + newMenu1.pos[0].h)
                {
                    current->nrOfPlayers = 1; //needs to be set as one for singleplayer game
                    running = false;
                    return 1;
                }
            }
        }
        // Clear screen
        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, media->menuBackgroundTexture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
        //Draw
        for (int i = 0; i < NUM_MENU; i++)
            SDL_RenderCopy(renderer, newMenu1.textures[i], NULL, &newMenu1.pos[i]);
        //show what was drawn
        SDL_RenderPresent(renderer);
    }

    //Release resourse
    for (int i = 0; i < NUM_MENU; i++)
    {
        SDL_DestroyTexture(newMenu1.textures[i]);
    }
    return 0;
}



//**************************************************************************************** MENU *********************************************************************
PRIVATE Menu createMenu(SDL_Renderer* renderer, Fonts fonts)
{
    // Init Menu choices 
    Menu newMenu;
    strcpy(newMenu.menuChoices[0], "Start");
    strcpy(newMenu.menuChoices[1], "Multiplayer");
    strcpy(newMenu.menuChoices[2], "Controls");
    strcpy(newMenu.menuChoices[3], "Quit");

    // Set text color to white 
    newMenu.color.r = 0;
    newMenu.color.g = 0;
    newMenu.color.b = 0;
    newMenu.color.a = 0;

    // Render text, create texture and realease resources 
    for (int i = 0; i < NUM_MENU; i++)
        newMenu.menuSurface[i] = TTF_RenderText_Solid(fonts->magical_45, newMenu.menuChoices[i], newMenu.color);
    for (int i = 0; i < NUM_MENU; i++)
        newMenu.textures[i] = SDL_CreateTextureFromSurface(renderer, newMenu.menuSurface[i]);
    for (int i = 0; i < NUM_MENU; i++)
        SDL_FreeSurface(newMenu.menuSurface[i]);

    // Define position for texture 
    newMenu.pos[0].x = 450;
    newMenu.pos[0].y = 220;
    newMenu.pos[1].x = 450;
    newMenu.pos[1].y = 270;
    newMenu.pos[2].x = 450;
    newMenu.pos[2].y = 320;
    newMenu.pos[3].x = 450;
    newMenu.pos[3].y = 370;

    // Get the size of texture (weight & high)
    for (int i = 0; i < NUM_MENU; i++) {
        SDL_QueryTexture(newMenu.textures[i], NULL, NULL, &newMenu.pos[i].w, &newMenu.pos[i].h);
    }

    return newMenu;
}



void control(SDL_Renderer* renderer, LoadMedia media)
{
    bool running = true;
    SDL_Event e;

    //Define positonen for Controls
    SDL_Rect controls_pos;
    controls_pos.x = 0;
    controls_pos.y = 0;
    controls_pos.w = 1000;
    controls_pos.h = 600;

    while (running) {

        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, media->controlsTexture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
    }
}



//**************************************************************************************** MULTIPLAYER *********************************************************************
void getHostOrClient(SDL_Renderer* renderer, LoadMedia media, bool* hostOrClient) {
    SDL_Event e;
    int done = true;
    int x, y;


    //Define position for Multiplayer
    SDL_Rect imageH_pos;
    imageH_pos.x = 100;
    imageH_pos.y = 200;
    imageH_pos.w = 354;
    imageH_pos.h = 185;


    //Define position for Multiplayer
    SDL_Rect imageC_pos;
    imageC_pos.x = 520;
    imageC_pos.y = 200;
    imageC_pos.w = 354;
    imageC_pos.h = 185;


    while (done) {

        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
            {
                done = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                x = e.button.x;
                y = e.button.y;
                if (x >= imageH_pos.x && x <= imageH_pos.x + imageH_pos.w && y > imageH_pos.y && y <= imageH_pos.y + imageH_pos.h)
                {
                    *hostOrClient = true;
                    done = false;
                }
                else if (x >= imageC_pos.x && x <= imageC_pos.x + imageC_pos.w && y > imageC_pos.y && y <= imageC_pos.y + imageC_pos.h)
                {
                    *hostOrClient = false;
                    done = false;
                }
            }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, media->hostButtonTexture, NULL, &imageH_pos);
        SDL_RenderCopy(renderer, media->clientButtonTexture, NULL, &imageC_pos);
        SDL_RenderPresent(renderer);
    }
}


void enterName(SDL_Renderer* renderer, LoadMedia media, Fonts fonts, char name[]) {

    SDL_Event event;
    bool done = false;
    bool renderText = true;
    char nameInit[] = "NAME: ";

    //Textbox
    SDL_Rect txRect_pos;
    txRect_pos.x = 170;
    txRect_pos.y = 150;
    txRect_pos.w = 530;
    txRect_pos.h = 65;

    //Name:
    SDL_Color textColor = { 144, 77, 255, 255 };
    SDL_Color initColor = { 255,255,255, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(fonts->cuvert_24, name, textColor);
    SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Surface* nameInitSurface = TTF_RenderText_Solid(fonts->cuvert_24, nameInit, initColor);
    SDL_Texture* nameInitTexture = SDL_CreateTextureFromSurface(renderer, nameInitSurface);
    SDL_FreeSurface(nameInitSurface);

    SDL_Rect textRect;
    textRect.x = 275;
    textRect.y = 167;
    SDL_QueryTexture(nameTexture, NULL, NULL, &textRect.w, &textRect.h);

    SDL_Rect tInitRect;
    tInitRect.x = 185;
    tInitRect.y = 167;
    SDL_QueryTexture(nameInitTexture, NULL, NULL, &tInitRect.w, &tInitRect.h);

    while (!done) {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {

                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {

                    done = true;
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(name) > 0)
                {
                    char* p = name;
                    p++[strlen(p) - 1] = 0;
                    renderText = true;
                }
                else if (event.key.keysym.sym >= 97 && event.key.keysym.sym <= 123 && strlen(name) < 20) {
                    strcat(name, SDL_GetScancodeName(event.key.keysym.scancode));
                    renderText = true;
                }
            }
        }
        //update texture

        if (renderText) {
            textSurface = TTF_RenderText_Solid(fonts->cuvert_24, name, textColor);
            nameTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_QueryTexture(nameTexture, NULL, NULL, &textRect.w, &textRect.h);

            //render
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, media->textboxTexture, NULL, &txRect_pos);
            SDL_RenderCopy(renderer, nameInitTexture, NULL, &tInitRect);
            SDL_RenderCopy(renderer, nameTexture, NULL, &textRect);
            SDL_RenderPresent(renderer);

            renderText = false;
        }
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(nameInitTexture);
    SDL_DestroyTexture(nameTexture);
}



void enterIp(SDL_Renderer* renderer, LoadMedia media, Fonts fonts, char ip[]) {

    SDL_Event event;
    bool done = false;
    bool renderText = true;
    char ipInit[] = "ip-adress: ";


    //Textbox
    SDL_Rect txRect_pos;
    txRect_pos.x = 170;
    txRect_pos.y = 150;
    txRect_pos.w = 530;
    txRect_pos.h = 65;

    SDL_Color textColor = { 144, 77, 255, 255 };
    SDL_Color initColor = { 255,255,255, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(fonts->cuvert_24, ip, textColor);
    SDL_Texture* ipTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Surface* ipInitSurface = TTF_RenderText_Solid(fonts->cuvert_24, ipInit, initColor);
    SDL_Texture* ipInitTexture = SDL_CreateTextureFromSurface(renderer, ipInitSurface);
    SDL_FreeSurface(ipInitSurface);

    SDL_Rect ipRect;
    ipRect.x = 360;
    ipRect.y = 167;
    SDL_QueryTexture(ipTexture, NULL, NULL, &ipRect.w, &ipRect.h);

    SDL_Rect ipInitRect;
    ipInitRect.x = 185;
    ipInitRect.y = 167;
    SDL_QueryTexture(ipInitTexture, NULL, NULL, &ipInitRect.w, &ipInitRect.h);

    while (!done) {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {

                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(ip) > 0)
                {
                    char* p = ip;
                    p++[strlen(p) - 1] = 0;
                    renderText = true;
                }
                else if (event.key.keysym.sym >= 46 && event.key.keysym.sym <= 58 && strlen(ip) < 20) {
                    strcat(ip, SDL_GetScancodeName(event.key.keysym.scancode));
                    renderText = true;
                }
            }
        }
        //update texture

        if (renderText) {
            textSurface = TTF_RenderText_Solid(fonts->cuvert_24, ip, textColor);
            ipTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_QueryTexture(ipTexture, NULL, NULL, &ipRect.w, &ipRect.h);

            //render
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, media->textboxTexture, NULL, &txRect_pos);
            SDL_RenderCopy(renderer, ipInitTexture, NULL, &ipInitRect);
            SDL_RenderCopy(renderer, ipTexture, NULL, &ipRect);
            SDL_RenderPresent(renderer);

            renderText = false;
        }
    }


    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(ipInitTexture);
    SDL_DestroyTexture(ipTexture);
}