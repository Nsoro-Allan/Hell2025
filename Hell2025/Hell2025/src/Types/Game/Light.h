#pragma once
#include "Camera/Frustum.h"
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

    const glm::mat4 GetProjectionView(int index) const      { return m_projectionTransforms[index]; }
    const float GetRadius() const                           { return m_radius; }
    const float GetStrength() const                         { return m_strength; }
    const bool IsDirty() const                              { return m_dirty; }
    const glm::vec3& GetPosition() const                    { return m_position; }
    const glm::vec3& GetColor() const                       { return m_color; }
    const uint64_t GetObjectId() const                      { return m_objectId; }
    const LightCreateInfo& GetCreateInfo() const            { return m_createInfo; };
    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }

private:
    void UpdateRenderItems();
    void UpdateDirtyState();

    bool m_forcedDirty = false;
    bool m_dirty = true;
    uint64_t m_objectId = 0;
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    float m_radius = 6.0f;
    float m_strength = 1.0f;
    LightType m_type = LightType::LAMP_POST;
    Material* m_material = nullptr;
    Model* m_modelMain = nullptr;
    Model* m_modelCord = nullptr;
    Model* m_modelCordMount = nullptr;
    Transform m_transformMain;
    Transform m_transformCord;
    Transform m_transformCordMount;
    std::vector<RenderItem> m_renderItems;
    LightCreateInfo m_createInfo;
    Frustum m_frustum[6];
    glm::mat4 m_projectionTransforms[6];
    glm::mat4 m_viewMatrix[6];

    // Your light is basically made of 3 things. A main model/transform which for most lights is the main model.
    // Hanging lights have 2 other models, the cord, and the cord mount.
};