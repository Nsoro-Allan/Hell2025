#pragma once
#include "HellTypes.h"
#include "HellEnums.h"
#include "CreateInfo.h"
#include "Types/Renderer/Model.h"

struct PickUp {
    PickUp() = default;
    void Init(PickUpCreateInfo createInfo);
    void SetPosition(glm::vec3 position);
    void Update(float deltaTime);
    void CleanUp();
    void UpdateRenderItems();
    //void MarkForRemoval();
    PickUpCreateInfo GetCreateInfo();

    //bool IsMarkedForRemoval()                           { return m_markedForRemoval; }
    const std::vector<RenderItem>& GetRenderItems()     { return m_renderItems; }
    const glm::vec3 GetPosition()                       { return m_modelMatrix[3]; }
    const glm::vec3& GetRotation()                      { return m_initialTransform.rotation; }
    const glm::mat4& GetModelMatrix()                   { return m_modelMatrix; }
    const uint64_t GetObjectId()                        { return m_objectId; }

private:
    Model* m_model = nullptr;
    std::vector<uint32_t> m_materialIndices;
    uint64_t m_physicsId = 0;
    uint64_t m_objectId = 0;
    PickUpType m_pickUpType = PickUpType::UNDEFINED;
    Transform m_initialTransform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    std::vector<RenderItem> m_renderItems;
    //bool m_markedForRemoval = false;
    bool m_isGold = false;

    void SetModel(const std::string& modelName);
    void SetMeshMaterial(const std::string& meshName, const std::string& materialName);
    void SetAllMeshMaterials(const std::string& materialName);
};