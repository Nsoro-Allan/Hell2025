#pragma once
#include "Types/Renderer/MeshBuffer.h"
#include "Physics/Physics.h"
#include "RagdollInfo.h"
#include "Types.h"
#include "Types/Game/AnimatedGameObject.h"

struct RagdollV2 {
    void Init(glm::vec3 spawnPosition, glm::vec3 spawnEulerRotation, const std::string& ragdollName);
    void CleanUp();
    void Update();
    void DisableSimulation();
    void EnableSimulation();
    void SetToInitialPose();
    void AddForce(uint64_t physicsId, glm::vec3 force);

    bool IsInMotion();
    AABB GetWorldSpaceAABB();

    void SetRigidGlobalPosesFromAnimatedGameObject(AnimatedGameObject* animatedGameObject);

    uint64_t GetRagdollId()                     { return m_ragdollId; }
    MeshBuffer& GetMeshBuffer()                 { return m_meshBuffer; }
    const std::string& GetRagdollName() const   { return m_ragdollName; }
    glm::vec3 GetMarkerColorByRigidIndex(uint32_t index) const;
    glm::mat4 GetModelMatrixByRigidIndex(uint32_t index) const;

    std::vector<std::string> m_markerBoneNames;
    std::vector<PxRigidDynamic*> m_pxRigidDynamics;

private:
    void AddMarkerMeshData(RagdollMarker& marker);

    std::vector<PxD6Joint*> m_pxD6Joints;
    std::vector<glm::vec3> m_markerColors;
    std::string m_ragdollName;
    MeshBuffer m_meshBuffer;
    Transform m_spawnTransform;
    uint64_t m_ragdollId;
    float m_scale = 1.0f;
    bool m_simulationEnabled = false;
};