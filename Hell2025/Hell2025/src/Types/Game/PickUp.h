#pragma once
#include "HellTypes.h"
#include "HellEnums.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"

struct PickUp {
    PickUp() = default;
    void Init(PickUpCreateInfo createInfo);
    void SetPosition(glm::vec3 position);
    void Update(float deltaTime);
    void CleanUp();
    PickUpCreateInfo GetCreateInfo();

    const std::vector<RenderItem>& GetRenderItems()     { return m_meshNodes.GetRenderItems(); }
    const glm::vec3 GetPosition()                       { return m_modelMatrix[3]; }
    const glm::vec3& GetRotation()                      { return m_initialTransform.rotation; }
    const glm::mat4& GetModelMatrix()                   { return m_modelMatrix; }
    const uint64_t GetObjectId()                        { return m_objectId; }

private:
    uint64_t m_physicsId = 0;
    uint64_t m_objectId = 0;
    PickUpType m_pickUpType = PickUpType::UNDEFINED;
    Transform m_initialTransform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    bool m_isGold = false;
    MeshNodes m_meshNodes;
};