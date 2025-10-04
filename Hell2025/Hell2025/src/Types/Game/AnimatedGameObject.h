#pragma once
#include "Types/Animation/Animator.h"
#include "Types/Renderer/SkinnedModel.h"
#include <unordered_map>

struct MeshRenderingEntry {
    std::string meshName;
    int materialIndex = 0;
    int woundMaterialIndex = 0;
    int emissiveColorTexutreIndex = -1;
    bool blendingEnabled = false;
    bool drawingEnabled = true;
    bool renderAsGlass = false;
    int meshIndex = -1;
    float furLength = 0.0f;
    float furShellDistanceAttenuation = 0.0f;
    float furUVScale = 0.0f;
};

struct AnimatedGameObject {
    enum class AnimationMode { BINDPOSE, ANIMATION, RAGDOLL, RAGDOLL_V2 };

public:
    void Init();
    void CleanUp();
    void SetMeshWoundMaskTextureIndex(const std::string& meshName, int32_t woundMaskTextureIndex);
    void UpdateRenderItems();
	void Update(float deltaTime);
	void SetName(std::string name);
	void SetSkinnedModel(std::string skinnedModelName);
	void SetScale(float scale);
	void SetPosition(glm::vec3 position);
	void SetRotationX(float rotation);
	void SetRotationY(float rotation);
	void SetRotationZ(float rotation);
    void PlayAnimation(const std::string& layerName, const std::string& animationName, float speed);
    void PlayAnimation(const std::string& layerName, std::vector<std::string>& animationNames, float speed);
    void PlayAndLoopAnimation(const std::string& layerName, const std::string& animationName, float speed);
    void PlayAndLoopAnimation(const std::string& layerName, std::vector<std::string>& animationNames, float speed);
    void SetAnimationModeToAnimated();
    void SetAnimationModeToBindPose();
    void SetAnimationModeToRagdoll();
    void SetAnimationModeToRagdollV2();
    void SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetMeshMaterialByMeshIndex(int meshIndex, const std::string& materialName);
    void SetMeshToRenderAsGlassByMeshIndex(const std::string& materialName);
    void SetMeshFurLength(const std::string& meshName, float furLength);
    void SetMeshFurShellDistanceAttenuation(const std::string& meshName, float furShellDistanceAttenuation);
    void SetMeshFurUVScale(const std::string& meshName, float uvScale);
    void SetMeshEmissiveColorTextureByMeshName(const std::string& meshName, const std::string& textureName);
    void SetMeshWoundMaterialByMeshName(const std::string& meshName, const std::string& textureName);
	void SetAllMeshMaterials(const std::string& materialName);
    void SetRagdoll(const std::string& ragdollName, float ragdollTotalWeight);
    void EnableCameraMatrix();
    void SetCameraMatrix(const glm::mat4& matrix);
    void DrawBones(int exclusiveViewportIndex = -1);
    void DrawBoneTangentVectors(float size = 0.1f, int exclusiveViewportIndex = -1);
    void SubmitForSkinning();
    void SetExclusiveViewportIndex(int index);
    void SetIgnoredViewportIndex(int index);
    void SetBaseTransfromIndex(int index);   
    void EnableDrawingForAllMesh();
    void EnableDrawingForMeshByMeshName(const std::string& meshName);
    void DisableDrawingForMeshByMeshName(const std::string& meshName);
    void PrintNodeNames();
    void PrintMeshNames();
    void EnableBlendingByMeshIndex(int index);
    void SetAdditiveTransform(const std::string& nodeName, const glm::mat4& matrix);
    void PauseAllAnimationLayers();
    void SetGoldFlag(bool flag);
    void SetRagdollV2Id(uint64_t ragdollV2Id);

    bool AnimationIsPastFrameNumber(const std::string& animationLayerName, int frameNumber);
    bool AnimationByNameIsComplete(const std::string& name);
    bool IsAllAnimationsComplete();
    void EnableRendering();
    void DisableRendering();

    const glm::mat4 GetModelMatrix();
    const glm::mat4 GetBindPoseByBoneName(const std::string& name);
    const glm::mat4 GetAnimatedTransformByBoneName(const std::string& name);
    const glm::mat4 GetBoneWorldMatrix(const std::string& boneName);
    const glm::vec3 GetBoneWorldPosition(const std::string& boneName);
    const uint32_t GetAnimationFrameNumber(const std::string& animationLayerName);
    const uint32_t GetBoneIndex(const std::string& boneName);
    const uint32_t GetVerteXCount();
        
    SkinnedModel* GetSkinnedModel()                                 { return m_skinnedModel; }
    bool RenderingEnabled()                                         { return m_renderingEnabled; }
    bool IsGold() const                                             { return m_isGold; }
    const uint64_t& GetObjectId() const                             { return m_objectId; }
    const uint64_t& GetRagdollId() const                            { return m_ragdollId; }
    const uint32_t GetBaseTransfromIndex() const                    { return baseTransformIndex; }
    const uint32_t& GetIgnoredViewportIndex() const                 { return m_ignoredViewportIndex; };
    const uint32_t& GetExclusiveViewportIndex() const               { return m_exclusiveViewportIndex; };
    const glm::vec3 GetScale() const                                { return m_transform.scale; }
    const std::vector<RenderItem>& GetRenderItems()                 { return m_renderItems; }
    const std::vector<glm::mat4>& GetGlobalBlendedNodeTransforms()  { return m_animator.m_globalBlendedNodeTransforms; }
    const std::vector<glm::mat4>& GetBoneSkinningMatrices()         { return m_boneSkinningMatrices; }
    const std::string& GetName() const                              { return m_name; }

private:
    void UpdateBoneTransformsFromRagdoll();
    void UpdateBoneTransformsFromRagdollV2();

    AnimationMode m_animationMode = AnimationMode::BINDPOSE;
    Animator m_animator;
    SkinnedModel* m_skinnedModel = nullptr;
    Transform m_transform;
    glm::mat4 m_cameraMatrix = glm::mat4(1);
    std::string m_name = "";
    std::vector<MeshRenderingEntry> m_meshRenderingEntries;
    std::vector<RenderItem> m_renderItems;
    std::vector<glm::mat4> m_boneSkinningMatrices;
    std::vector<uint32_t> m_skinnedBufferIndices;
    std::vector<int32_t> m_woundMaskTextureIndices;
    std::unordered_map<std::string, unsigned int> m_boneMapping;
    uint64_t m_objectId = 0;
    uint64_t m_ragdollId = 0;   // REMOVE ME WHEN U CAN
    uint64_t m_ragdollV2Id = 0;
    uint32_t m_ignoredViewportIndex = -1;
    uint32_t m_exclusiveViewportIndex = -1;
    uint32_t baseTransformIndex = -1;
    bool m_useCameraMatrix = false;
    bool m_isGold = false;
    bool m_renderingEnabled = true;
};