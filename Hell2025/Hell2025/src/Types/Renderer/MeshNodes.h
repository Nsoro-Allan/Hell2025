#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Math/OBB.h"
#include "Model.h"
#include <vector>
#include <unordered_map>

#include "CreateInfo.h"

struct MeshNode {
    uint64_t id; // unused. you need to think if it would be worthwhile storing all mesh nodes elsewhere
    uint64_t parentObjectId;
    uint32_t openableId;
    uint32_t customId;
    uint64_t rigidDynamicId;
    uint64_t mirrorId;
    BlendingMode blendingMode;
    int32_t localParentIndex;
    uint64_t meshBvhId;
    uint32_t globalMeshIndex;
    uint32_t nodeIndex;
    uint32_t materialIndex;
    Transform transform;              // These are the transforms updated by an Openable // rename to offsetTransform
    glm::mat4 localTransform;         // Think of better name. Same for transform/transformPreviousFrame. Cause you are always confused.
    glm::mat4 inverseBindTransform;
    AABB worldspaceAabb;
    OBB worldSpaceObb;
    RenderItem renderItem;
    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);
    glm::mat4 worldModelMatrixPreviousFrame = glm::mat4(0.0f); // Intentionally invalid matrix (forces static World scene bvh update)
    glm::mat4 inverseWorldMatrix = glm::mat4(1.0f);
    DecalType decalType = DecalType::PLASTER;
    bool forceDynamic;
    bool castShadows;
    bool castCSMShadows;
    bool ownsOpenableId = false;      // Only nodes with this flag set to true receive the transform from the openable ID
    glm::vec3 emissiveColor = glm::vec3(1.0f);
    glm::vec3 tintColor = glm::vec3(1.0f);
    bool addToNavMesh = false;
    //bool rigidIsKinematic = false;    // You need to be careful this does not get out of sync with actual physx kinematic state
    bool movedThisFrame = true;
    glm::mat4 scaleMatrix = glm::mat4(1.0f);
};

struct MeshNodes {
    std::vector<MeshNode> m_meshNodes;
    std::unordered_map<std::string, uint32_t> m_localIndexMap; // maps mesh name to its local index
    AABB m_worldspaceAABB;
    
    void Init(uint64_t parentId, const std::string& modelName, const std::vector<MeshNodeCreateInfo>& meshNodeCreateInfoSet);
    void CleanUp();
    void Update(const glm::mat4& worldMatrix);
    void SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode);
    void SetObjectIdByMeshName(const std::string& meshName, uint64_t id);
    void SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId);
    
    void SetMeshMaterials(const std::string& materialName);
    void SetMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetTransformByMeshName(const std::string& meshName, Transform transform);
    void PrintMeshNames();
    void DrawWorldspaceAABB(glm::vec4 color);
    void DrawWorldspaceAABBs(glm::vec4 color);
    void ForceDirty();
    void ResetFirstFrame();
    void SleepAllPhysics();
    void WakeAllPhysics();
    void AddForceToPhsyics(const glm::vec3 force);
    void CastCSMShadows();
    
    const void SubmitRenderItems() const;
    const void SubmitOutlineRenderItems() const;
    
    bool NodeExists(const std::string& meshName);
    bool BoneExists(const std::string& boneName);
    bool HasNodeWithObjectId(uint64_t objectId) const;
    bool MeshNodeIsOpen(const std::string& meshName);
    bool MeshNodeIsClosed(const std::string& meshName);
    bool MeshNodeIsStatic(int nodeIndex);
    bool MeshNodeIsNonKinematicRigidDynamic(int nodeIndex);
    
    int32_t GetGlobalMeshIndex(int nodeIndex);
    Material* GetMaterial(int nodeIndex);
    const AABB* GetWorldSpaceAabbByMeshName(const std::string& meshName);
    const glm::mat4& GetLocalTransform(int32_t nodeIndex) const;
    const glm::mat4& GetInverseBindTransform(int32_t nodeIndex) const;
    const glm::mat4& GetLocalModelMatrix(int32_t nodeIndex) const;
    const glm::mat4& GetWorldModelMatrix(int32_t nodeIndex) const;
    const glm::mat4& GetBoneLocalMatrix(const std::string& boneName) const;
    const std::string& GetMeshNameByNodeIndex(int32_t nodeIndex) const;
    
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
    const std::vector<RenderItem>& GetRenderItemsStainedGlass() const       { return m_renderItemsStainedGlass; }

private:
    void UpdateAABBsFromWorldMatrices();
    //void UpdateAABBs(const glm::mat4& worldMatrix);
    void UpdateHierarchy();
    void InitPhysicsTransforms();
    void UpdateKinematicPhysicsTransforms();

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
    std::vector<RenderItem> m_renderItemsStainedGlass;
    bool m_isDirty = true;
    bool m_forceDirty = true;
    bool m_firstFrame = true;
};