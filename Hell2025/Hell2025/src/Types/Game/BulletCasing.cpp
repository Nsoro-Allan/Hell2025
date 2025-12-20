#include "BulletCasing.h"
#include "Util.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"

BulletCasing::BulletCasing(BulletCasingCreateInfo createInfo) {
    m_materialIndex = createInfo.materialIndex;

    // Get model
    Model* model = AssetManager::GetModelByIndex(createInfo.modelIndex);
    if (!model) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from invalid model\n";
        return;
    }
    if (model->GetMeshCount() < 1) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from mesh count 0\n";
    }
    
    // Get mesh
    m_meshIndex = model->GetMeshIndices()[0];
    Mesh* mesh = AssetManager::GetMeshByIndex(m_meshIndex);
    if (!mesh) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from invalid mesh\n";
    }
    
    Transform transform;
    transform.position = createInfo.position;
    transform.rotation = createInfo.rotation;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
    filterData.collisionGroup = CollisionGroup::BULLET_CASING;
    filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

    glm::vec3 force = createInfo.force;
    glm::vec3 torque = glm::vec3(Util::RandomFloat(-10.0f, 10.0f), Util::RandomFloat(-10.0f, 10.0f), Util::RandomFloat(-10.0f, 10.0f));

    m_rigidDynamicId = Physics::CreateRigidDynamicFromBoxExtents(transform, mesh->extents, createInfo.mass, filterData, force, torque);
}

void BulletCasing::CleanUp() {
    Physics::MarkRigidDynamicForRemoval(m_rigidDynamicId);
}

const glm::mat4& BulletCasing::GetModelMatrix() {
    return m_modelMatrix;
}

void BulletCasing::Update(float deltaTime) {
    m_lifeTime += deltaTime;

    float maxLifeTime = 5.0f;
    if (m_lifeTime > maxLifeTime) {
        Physics::MarkRigidDynamicForRemoval(m_rigidDynamicId);
    }

    if (Physics::RigidDynamicExists(m_rigidDynamicId)) {
        m_modelMatrix = Physics::GetRigidDynamicWorldMatrix(m_rigidDynamicId);
    }
}

void BulletCasing::SubmitRenderItem() {
    Material* material = AssetManager::GetMaterialByIndex(GetMaterialIndex());
    
    RenderItem renderItem;
    renderItem.modelMatrix = GetModelMatrix();
    renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
    renderItem.baseColorTextureIndex = material->m_basecolor;
    renderItem.rmaTextureIndex = material->m_rma;
    renderItem.normalMapTextureIndex = material->m_normal;
    renderItem.meshIndex = GetMeshIndex();
    renderItem.castShadows = false;

    Util::UpdateRenderItemAABB(renderItem);
    RenderDataManager::SubmitRenderItem(renderItem);
}