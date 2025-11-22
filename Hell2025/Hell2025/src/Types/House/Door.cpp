#include "Door.h"
#include "Audio/Audio.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "UniqueID.h"
#include "Util.h"
#include "World/World.h"

Door::Door(uint64_t id, const DoorCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;
    //m_frameObjectId = UniqueID::GetNext(ObjectType::DOOR_FRAME);

    m_createInfo = createInfo;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    m_material = AssetManager::GetMaterialByName("Door");
    m_doorModel = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("Door"));
    m_frameModel = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("DoorFrame"));

    PhysicsFilterData filterData;
    filterData.raycastGroup = RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);

    Transform doorFrameTransform;
    doorFrameTransform.position = m_position;
    doorFrameTransform.rotation = m_rotation;

    Transform shapeOffset;
    shapeOffset.position.x = -0.005f;

    glm::vec3 boxExtents = glm::vec3(DOOR_DEPTH, DOOR_HEIGHT, DOOR_WIDTH);
    m_physicsId = Physics::CreateRigidStaticBoxFromExtents(doorFrameTransform, boxExtents, filterData, shapeOffset);

    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_physicsId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::DOOR;
    Physics::SetRigidStaticUserData(m_physicsId, userData);

    m_lifeTime = 0;

    UpdateFloor();
}

void Door::UpdateFloor() {
    float half_w = 0.05f;
    float half_d = 0.4f;

    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;

    HousePlaneCreateInfo createInfo;
    createInfo.p0 = glm::vec3(transform.to_mat4() * glm::vec4(-half_w, 0.0f, -half_d, 1.0f));
    createInfo.p1 = glm::vec3(transform.to_mat4() * glm::vec4(-half_w, 0.0f, +half_d, 1.0f));
    createInfo.p2 = glm::vec3(transform.to_mat4() * glm::vec4(+half_w, 0.0f, +half_d, 1.0f));
    createInfo.p3 = glm::vec3(transform.to_mat4() * glm::vec4(+half_w, 0.0f, -half_d, 1.0f));
    createInfo.parentDoorId = GetObjectId();
    createInfo.type = HousePlaneType::FLOOR;

    createInfo.textureScale = 0.4f;
    createInfo.materialName = "FloorBoards";

    World::AddHousePlane(createInfo, SpawnOffset());
}

void Door::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
}

void Door::Update(float deltaTime) {
    m_movedThisFrame = (m_lifeTime == 0 || m_openingState == OpeningState::OPENING || m_openingState == OpeningState::CLOSING);
    m_lifeTime++;

    float openSpeed = 5.208f;
    if (m_openingState == OpeningState::OPENING) {
        m_currentOpenRotation -= openSpeed * deltaTime;
        if (m_currentOpenRotation < -m_maxOpenRotation) {
            m_currentOpenRotation = -m_maxOpenRotation;
            m_openingState = OpeningState::OPEN;
        }
    }
    if (m_openingState == OpeningState::CLOSING) {
        m_currentOpenRotation += openSpeed * deltaTime;
        if (m_currentOpenRotation > 0) {
            m_currentOpenRotation = 0;
            m_openingState = OpeningState::CLOSED;
        }
    }

    Transform doorFrameTransform;
    doorFrameTransform.position = m_position;
    doorFrameTransform.rotation = m_rotation;
    m_frameModelMatrix = doorFrameTransform.to_mat4();

    Transform doorTransform;
    doorTransform.position = glm::vec3(0.058520f, 0.0f, 0.39550f);
    doorTransform.rotation.y = m_currentOpenRotation;
    m_doorModelMatrix = m_frameModelMatrix * doorTransform.to_mat4();

    // Update PhysX transform
    Transform physxOffsetTransform;
    physxOffsetTransform.position.z = DOOR_WIDTH * -0.5f;
    physxOffsetTransform.position.y = DOOR_HEIGHT * 0.5f;
    physxOffsetTransform.position.x = DOOR_DEPTH * -0.5f;
    glm::mat4 globalPoseMatrix = m_doorModelMatrix * physxOffsetTransform.to_mat4();
    Physics::SetRigidStaticGlobalPose(m_physicsId, globalPoseMatrix);

    // Calculate interact position
    glm::mat4 interactOffset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, DOOR_HEIGHT / 2, -DOOR_WIDTH / 2));
    m_interactPosition = (m_doorModelMatrix * interactOffset)[3];

    UpdateRenderItems();
}

void Door::SetPosition(glm::vec3 position) {
    m_createInfo.position = position;
    m_position = position;
}

void Door::UpdateRenderItems() {
    m_renderItems.clear();

    // Handle missing material
    if (!m_material) {
        m_material = AssetManager::GetDefaultMaterial();
    }
    
    // Bail if models are invalid
    if (!m_doorModel || !m_frameModel) return;

    for (const uint32_t& meshIndex : m_doorModel->GetMeshIndices()) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.modelMatrix = m_doorModelMatrix;
        renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.baseColorTextureIndex = m_material->m_basecolor;
        renderItem.rmaTextureIndex = m_material->m_rma;
        renderItem.normalMapTextureIndex = m_material->m_normal;
        renderItem.objectType = Util::EnumToInt(ObjectType::DOOR);
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }

    for (const uint32_t& meshIndex : m_frameModel->GetMeshIndices()) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.modelMatrix = m_frameModelMatrix;
        renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.baseColorTextureIndex = m_material->m_basecolor;
        renderItem.rmaTextureIndex = m_material->m_rma;
        renderItem.normalMapTextureIndex = m_material->m_normal;
        renderItem.objectType = Util::EnumToInt(ObjectType::DOOR_FRAME);
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(0, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit); // ZERO ID!
    }
}

void Door::SubmitRenderItems() {
    RenderDataManager::SubmitRenderItems(m_renderItems);
    if (Editor::GetSelectedObjectId() == m_objectId) {
        RenderDataManager::SubmitOutlineRenderItems(m_renderItems);
    }
}

void Door::Interact() {
    if (m_openingState == OpeningState::CLOSED) {
        m_openingState = OpeningState::OPENING;
        Audio::PlayAudio("Door_Open.wav", 1.0f);
    }
    else if (m_openingState == OpeningState::OPEN) {
        m_openingState = OpeningState::CLOSING;
        Audio::PlayAudio("Door_Open.wav", 1.0f);
    }
}