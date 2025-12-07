#pragma once
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Math/OBB.h"
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

    void DrawPoint(const glm::vec3& position, const glm::vec4& color, bool obeyDepth = false, int exclusiveViewportIndex = -1);
    void DrawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color, bool obeyDepth = false, int exclusiveViewportIndex = -1, int ignoredViewportIndex = -1);
    void DrawLine2D(const glm::ivec2& begin, const glm::ivec2& end, const glm::vec4& color, int viewportWidth, int viewportHeight);
    void DrawAABB(const AABB& aabb, const glm::vec4& color);
    void DrawAABB(const AABB& aabb, const glm::vec4& color, const glm::mat4& worldTransform);
    void DrawOBB(const OBB& obb, const glm::vec4& color);
    void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color);

    void ReadBackHeightMapData(Map* map);

    RendererSettings& GetCurrentRendererSettings();
}