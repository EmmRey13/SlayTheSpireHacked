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

        if (hasIndent && !cleanLine.empty() && cleanLine.find(':') == string::npos) {\
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
}