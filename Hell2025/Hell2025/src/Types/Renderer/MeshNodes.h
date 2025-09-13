#pragma once
#include "HellTypes.h"
#include "Model.h"
#include <vector>
#include <unordered_map>

struct MeshNodes {
    std::vector<BlendingMode> m_blendingModes;
    std::vector<uint32_t> m_globalMeshIndices;
    std::vector<uint32_t> m_materialIndices;
    std::vector<uint64_t> m_openHandlerStateIds;
    std::vector<int32_t> m_localParentIndices;
    std::vector<Transform> m_transforms;
    std::vector<glm::mat4> m_modelMatrices;
    std::vector<glm::mat4> m_localTransforms;
    std::vector<glm::mat4> m_inverseBindTransforms;
    std::vector<ObjectType> m_objectTypes;
    std::vector<uint64_t> m_objectIds;
    std::unordered_map<std::string, uint32_t> m_localIndexMap; // maps mesh name to its local index

    void InitFromModel(const std::string& modelName);
    void InitFromModel(Model* model);
    void CleanUp();
    void UpdateHierachy();
    void UpdateRenderItems(const glm::mat4& worldMatrix);
    void SetTransformByMeshName(const std::string& meshName, Transform transform);
    void SetMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode);
    void SetObjectTypes(ObjectType objectType);
    void SetObjectIds(uint64_t objectId);
    void PrintMeshNames();
    void SetGoldFlag(bool flag);

    bool HasNodeWithObjectId(uint64_t objectId) const;
        
    RenderItem* GetRenderItemByNodeIndex(int nodeIndex);
    int32_t GetGlobalMeshIndex(int nodeIndex);
    Material* GetMaterial(int nodeIndex);
    glm::mat4 GetLocalTransform(int nodeIndex);
    glm::mat4 GetInverseBindTransform(int nodeIndex);
    glm::mat4 GetMeshModelMatrix(int nodeIndex);

    const size_t GetNodeCount() const                                       { return m_nodeCount; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_renderItems; }
    const std::vector<RenderItem>& GetRenderItemsBlended()const             { return m_renderItemsBlended; }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_renderItemsAlphaDiscarded; }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_renderItemsHairTopLayer; }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_renderItemsHairBottomLayer; }

private:
    uint32_t m_nodeCount = 0;
    std::string m_modelName = "UNDEFINED\n";
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
    bool m_isGold = false;
};