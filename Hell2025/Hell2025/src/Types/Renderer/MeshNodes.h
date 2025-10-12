#pragma once
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Model.h"
#include <vector>
#include <unordered_map>

struct MeshNodes {
    std::vector<BlendingMode> m_blendingModes;
    std::vector<uint32_t> m_globalMeshIndices;
    std::vector<uint32_t> m_materialIndices;
    std::vector<int32_t> m_localParentIndices;
    std::vector<Transform> m_transforms;
    std::vector<glm::mat4> m_modelMatrices;
    std::vector<glm::mat4> m_localTransforms;
    std::vector<glm::mat4> m_inverseBindTransforms;
    std::vector<AABB> m_worldspaceAabbs;
    std::vector<ObjectType> m_objectTypes;
    std::vector<uint64_t> m_objectIds;
    std::unordered_map<std::string, uint32_t> m_localIndexMap; // maps mesh name to its local index
    AABB m_worldspaceAABB;

    void InitFromModel(const std::string& modelName);
    void InitFromModel(Model* model);
    void CleanUp();
    void UpdateHierachy();
    void UpdateRenderItems(const glm::mat4& worldMatrix);
    void SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode);
    void SetObjectTypeByMeshName(const std::string& meshName, ObjectType objectType);
    void SetObjectIdByMeshName(const std::string& meshName, uint64_t id);
    void SetOpenableByMeshName(const std::string& meshName, uint64_t openableId);
    void SetMeshMaterials(const std::string& materialName);
    void SetMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetTransformByMeshName(const std::string& meshName, Transform transform);
    void SetObjectTypes(ObjectType objectType);
    void SetObjectIds(uint64_t objectId);
    void PrintMeshNames();
    void SetGoldFlag(bool flag);
    void DrawWorldspaceAABB(glm::vec4 color);
    void DrawWorldspaceAABBs(glm::vec4 color);

    bool BoneExists(const std::string& boneName);
    bool HasNodeWithObjectId(uint64_t objectId) const;
        
    RenderItem* GetRenderItemByNodeIndex(int nodeIndex);
    int32_t GetGlobalMeshIndex(int nodeIndex);
    Material* GetMaterial(int nodeIndex);
    glm::mat4 GetLocalTransform(int nodeIndex);
    glm::mat4 GetInverseBindTransform(int nodeIndex);
    glm::mat4 GetMeshModelMatrix(int nodeIndex);
    glm::mat4 GetBoneLocalMatrix(const std::string& boneName);

    size_t GetNodeCount() const                                             { return m_nodeCount; }
    bool HasHierachyChanged() const                                         { return m_hierarchyChanged; }
    const ArmatureData& GetArmature() const                                 { return m_armatureData; }
    const std::string& GetModelName() const                                 { return m_modelName; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_renderItems; }
    const std::vector<RenderItem>& GetRenderItemsBlended()const             { return m_renderItemsBlended; }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_renderItemsAlphaDiscarded; }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_renderItemsHairTopLayer; }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_renderItemsHairBottomLayer; }

private:
    void UpdateAABBs(const glm::mat4& worldMatrix);

    ArmatureData m_armatureData;
    uint32_t m_nodeCount = 0;
    std::string m_modelName = UNDEFINED_STRING;
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
    bool m_isGold = false;
    bool m_hierarchyChanged = false;
};