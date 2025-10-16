#pragma once
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Model.h"
#include <vector>
#include <unordered_map>

#include "CreateInfo.h"

enum MeshNodeType {
    DEFAULT,
    OPENABLE,
    RIGID_STATIC,
    RIGID_DYNAMIC
};

struct MeshNode {
    AABB worldspaceAabb;
    BlendingMode blendingMode;
    int32_t localParentIndex;
    uint32_t globalMeshIndex;
    uint32_t materialIndex;
    Transform transform;              // These are the transforms updated by an Openable // rename to offsetTransform
    Transform transformPreviousFrame; // These are the transforms updated by an Openable // rename to offsetTransform
    //std::vector<ObjectType> m_objectTypes;
    uint64_t objectId;                // Written to renderItem.objectId  - rename to renderItemObjectId or something that implies its written to the render item id
    //std::vector<uint64_t> m_openableIds;              // Maps node to an Openable     - do not use me. think of a better way.
    //std::vector<uint64_t> m_rigidDynamictIds;         // Maps node to a RigidDynamic  - do not use me. think of a better way.
    //std::vector<uint64_t> m_rigidStaticIds;           // Maps node to a RigidStatic   - do not use me. think of a better way.
    glm::mat4 modelMatrix;   // world or local? find out and name accordingly
    glm::mat4 localTransform; // think of better namw
    glm::mat4 inverseBindTransform; // think of better name
    MeshNodeType type = MeshNodeType::DEFAULT;
    bool isGold = false;

    // probably store parentId, but also somehow openableId if type is openable
};

struct MeshNodes {
    std::vector<MeshNode> m_meshNodes;
    //std::vector<AABB> m_worldspaceAabbs;
    //std::vector<BlendingMode> m_blendingModes;
    //std::vector<int32_t> m_localParentIndices;
    //std::vector<uint32_t> m_globalMeshIndices;
    //std::vector<uint32_t> m_materialIndices;
    //std::vector<Transform> m_transforms;              // These are the transforms updated by an Openable
    //std::vector<Transform> m_transformsPreviousFrame; // These are the transforms updated by an Openable
    ////std::vector<ObjectType> m_objectTypes;
    //std::vector<uint64_t> m_objectIds;                // Written to renderItem.objectId
    //std::vector<glm::mat4> m_modelMatrices;
    //std::vector<glm::mat4> m_localTransforms;
    //std::vector<glm::mat4> m_inverseBindTransforms;
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

    bool NodeExists(const std::string& meshName);
    bool BoneExists(const std::string& boneName);
    bool HasNodeWithObjectId(uint64_t objectId) const;

    int32_t GetGlobalMeshIndex(int nodeIndex);
    uint64_t GetObjectIdOfFirstOpenableParentNode(int nodeIndex);
    Material* GetMaterial(int nodeIndex);
    const AABB* GetWorldSpaceAabbByMeshName(const std::string& meshName);
    const glm::mat4& GetLocalTransform(int nodeIndex) const;
    const glm::mat4& GetInverseBindTransform(int nodeIndex) const;
    const glm::mat4& GetMeshModelMatrix(int nodeIndex) const;
    const glm::mat4& GetBoneLocalMatrix(const std::string& boneName) const;
    const std::string& GetMeshNameByNodeIndex(int nodeIndex) const;

    MeshNode* GetMeshNodeByLocalIndex(int32_t index) ;
    MeshNode* GetMeshNodeByMeshName(const std::string& meshName);
    int32_t GetMeshNodeIndexByMeshName(const std::string& meshName);

    size_t GetNodeCount() const                                             { return m_nodeCount; }
    bool IsDirty() const                                                    { return m_isDirty; }
    const ArmatureData& GetArmature() const                                 { return m_armatureData; }
    const std::string& GetModelName() const                                 { return m_modelName; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_renderItems; }
    const std::vector<RenderItem>& GetRenderItemsBlended() const            { return m_renderItemsBlended; }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_renderItemsAlphaDiscarded; }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_renderItemsHairTopLayer; }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_renderItemsHairBottomLayer; }

private:
    void UpdateAABBs(const glm::mat4& worldMatrix);

    ArmatureData m_armatureData;
    size_t m_nodeCount = 0;
    std::string m_modelName = UNDEFINED_STRING;
    glm::mat4 m_worldMatrixPreviousFrame = glm::mat4(0.0f); // Intentionally (0.0f) to force a dirty update on first use
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
    bool m_isGold = false;
    bool m_isDirty = true;
};