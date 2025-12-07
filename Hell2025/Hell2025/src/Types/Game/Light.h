#pragma once
#include "Camera/Frustum.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"
#include "CreateInfo.h"
#include "HellTypes.h"

struct Light {
    Light() = default;
    Light(LightCreateInfo createInfo);

    void Update(float deltaTime);
    void SetPosition(glm::vec3 position);
    void UpdateMatricesAndFrustum();
    void ForceDirty();
    void BuildCord();

    Frustum* GetFrustumByFaceIndex(uint32_t faceIndex);

	MeshNodes& GetMeshNodes() { return m_meshNodes; }
    const glm::mat4 GetProjectionView(int index) const      { return m_projectionTransforms[index]; }
    const bool IsDirty() const                              { return m_dirty; }
    const float GetRadius() const                           { return m_createInfo.radius; }
    const float GetStrength() const                         { return m_createInfo.strength; }
    const glm::vec3& GetPosition() const                    { return m_createInfo.position; }
    const glm::vec3& GetColor() const                       { return m_createInfo.color; }
    const uint64_t GetObjectId() const                      { return m_objectId; }
    const LightCreateInfo& GetCreateInfo() const            { return m_createInfo; };
    const std::vector<RenderItem>& GetRenderItems() const   { return m_meshNodes.GetRenderItems(); }

private:
    void UpdateRenderItems();
    void UpdateDirtyState();

	MeshNodes m_meshNodes;
    bool m_forcedDirty = false;
    bool m_dirty = true;
    uint64_t m_objectId = 0;
    std::vector<RenderItem> m_renderItems;
    LightCreateInfo m_createInfo;
    Frustum m_frustum[6];
    glm::mat4 m_projectionTransforms[6];
    glm::mat4 m_viewMatrix[6];

    // Your light is basically made of 3 things. A main model/transform which for most lights is the main model.
    // Hanging lights have 2 other models, the cord, and the cord mount.
};