#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>

using namespace std;

struct AtlasRegion {
    string name;
    int x, y;
    int width, height;
    bool rotated;

    int origHeight;
    int origWidth;
    int offsetX;
    int offsetY;

    int index;
};

struct AtlasPage {
    string textureFile;
    SDL_Texture* texture;
    vector<AtlasRegion> regions;

    AtlasRegion* findRegion(const string& name);
};

class Atlas {
public:
    vector<AtlasPage> pages;

    bool load(const string& path, SDL_Renderer* renderer);

    AtlasRegion* findRegion(const string& name, AtlasPage** pageOut = nullptr);

    static void renderAtlasTextures(SDL_Renderer* renderer, Atlas& atlas, float xOffset = 0, float yOffset = 0);

    
};
