#include "Editor/Editor.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "HellLogging.h"
#include "Managers/MapManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"


namespace Editor {
    void UpdatePlayerCampaignSpawnPlacement() {
        Map* map = MapManager::GetMapByName(GetEditorMapName());
        if (!map) return;

        if (Input::LeftMousePressed()) {
            PhysXRayResult result = GetMouseRayPhsyXHitPosition();
            if (result.hitFound) {
                map->AddPlayerCampaignSpawn(result.hitPosition);
                World::UpdateWorldSpawnPointsFromMap(map);
                ExitObjectPlacement();
                Logging::Debug() << "Added player campaign spawn: " << result.hitPosition;
            }
        }
    }

    void UpdatePlayerDeathmatchSpawnPlacement() {
        Map* map = MapManager::GetMapByName(GetEditorMapName());
        if (!map) return;

        if (Input::LeftMousePressed()) {
            PhysXRayResult result = GetMouseRayPhsyXHitPosition();
            if (result.hitFound) {
                map->AddPlayerDeathmatchSpawn(result.hitPosition);
                World::UpdateWorldSpawnPointsFromMap(map);
                Logging::Debug() << "Added player deathmatch spawn: " << result.hitPosition;
                ExitObjectPlacement();
            }
        }
    }
}