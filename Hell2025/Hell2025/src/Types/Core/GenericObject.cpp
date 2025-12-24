#include "GenericObject.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "HellLogging.h"
#include "Managers/OpenableManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util/Util.h"

GenericObject::GenericObject(uint64_t id, const GenericObjectCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;

    // FOR THE LOVE OF SATAN REMOVE ME!!!!!!!!!
    if (m_createInfo.type == GenericObjectType::PLANT_BLACKBERRIES ||
        m_createInfo.type == GenericObjectType::PLANT_TREE) {
        m_createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
    }

    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_transform.scale = m_createInfo.scale;
    m_objectId = id;

    Bible::ConfigureMeshNodes(id, m_createInfo.type, m_meshNodes);

}

void GenericObject::Update(float deltaTime) {
    m_meshNodes.Update(m_transform.to_mat4());
}

void GenericObject::CleanUp() {
    m_meshNodes.CleanUp();
}

void GenericObject::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_transform.position = position;
}

void GenericObject::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_transform.rotation = rotation;
};

glm::vec3 GenericObject::GetGizmoOffset() {
    //glm::vec3 aabbMin = m_meshNodes.m_worldspaceAABB.GetBoundsMin();
    //glm::vec3 aabbMax = m_meshNodes.m_worldspaceAABB.GetBoundsMax();
    //return ((aabbMin + aabbMax) * glm::vec3(0.5f)) - m_transform.position;
    return glm::vec3(0.0f);
}