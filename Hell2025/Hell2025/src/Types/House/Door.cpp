#include "Door.h"
#include "Audio/Audio.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util.h"
#include "World/World.h"

Door::Door(uint64_t id, DoorCreateInfo& createInfo, SpawnOffset& spawnOffset) {
    m_objectId = id;
	m_createInfo = createInfo;
	m_spawnOffset = spawnOffset;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    createInfo.type = DoorType::STAINED_GLASS;
    createInfo.materialTypeFront = DoorMaterialType::RESIDENT_EVIL;
    createInfo.materialTypeBack = DoorMaterialType::WHITE_PAINT;
    createInfo.materialTypeFrame = DoorMaterialType::RESIDENT_EVIL;

    Bible::ConfigureDoorMeshNodes(id, createInfo, m_meshNodes);

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