#pragma once
#include "Types/Map/Map.h"
#include <vector>
#include <string>

namespace MapManager {
    void Init();
    void NewMap(const std::string& name, int chunkWidth, int chunkDepth, float initialHeight);
    void SaveMap(const std::string& mapName);
    void LoadMap(const std::string& mapName);

    Map* GetTestMap();
    Map* GetMapByIndex(int32_t index);
    Map* GetMapByName(const std::string& name);
    int32_t GetMapIndexByName(const std::string& name);
}