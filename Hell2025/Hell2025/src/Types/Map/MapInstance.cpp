#include "MapInstance.h"
#include "Managers/MapManager.h"

uint32_t MapInstance::GetChunkCountX() {
    Map* map = MapManager::GetMapByIndex(m_mapIndex);
    if (!map) return 0;
    else return map->GetChunkCountX();
}

uint32_t MapInstance::GetChunkCountZ() {
    Map* map = MapManager::GetMapByIndex(m_mapIndex);
    if (!map) return 0;
    else return map->GetChunkCountZ();
}