#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"

struct Decal {
    Decal() = default;
    Decal(const Decal2CreateInfo& createInfo);
    Decal(const Decal&) = delete;
    Decal& operator=(const Decal&) = delete;
    Decal(Decal&&) noexcept = default;
    Decal& operator=(Decal&&) noexcept = default;
    ~Decal() = default;
    void Update();

    const glm::vec3 GetPosition() const         { return glm::vec3(m_worldMatrix[3]); }
    const glm::vec3 GetWorldNormal() const      { return glm::vec3(m_worldNormal); }
    const RenderItem& GetRenderItem() const     { return m_renderItem; }

private:
    const glm::mat4& GetParentWorldMatrix();

    DecalType m_type = DecalType::UNDEFINED;
    Decal2CreateInfo m_createInfo;
    Material* m_material = nullptr;
    RenderItem m_renderItem;
    glm::vec3 m_localPosition = glm::vec3(0.0f);
    glm::vec3 m_localNormal = glm::vec3(0.0f);
    glm::vec3 m_worldNormal = glm::vec3(0.0f);
    glm::mat4 m_worldMatrix = glm::mat4(1.0f);
    glm::mat4 m_localMatrix = glm::mat4(1.0f);

};


