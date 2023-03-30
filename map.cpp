#include "Map.h"
#include "texture.h"
#include "game.h"
#include "gameObject.h"


/// <summary>
/// Hashed Map for Level 1.
/// Demo Version.
/// </summary>
int lv1[240][60];

void Map::init(int lv1[240][60]) {
    ifstream fi("hashmap.txt");
    if (!fi) {
        cout << "Error: File is unavailable to read.\n";
        return;
    }
    cout << "File is ready to read.\n";
    int n = 0;
    while (!fi.eof()) {
        string line;
        getline(fi, line);
        int id = 0;
        for (auto& x: line) {
            if (x != ' ') {
                lv1[n][id] = x - '0';
                id++;
            }
        }
        n++;
    }
}

/// <summary>
/// Constructor.
/// Load Texture to get ready for Map Layouts.
/// </summary>
Map::Map()
{
    dirt = texture::LoadTexture("image/dirt.png");
    grass = texture::LoadTexture("image/grass.png");
    stone = texture::LoadTexture("image/stone.png");
    red_grass = texture::LoadTexture("image/red_grass.png");
    

    src.x = 0;
    src.y = 0;
    src.h = 16;
    src.w = 16;

    dest.x = 0;
    dest.y = 0;
    dest.h = 16;
    dest.w = 16;

    init(lv1);
    LoadMap(lv1);
}

/// <summary>
/// Load Map based on given resolutions via "blocks" quantities.
/// 32x32 pixels for each "block" loaded.
/// </summary>
void Map::LoadMap(int arr[240][60]) {
    for (int row = 0; row < 240; row++) {
        for (int column = 0; column < 60; column++) {
            mapping[row][column] = arr[row][column];

            tile[row][column].x = column * 16;
            tile[row][column].y = row * 16;
            tile[row][column].w = TILE_WIDTH;
            tile[row][column].h = TILE_HEIGHT;
        }
    }
}


/// <summary>
/// Check Collision.
/// Simply just Coordinators.
/// </summary>
bool Map::checkCollision(SDL_Rect a, SDL_Rect b) {
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if (bottomA <= topB) {
        return false;
    }

    if (topA >= bottomB) {
        return false;
    }

    if (rightA <= leftB) {
        return false;
    }

    if (leftA >= rightB) {
        return false;
    }

    return true;
}

/// <summary>
/// Render based on JK position.
/// </summary>
/// <param name="Camera"></param>
void Map::DrawMap(SDL_Rect Camera) {
    int type = 0;
    for (int row = 0; row < 240; row++) {
        for (int column = 0; column < 60; column++){
            type = mapping[row][column];

            dest.x = column * 16;
            dest.y = row * 16;
            if (checkCollision(Camera, tile[row][column]))
            {
                dest.y -= Camera.y;
                switch (type)
                {
                case 0:
                    texture::Draw(dirt, src, dest);
                    break;
                case 1:
                    texture::Draw(grass, src, dest);
                    break;
                case 2:
                    texture::Draw(stone, src, dest);
                    break;
                case 3:
                    //nothing
                    break;
                case 4:
                    //speed buff
                    break;
                case 5:
                    // jump buff
                    break;
                case 6: 
                    // lag potion
                    break;
                case 7:
                    // erase lag effect
                    break;
                case 8:
                    texture::Draw(red_grass, src, dest);
                    break;
                default:
                    break;
                }
            }
        }
    }

}

void Map::CloseMap()
{
    SDL_DestroyTexture(dirt);
    SDL_DestroyTexture(stone);
    SDL_DestroyTexture(grass);
    SDL_DestroyTexture(red_grass);
    dirt = NULL;
    stone = NULL;
    grass = NULL;
    red_grass = NULL;
}
