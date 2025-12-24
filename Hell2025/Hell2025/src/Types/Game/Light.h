#pragma once
#include "Camera/Frustum.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"
#include "CreateInfo.h"
#include "HellTypes.h"


struct LightFlicker {
    float m_baseIntensity = 1.0f;
    float m_amplitude = 0.25f; // 0..1
    float m_responseHz = 12.0f; // smoothing speed
    float m_slowFrequencyHz = 2.0f;
    float m_midFrequencyHz = 6.5f;
    float m_fastFrequencyHz = 11.0f;
    float m_slowWeight = 0.60f;
    float m_midWeight = 0.30f;
    float m_fastWeight = 0.10f;
    float m_shapePower = 1.8f; // The higher this value, the lower the peaks
    glm::vec3 m_lowColor = glm::vec3(1.00f, 0.35f, 0.10f);
    glm::vec3 m_highColor = glm::vec3(1.00f, 0.75f, 0.35f);
    glm::vec3 m_currentColor = glm::vec3(1.0f);
    float m_currentFlicker = 0.8f; // internal state in [0,1]

    void Update(float deltaTime, float timeSeconds);
};

struct Light {
    Light() = default;
    Light(uint64_t id, LightCreateInfo& createInfo);
    Light(const Light&) = delete;
    Light& operator=(const Light&) = delete;
    Light(Light&&) noexcept = default;
    Light& operator=(Light&&) noexcept = default;
    ~Light() = default;

    void Update(float deltaTime);
    void SetColor(const glm::vec3& color);
    void SetRadius(float radius);
    void SetPosition(const glm::vec3& position);
    void SetStrength(float strengtht);
    void UpdateMatricesAndFrustum();
    void ForceDirty();
    void ConfigureMeshNodes();

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

    bool m_doFlicker = false;
    LightFlicker m_lightFlicker;

private:
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