#include "Door.h"
#include "Audio/Audio.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util.h"
#include "World/World.h"

Door::Door(uint64_t id, const DoorCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;
	m_createInfo = createInfo;
	m_spawnOffset = spawnOffset;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

	std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

	MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
	door.meshName = "Door";
	door.materialName = "Door";
	door.openable.additionalTriggerMeshNames = { "Door_Handle", "Door_Hinges", "Door_Lock" };
    door.type = MeshNodeType::OPENABLE;
	door.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
	door.openable.initialOpenState = OpenState::CLOSED;
	door.openable.minOpenValue = 0.0f;
	door.openable.maxOpenValue = m_createInfo.maxOpenValue;
	door.openable.openSpeed = 5.208f;
	door.openable.closeSpeed = 5.208f;
	door.openable.openingAudio = "Door_Open.wav";
	door.openable.closingAudio = "Door_Open.wav";
	door.aabbCollision = true;

	MeshNodeCreateInfo& doorHandle = meshNodeCreateInfoSet.emplace_back();
    doorHandle.meshName = "Door_Handle";
    doorHandle.materialName = "Door";

	MeshNodeCreateInfo& doorHinges = meshNodeCreateInfoSet.emplace_back();
    doorHinges.meshName = "Door_Hinges";
    doorHinges.materialName = "Door";

	MeshNodeCreateInfo& doorLock = meshNodeCreateInfoSet.emplace_back();
    doorLock.meshName = "Door_Lock";
    doorLock.materialName = "Door";

	MeshNodeCreateInfo& doorFrame = meshNodeCreateInfoSet.emplace_back();
	doorFrame.meshName = "Door_Frame";
    doorFrame.materialName = "Door";

	MeshNodeCreateInfo& doorFrameHinges = meshNodeCreateInfoSet.emplace_back();
	doorFrameHinges.meshName = "Door_Frame_Hinges";
	doorFrameHinges.materialName = "Door";

	MeshNodeCreateInfo& doorFrameInner = meshNodeCreateInfoSet.emplace_back();
    doorFrameInner.meshName = "Door_Frame_Inner";
    doorFrameInner.materialName = "Door";
	
    MeshNodeCreateInfo& doorFrameLock = meshNodeCreateInfoSet.emplace_back();
    doorFrameLock.meshName = "Door_Frame_Lock";
    doorFrameLock.materialName = "Door";

	m_meshNodes.Init(id, "Door", meshNodeCreateInfoSet);

    //PhysicsFilterData filterData;
    //filterData.raycastGroup = RAYCAST_ENABLED;
    //filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    //filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);

    UpdateFloor();
}

void Door::UpdateFloor() {
    float half_w = 0.05f;
    float half_d = 0.4f;

    //half_w = 0.1f;
	//half_d = 0.3f;


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
	m_meshNodes.CleanUp();
}

void Door::Update(float deltaTime) {
    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;

    m_meshNodes.Update(transform.to_mat4());


    //Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Door", "Door");
    //if (mesh) {
    //    AABB aabb = AABB(mesh->aabbMin, mesh->aabbMax);
    //
    //
	//	Transform transform;
	//	transform.position = m_position;
	//	transform.rotation = m_rotation;
    //
	//	Transform openTransform;
	//	openTransform.rotation.y = -m_createInfo.maxOpenValue;
    //
	//	glm::vec3 origin = transform.to_mat4() * mesh->localTransform * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMax.z, 1.0f);
	//	glm::vec3 oppositePointClosed = transform.to_mat4() * mesh->localTransform * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMin.z, 1.0f);
	//	glm::vec3 oppositePointOpen = transform.to_mat4() * mesh->localTransform * openTransform.to_mat4() * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMin.z, 1.0f);
    //
	//	Renderer::DrawPoint(origin, YELLOW);
	//	Renderer::DrawPoint(oppositePointClosed, RED);
	//	Renderer::DrawPoint(oppositePointOpen, GREEN);
    //
    //    int segmentCount = 10;
    //    float openIncrement = m_createInfo.maxOpenValue / (float)segmentCount;
    //    for (int i = 0; i < segmentCount; i++) {
    //
	//		Transform openTransform;
	//		openTransform.rotation.y = i * -openIncrement;
	//		glm::vec3 oppositePoint = transform.to_mat4() * mesh->localTransform * openTransform.to_mat4() * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMin.z, 1.0f);
	//		Renderer::DrawPoint(oppositePoint, BLUE);
    //    }
    //}

    //Renderer::DrawLine()
}

void Door::SetPosition(glm::vec3 position) {
    m_createInfo.position = position;
    m_position = position;
}