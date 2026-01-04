#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

#include "Input/Input.h"
#include "World/World.h"
#include "Renderer/Renderer.h"

PickUp::PickUp(uint64_t id, const PickUpCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_objectId = id;

    ItemInfo* inventoryItemInfo = Bible::GetItemInfoByName(createInfo.name);
    if (!inventoryItemInfo) return; // Should never happen

    m_initialTransform.position = createInfo.position + spawnOffset.translation;
    m_initialTransform.rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    Bible::ConfigureMeshNodesByItemName(id, inventoryItemInfo->GetName(), m_meshNodes, true);
}

void PickUp::Update(float deltaTime) {
    m_modelMatrix = m_initialTransform.to_mat4();
    m_meshNodes.Update(GetModelMatrix());

    if (m_firstFrame && m_createInfo.disablePhysicsAtSpawn) {
        m_meshNodes.SleepAllPhysics();
    }

    m_firstFrame = false;

    m_respawnCounter += deltaTime;

    if (m_despawned) {
        std::cout << "m_respawnCounter: " << m_respawnCounter << "\n";
    }

    if (m_despawned && m_respawnCounter >= 0.0f) {
        m_despawned = false;
        m_meshNodes.ForceDirty();
    }

}

void PickUp::Despawn() {
    m_respawnCounter = -8.0f;
    m_despawned = true;
    //SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    //std::cout << "DESPAWNDED\n";
}

void PickUp::CleanUp() {
    m_meshNodes.CleanUp();
}

void PickUp::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_initialTransform.position = position;
    m_meshNodes.ResetFirstFrame();
}

void PickUp::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_initialTransform.rotation = rotation;
    m_meshNodes.ResetFirstFrame();
}

void PickUp::SetDisabledPhysicsAtSpawnState(bool state) {
    m_createInfo.disablePhysicsAtSpawn = state;
}

void PickUp::SetRespawnState(bool state) {
    m_createInfo.respawn = state;
}