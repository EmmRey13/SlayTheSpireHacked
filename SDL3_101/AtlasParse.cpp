#include "AtlasParse.h"
#include "SDL3_image/SDL_image.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

string clean(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);

}

string getDir(const string& path) {
    size_t end = path.find_last_of("/\\");

    if (end == string::npos) {
        return "";
    }
    else {
        return path.substr(0, end + 1);
    }
}

bool Atlas::load(const string& path, SDL_Renderer* renderer) {
    ifstream file(path);

    if (!file.is_open()) {
        SDL_Log("Some files failed to open");
        return false;
    }

    string line;
    AtlasPage* currentPage = nullptr;
    AtlasRegion* currentRegion = nullptr;

    string atlasDir = getDir(path);

    while (getline(file, line)) {
        
        if (line.empty()) {
            continue;
        }

        bool hasIndent = (line[0] == ' ' || line[0] == '\t');
        string cleanLine = clean(line);

        if (!cleanLine.empty() && !hasIndent && cleanLine.find(':') == string::npos && (cleanLine.find(".png") != string::npos || cleanLine.find(".jpg") != string::npos)) {
            pages.push_back({});
            currentPage = &pages.back();
            currentPage->textureFile = cleanLine;
            currentRegion = nullptr;
            continue;
        }

        if (hasIndent && !cleanLine.empty() && cleanLine.find(':') == string::npos) {

            if (!currentPage) {
                SDL_Log("Atlas parse error: region found before any page");
                continue;
            }

            currentPage->regions.push_back({});
            currentRegion = &currentPage->regions.back();
            currentRegion->name = cleanLine;
            continue;
        }

        if (currentRegion) {
            size_t colon = cleanLine.find(':');
            if (colon == string::npos) continue;

            string key = cleanLine.substr(0, colon);
            string value = clean(cleanLine.substr(colon + 1));

            if (key == "rotate")
                currentRegion->rotated = (value == "true");

            else if (key == "xy")
                sscanf_s(value.c_str(), "%d, %d", &currentRegion->x, &currentRegion->y);

            else if (key == "size")
                sscanf_s(value.c_str(), "%d, %d", &currentRegion->width, &currentRegion->height);

            else if (key == "orig") {
                sscanf_s(value.c_str(), "%d, %d",
                    &currentRegion->origWidth,
                    &currentRegion->origHeight);
            }
            else if (key == "offset") {
                sscanf_s(value.c_str(), "%d, %d",
                    &currentRegion->offsetX,
                    &currentRegion->offsetY);
            }
            else if (key == "index") {
                currentRegion->index = stoi(value);
            }
        }

    }

    for (auto& page : pages) {
        string fileDir = atlasDir + page.textureFile;

        SDL_Surface* surf = IMG_Load(fileDir.c_str());
        
        if (!surf) {
            SDL_Log("some textures failed to load");
            return false;
        }

        page.texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);

        if (!page.texture) {
            SDL_Log("Failed to create texture from: %s", fileDir.c_str());
            return false;
        }

    } 
    return true;
}

AtlasRegion* Atlas::findRegion(const string& name, AtlasPage** pageOut) {
    for (auto& page : pages) {
        for (auto& region : page.regions) {
            if (region.name == name) {
                if (pageOut) *pageOut = &page;
                return &region;
            }
        }
    }
    return nullptr;
}

void Atlas::renderAtlasTextures(SDL_Renderer* renderer, Atlas& atlas, float xOffset = 0, float yOffset = 0) {
    for (auto& page : atlas.pages) {
        if (!page.texture) continue;

        for (auto& region : page.regions) {
            SDL_FRect src;
            src.x = (float)region.x;
            src.y = (float)region.y;
            src.w = (float)region.width;
            src.h = (float)region.height;

            SDL_FRect dst;
            int origW = (region.origWidth > 0) ? region.origWidth : region.width;
            int origH = (region.origHeight > 0) ? region.origHeight : region.height;
            int offX = region.offsetX;
            int offY = region.offsetY;

            dst.x = xOffset + offX;
            dst.y = yOffset + offY;
            dst.w = (float)origW;
            dst.h = (float)origH;

            if (region.rotated) {
                SDL_RenderTextureRotated(renderer, page.texture, &src, &dst, 90.0, nullptr, SDL_FLIP_NONE);
            }
            else {
                SDL_RenderTexture(renderer, page.texture, &src, &dst);
            }
        }
    }
}
