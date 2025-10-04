#pragma once
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Types/Map/Map.h"

namespace Renderer {
    void InitMain();
    void InitWoundMaskArray();
    void RenderLoadingScreen();
    void PreGameLogicComputePasses();
    void RenderGame();
    void HotloadShaders();
    void NextRendererOverrideState();

    int32_t GetNextFreeWoundMaskIndexAndMarkItTaken();
    void MarkWoundMaskIndexAsAvailable(int32_t index);

    void RecalculateAllHeightMapData(bool blitWorldMap);

    void DrawPoint(glm::vec3 position, glm::vec3 color, bool obeyDepth = false, int exclusiveViewportIndex = -1);
    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color, bool obeyDepth = false, int exclusiveViewportIndex = -1, int ignoredViewportIndex = -1);
    void DrawAABB(const AABB& aabb, const glm::vec3& color);
    void DrawAABB(const AABB& aabb, const glm::vec3& color, const glm::mat4& worldTransform);

    void ReadBackHeightMapData(Map* map);

    RendererSettings& GetCurrentRendererSettings();
}