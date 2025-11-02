#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Model.h"
#include <vector>
#include <unordered_map>

#include "CreateInfo.h"

struct MeshNode {
    uint64_t parentObjectId;
    uint32_t openableId;
    uint32_t customId;
    uint64_t physicsId;
    uint64_t mirrorId;
    MeshNodeType type;
    BlendingMode blendingMode;
    int32_t localParentIndex;
    uint32_t globalMeshIndex;
    uint32_t nodeIndex;
    uint32_t materialIndex;
    Transform transform;              // These are the transforms updated by an Openable // rename to offsetTransform
    Transform transformPreviousFrame; // These are the transforms updated by an Openable // rename to offsetTransform
    glm::mat4 localTransform;         // Think of better name. Same for transform/transformPreviousFrame. Cause you are always confused.
    glm::mat4 inverseBindTransform;
    AABB worldspaceAabb;
    RenderItem renderItem;
    glm::mat4 localModelMatrix = glm::mat4(1.0f);
    glm::mat4 worldModelMatrix = glm::mat4(1.0f);

    bool isGold = false;              // Remove me when Carlos finishes Golden Glock texture set
};

struct MeshNodes {
    std::vector<MeshNode> m_meshNodes;
    std::unordered_map<std::string, uint32_t> m_localIndexMap; // maps mesh name to its local index
    AABB m_worldspaceAABB;

    void Init(uint64_t parentId, const std::string& modelName, const std::vector<MeshNodeCreateInfo>& meshNodeCreateInfoSet);
    void CleanUp();
    void UpdateHierarchy();
    void UpdateRenderItems(const glm::mat4& worldMatrix);
    void SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode);
    void SetObjectIdByMeshName(const std::string& meshName, uint64_t id);
    void SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId);
    
    void SetMeshMaterials(const std::string& materialName);
    void SetMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetTransformByMeshName(const std::string& meshName, Transform transform);
    void PrintMeshNames();
    void SetGoldFlag(bool flag);
    void DrawWorldspaceAABB(glm::vec4 color);
    void DrawWorldspaceAABBs(glm::vec4 color);
    void ForceDirty();

    const void SubmitRenderItems() const;
    const void SubmitOutlineRenderItems() const;

    bool NodeExists(const std::string& meshName);
    bool BoneExists(const std::string& boneName);
    bool HasNodeWithObjectId(uint64_t objectId) const;

    int32_t GetGlobalMeshIndex(int nodeIndex);
    Material* GetMaterial(int nodeIndex);
    const AABB* GetWorldSpaceAabbByMeshName(const std::string& meshName);
    const glm::mat4& GetLocalTransform(int nodeIndex) const;
    const glm::mat4& GetInverseBindTransform(int nodeIndex) const;
    const glm::mat4& GetLocalModelMatrix(int nodeIndex) const;
    const glm::mat4& GetWorldModelMatrix(int nodeIndex) const;
    const glm::mat4& GetBoneLocalMatrix(const std::string& boneName) const;
    const std::string& GetMeshNameByNodeIndex(int nodeIndex) const;

    MeshNode* GetMeshNodeByLocalIndex(int32_t index) ;
    MeshNode* GetMeshNodeByMeshName(const std::string& meshName);
    int32_t GetMeshNodeIndexByMeshName(const std::string& meshName);

    size_t GetNodeCount() const                                             { return m_nodeCount; }
    bool IsDirty() const                                                    { return m_isDirty; }
    const ArmatureData& GetArmature() const                                 { return m_armatureData; }
    const std::string& GetModelName() const                                 { return m_modelName; }
    const std::vector<MeshNode>& GetNodes() const                           { return m_meshNodes; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_renderItems; }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_renderItemsAlphaDiscarded; }
    const std::vector<RenderItem>& GetRenderItemsBlended() const            { return m_renderItemsBlended; }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_renderItemsHairTopLayer; }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_renderItemsHairBottomLayer; }
    const std::vector<RenderItem>& GetRenderItemsGlass() const              { return m_renderItemsGlass; }
    const std::vector<RenderItem>& GetRenderItemsToiletWater() const        { return m_renderItemsHairBottomLayer; }
    const std::vector<RenderItem>& GetRenderItemsMirror() const             { return m_renderItemsMirror; }

private:
    void UpdateAABBs(const glm::mat4& worldMatrix);

    ArmatureData m_armatureData;
    size_t m_nodeCount = 0;
    std::string m_modelName = UNDEFINED_STRING;
    glm::mat4 m_worldMatrixPreviousFrame = glm::mat4(0.0f); // Intentionally (0.0f) to force a dirty update on first use
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsGlass;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
    std::vector<RenderItem> m_renderItemsMirror;
    std::vector<RenderItem> m_renderItemsToiletWater;
    bool m_isGold = false;
    bool m_isDirty = true;
    bool m_forceDirty = true;
};