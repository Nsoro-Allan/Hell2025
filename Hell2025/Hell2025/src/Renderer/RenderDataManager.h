#pragma once
#include "HellTypes.h"
#include "Types/Game/AnimatedGameObject.h"
#include "Types/Game/Bullet.h"
#include <vector>

struct DecalPaintingInfo {
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;
    int textureArrayIndex = 0;
};

namespace RenderDataManager {
    void BeginFrame();
    void Update();


    void SubmitAnimatedGameObjectForSkinning(AnimatedGameObject* animatedGameObject);
    void ResetBaseSkinnedVertex();
    void IncrementBaseSkinnedVertex(uint32_t vertexCount);

    uint32_t GetBaseSkinnedVertex();
    std::vector<AnimatedGameObject*>& GetAnimatedGameObjectToSkin();

    inline std::vector<glm::mat4> skinningTransforms;

    int EncodeBaseInstance(int playerIndex, int instanceOffset);
    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset);

    // Submissions
    void SubmitGPULightHighRes(uint32_t lightIndex);
    void SubmitDecalRenderItem(const RenderItem& renderItem);
    void SubmitRenderItem(const RenderItem& renderItem);
    void SubmitGlassRenderItem(const RenderItem& renderItem);
    void SubmitRenderItemsGlass(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItems(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItemsBlended(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItemsAlphaDiscard(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItemsAlphaHairTopLayer(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItemsAlphaHairBottomLayer(const std::vector<RenderItem>& renderItems);
    void SubmitRenderItemsMirror(const std::vector<RenderItem>& renderItems);

    void SubmitOutlineRenderItem(const RenderItem& renderItem);
    void SubmitOutlineRenderItems(const std::vector<RenderItem>& renderItems);

    // House submissions
    void SubmitRenderItem(const HouseRenderItem& renderItem);
    void SubmitRenderItems(const std::vector<HouseRenderItem>& renderItems);
    void SubmitOutlineRenderItem(const HouseRenderItem& renderItem);
    void SubmitOutlineRenderItems(const std::vector<HouseRenderItem>& renderItems);

    void SubmitDecalPaintingInfo(DecalPaintingInfo decalPaintingInfo);

    const RendererData& GetRendererData();
    const std::vector<glm::mat4> GetOceanPatchTransforms();
    const std::vector<GPULight>& GetGPULightsHighRes();
    const std::vector<DecalPaintingInfo>& GetDecalPaintingInfo();
    const std::vector<HouseRenderItem>& GetHouseRenderItems();
    const std::vector<HouseRenderItem>& GetHouseOutlineRenderItems();
    const std::vector<RenderItem>& GetRenderItems();
    const std::vector<RenderItem>& GetGlassRenderItems();
    const std::vector<RenderItem>& GetDecalRenderItems();
    const std::vector<RenderItem>& GetInstanceData();
    const std::vector<RenderItem>& GetOutlineRenderItems();
    const std::vector<RenderItem>& GetMirrorRenderItems();
    const std::vector<ScreenSpaceBloodDecalInstanceData>& GetScreenSpaceBloodDecalInstanceData();
    const std::vector<ViewportData>& GetViewportData();
    const DrawCommandsSet& GetDrawInfoSet();
    const FlashLightShadowMapDrawInfo& GetFlashLightShadowMapDrawInfo();

}