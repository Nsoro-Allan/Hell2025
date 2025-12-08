#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include <vector>
#include "Camera/Camera.h"
#include "Player/Player.h"

namespace Game {
    void BeginFrame();
    void Create();
    void Update();
    float GetDeltaTime();
    float GetTotalTime();
    void RespawnPlayers();

    Player* GetPlayerByPlayerId(uint64_t playerId);
    Player* GetLocalPlayerByIndex(uint32_t index);
    void SetPlayerKeyboardAndMouseIndex(int playerIndex, int keyboardIndex, int mouseIndex);
    void NextSplitScreenMode();
    void SetSplitscreenMode(SplitscreenMode mode);
    const SplitscreenMode& GetSplitscreenMode();
    int32_t GetLocalPlayerCount();
    int32_t GetOnlinePlayerCount();
    Camera* GetLocalPlayerCameraByIndex(uint32_t index);
    float GetLocalPlayerFovByIndex(uint32_t index);

    const glm::vec3& GetMoonlightDirection();
    
    // Audio
    void PlayGlassHitAudio();
    void PlayFootstepIndoorAudio();
    void PlayFootstepOutdoorAudio();
}