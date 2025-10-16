#include "ChristmasPresent.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Util/Util.h"
#include "UniqueID.h"

ChristmasPresent::ChristmasPresent(const ChristmasPresentCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_createInfo = createInfo;

    CreatePhysicsObject();
    m_objectId = UniqueID::GetNextObjectId(ObjectType::CHRISMAS_PRESENT);

    if (m_createInfo.type == ChristmasPresentType::SMALL) {
        m_model = AssetManager::GetModelByName("ChristmasPresentSmall");
        switch (Util::RandomInt(0, 3)) {
            case 0: m_material = AssetManager::GetMaterialByName("PresentSmallRed"); break;
            case 1: m_material = AssetManager::GetMaterialByName("PresentSmallGreen"); break;
            case 2: m_material = AssetManager::GetMaterialByName("PresentSmallYellow"); break;
            case 3: m_material = AssetManager::GetMaterialByName("PresentSmallBlue"); break;
            default: break;
        }
    }
}

void ChristmasPresent::CreateRenderItems() {
    // Build render items
    m_renderItems.clear();

    if (!m_model) {
        std::cout << "Could not get ChristmasTree model\n";
        return;
    }

    if (!m_material) {
        std::cout << "Could not get ChristmasTree mateial\n";
        return;
    }

    for (uint32_t meshIndex : m_model->GetMeshIndices()) {
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.objectType = (int)ObjectType::GAME_OBJECT;
        renderItem.modelMatrix = m_modelMatrix;
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.castShadows = false;
        if (m_material) {
            renderItem.baseColorTextureIndex = m_material->m_basecolor;
            renderItem.normalMapTextureIndex = m_material->m_normal;
            renderItem.rmaTextureIndex = m_material->m_rma;
        }
        Util::UpdateRenderItemAABB(renderItem);
    }
}

void ChristmasPresent::Update(float deltaTime) {

    if (Physics::RigidDynamicExists(m_rigidDynamicId)) {
        m_modelMatrix = Physics::GetRigidDynamicWorldMatrix(m_rigidDynamicId);
    }


   //Transform transform;
   //transform.position = m_position;
   //transform.rotation = m_rotation;
   //m_modelMatrix = transform.to_mat4();

    CreateRenderItems();
}

void ChristmasPresent::CleanUp() {
    Physics::MarkRigidDynamicForRemoval(m_rigidDynamicId);
}

void ChristmasPresent::CreatePhysicsObject() {

    glm::vec3 boxExtents = glm::vec3(0.0f);
    switch (m_createInfo.type) {
        case ChristmasPresentType::SMALL:  boxExtents = glm::vec3(0.2f); break;
        case ChristmasPresentType::MEDIUM: boxExtents = glm::vec3(1.0f); break;
        case ChristmasPresentType::LARGE:  boxExtents = glm::vec3(1.0f); break;
        default: break;
    }

    float mass = 0.8;

    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::GENERIC_BOUNCEABLE;
    filterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | GENERIC_BOUNCEABLE);

    m_rigidDynamicId = Physics::CreateRigidDynamicFromBoxExtents(transform, boxExtents, mass, filterData);

    PhysicsUserData userData;
    userData.physicsId = m_rigidDynamicId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_DYNAMIC;
    userData.objectType = ObjectType::GENERIC_BOUNCABLE;
    Physics::SetRigidDynamicUserData(m_rigidDynamicId, userData);
 }