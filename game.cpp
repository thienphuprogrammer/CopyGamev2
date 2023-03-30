#include "Game.h"
#include "texture.h"
#include "GameObject.h"
#include "Map.h"
#include "textObj.h"



using namespace std;

SDL_Texture* imgStart = NULL;
SDL_Texture* background = NULL;
SDL_Texture* foreground = NULL;
SDL_Texture* victory = NULL;
SDL_Texture* bestScore = NULL;
SDL_Texture* babe = NULL;
SDL_Texture* spdPot = NULL;
SDL_Texture* jmpPot = NULL;
SDL_Texture* lagPot = NULL;
SDL_Texture* godPot = NULL; // erase lag effect
Mix_Music* Music = NULL;
TTF_Font* font = NULL;
TTF_Font* fontMenu = NULL;


SDL_Rect spdSrcRect = { 0,0,32,32 };
SDL_Rect spdRect = { 896,3456,32,32 };
SDL_Rect spdDestRect = { 896,3456,32,32 };

SDL_Rect jmpSrcRect = { 0,0,32,32 };
SDL_Rect jmpRect = { 272,1328,32,32};
SDL_Rect jmpDestRect = { 272,1328,32,32};

SDL_Rect lagSrcRect = { 0,0,32,32 };
SDL_Rect lagRect = {288,2576,32,32};
SDL_Rect lagDestRect = {288,2576,32,32};

SDL_Rect godSrcRect = { 0,0,32,32 };
SDL_Rect godRect = {32,1792,32,32};
SDL_Rect godDestRect = {32,1792,32,32};

SDL_Rect babeSrcRect = { 0,0,48,48 };
SDL_Rect babeRect = { 592,112,48,48 };
SDL_Rect babeDestRect = { 592,112,48,48 };

Uint32 score, hScore, timeVal, startTime;

SDL_Rect BgSrc = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT }, BgDest = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
GameObject* player;
textObj timeGame;
Map* mapper;



SDL_Renderer* Game::renderer = nullptr;

Game::Game()
{}
Game::~Game()
{}

int Game::createMenu(TTF_Font* font){
    imgStart = texture::LoadTexture("image/main_image/main_menu.png");
    // texture::Draw(imgStart, player->Camera, BgDest);
    if (imgStart == NULL) return 1;
    texture::Draw(imgStart, player->Camera, BgDest);
    const int numMenu = 2;
    // 0 = start, 1 = keys info, 2 = exit;

    SDL_Rect menuRect[numMenu];

    menuRect[0].x = 550; 
    menuRect[0].y = 360;
    menuRect[0].w = 960;
    menuRect[0].h = 32;

    menuRect[1].x = 550; 
    menuRect[1].y = 392;
    menuRect[1].w = 960;
    menuRect[1].h = 32;


    textObj textMenu[numMenu];

    textMenu[0].setText("New Game");
    textMenu[0].setTextColor(white);

    textMenu[1].setText("Exit");
    textMenu[1].setTextColor(white);

    bool chosen[numMenu] = {false, false};

    SDL_Event mouseEvent;
    while(true){
        texture::Draw(imgStart, player->Camera, BgDest);
        for (int i=0; i < numMenu; i++){
            textMenu[i].loadFromRenderedText(font, renderer);
            textMenu[i].renderText(renderer, menuRect[i].x, menuRect[i].y);
        }
        while(SDL_PollEvent(&mouseEvent)){
            switch(mouseEvent.type){
                case SDL_QUIT:
                    return 1;
                case SDL_MOUSEMOTION:
                    for (int i=0; i<numMenu; i++){
                        if(checkSelected(mouseEvent.motion.x, mouseEvent.motion.y, menuRect[i])){
                            if (chosen[i] == false){
                                chosen[i] = true;
                                textMenu[i].setTextColor(red);
                            }
                        } else {
                            if (chosen[i] == true){
                                chosen[i] = false;
                                textMenu[i].setTextColor(white);
                            }
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    for (int i=0; i < numMenu; i++){
                        if(checkSelected(mouseEvent.motion.x, mouseEvent.motion.y, menuRect[i])){
                                return i;
                            }
                        }
                    break;
                case SDL_KEYDOWN:
                    if (mouseEvent.key.keysym.sym == SDLK_ESCAPE){
                        return 1;
                    }
                default:
                    break;
            };
        }
        SDL_RenderPresent(renderer);
    }

    return 0;
}
void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    int flag = 0;
    if (fullscreen)
    {
        flag = SDL_WINDOW_FULLSCREEN;
    }
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        cout << "Initialized..." << endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flag);
        if (window)
        {
            cout << "Window created!" << endl;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            cout << "Renderer created!" << endl;
        }
        if (TTF_Init() == 0) {
            font = TTF_OpenFont("font/font2.ttf", 24);
        }
        if(TTF_Init() == 0){
            fontMenu = TTF_OpenFont("font/font2.ttf", 32);
        }
        // isRunning = true;
    }
    else {
        // isRunning = false;
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Music = Mix_LoadMUS("sound/win.wav");

    win = false;
    isRetrying = true;

    background = texture::LoadTexture("image/main_image/background.png");
    foreground = texture::LoadTexture("image/main_image/foreground.png");
    spdPot = texture::LoadTexture("image/speed_pot.png");
    jmpPot = texture::LoadTexture("image/jump_pot.png");
    lagPot = texture::LoadTexture("image/lag_pot.png");
    godPot = texture::LoadTexture("image/god_pot.png"); 
    babe = texture::LoadTexture("image/babe.png");
    victory = texture::LoadTexture("image/victory.png");
    bestScore = texture::LoadTexture("image/highScore.png");

    //Icon loading.
    string icoName = "image/icon.bmp";
    SDL_Surface* loadedSurface = SDL_LoadBMP(icoName.c_str());
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));
    SDL_SetWindowIcon(window, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    player = new GameObject(64, LEVEL_HEIGHT - 100);
    mapper = new Map();
    startTime = 0;

    int menuCheck = createMenu(fontMenu);
    if (menuCheck == 0){
        isRunning = true;
        startTime = SDL_GetTicks()/1000;
    } else if (menuCheck == 1){  
        isRunning = false;
    } else {
        isRunning = false;
    }
}

void Game::handleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT)
    {
        isRunning = false;
    }
    else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_RIGHT:
            {
                if (pause) break;
                player->inputType.right = 1;
                player->inputType.left = 0;
                break;
            }
            case SDLK_LEFT:
            {
                if (pause) break;
                player->inputType.left = 1;
                player->inputType.right = 0;
                break;
            }
            case SDLK_UP:
            {
                if (pause) break;
                player->inputType.up = 1;
                break;
            }

            case SDLK_SPACE:
            {
                if (pause) break;
                player->PrepareJump();
                break;
            }
            case SDLK_p:
            {
                pause = !pause;
            }
        }
    }
    else if (event.type == SDL_KEYUP)  //&& event.key.repeat == 0)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_RIGHT:
            {
                player->inputType.right = 2;
                break;
            }
            case SDLK_LEFT:
            {
                player->inputType.left = 2;
                break;
            }
            case SDLK_UP:
            {
                player->inputType.up = 2;
                break;
            }
            case SDLK_SPACE:
                if (pause) break;
                //if(player->status != jumping && player->status != charging)
                if (player->onGround == true)
                {
                    switch (player->inputType.jump)
                    {
                    case 0:
                        if (player->isJmpBuff == true)player->JumpBuff();
                        else player->Jump();
                        break;

                    case 1:
                        if (player->isJmpBuff == true)player->JumpRightBuff();
                        else player->JumpRight();
                        break;

                    case 2:
                        if (player->isJmpBuff == true)player->JumpLeftBuff();
                        else player->JumpLeft();
                        break;
                    }
                }
        }
        
    }
}
void Game::update()
{
    player->Update(mapper->tile, mapper->mapping);
    babeDestRect.y = babeRect.y - player->Camera.y;
    spdDestRect.y = spdRect.y - player->Camera.y;
    jmpDestRect.y = jmpRect.y - player->Camera.y;
    lagDestRect.y = lagRect.y - player->Camera.y;
    godDestRect.y = godRect.y - player->Camera.y;
    if (player->isWin == true)
    {
        win = true;
        if (Mix_PlayingMusic() == 0)
        {
            Mix_PlayMusic(Music, -1);
        }

        //Saving best score to file
        score = timeVal;
        cout << score;
        ifstream input("bestScore.txt");
        input >> hScore;
        ofstream output("bestScore.txt");
        if (score < hScore || hScore == 0) {
            output << score;
        }
        else output << hScore;
    }
    if (player->isWin == false)
    {
        Mix_HaltMusic();
    }
}
void Game::render()
{
    SDL_RenderClear(renderer);
    mapper->DrawMap(player->Camera);
    texture::Draw(background, player->Camera, BgDest);
    player->Render();
    texture::Draw(foreground, player->Camera, BgDest);
    texture::Draw(babe, babeSrcRect, babeDestRect);
    if(player->isSpdBuff_forDraw == false) texture::Draw(spdPot, spdSrcRect, spdDestRect);
    if(player->isJmpBuff_forDraw == false) texture::Draw(jmpPot, jmpSrcRect, jmpDestRect);
    if(player->isLag_forDraw == false) texture::Draw(lagPot, lagSrcRect, lagDestRect);
    if(player->godPot_draw == false) texture::Draw(godPot, godSrcRect, godDestRect);
    
    //Time counting
    string strTime = "TIME: ";
    timeVal = SDL_GetTicks() / 1000 - startTime;
    string timeRes = to_string(timeVal);
    strTime += timeRes;
    timeGame.setText(strTime);
    timeGame.setTextColor(red);
    timeGame.loadFromRenderedText(font, renderer);
    timeGame.renderText(renderer, 25, 15);

    if (pause) {
        SDL_Rect rPause;
        rPause.x = 0;
        rPause.y = 0;
        rPause.w = 960;
        rPause.h = 720;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &rPause);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Texture* texture = IMG_LoadTexture(renderer, "play-128.png");
        rPause.x = SCREEN_WIDTH / 2 - 45;
        rPause.y = SCREEN_HEIGHT / 2 - 45;
        SDL_QueryTexture(texture, NULL, NULL, &rPause.w, &rPause.h);
        SDL_RenderCopy(renderer, texture, NULL, &rPause);
    }
    SDL_RenderPresent(renderer);

}

/// <summary>
/// Retry the Current Level Option?
/// </summary>
void Game::retry()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT)
    {
        isRunning = false;
    }
    else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_y:
            win = false;
            player->isWin = false;
            isRetrying = true;
            startTime = SDL_GetTicks()/1000;
            player->Reset();
            mapper = new Map();
            break;
        case SDLK_n:
            isRunning = false;
            break;
        }
    }
    if (score < hScore || hScore == 0) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bestScore, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    else {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, victory, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}

void Game::clean()
{
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyTexture(imgStart);
    imgStart = NULL;
    SDL_DestroyTexture(background);
    background = NULL;
    SDL_DestroyTexture(foreground);
    foreground = NULL;
    SDL_DestroyTexture(babe);
    babe = NULL;
    SDL_DestroyTexture(victory);
    victory = NULL;
    SDL_DestroyTexture(bestScore);
    bestScore = NULL;
    SDL_DestroyTexture(spdPot);
    spdPot = NULL;
    SDL_DestroyTexture(jmpPot);
    jmpPot = NULL;
    SDL_DestroyTexture(lagPot);
    lagPot = NULL;
    player->ObjectClose();
    player = NULL;
    mapper->CloseMap();
    mapper = NULL;
    TTF_CloseFont(font);
    font = NULL;

    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
    cout << "Game cleaned" << endl;
}

bool Game::checkSelected(const int& x, const int& y, const SDL_Rect& rect)
{
    if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) return true;
    return false;
}
bool Game::running()
{
    return isRunning;
}

bool Game::winning()
{
    return win;
}

bool Game::retrying()
{
    return isRetrying;
}

