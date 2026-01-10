#include "Door.h"
#include "Audio/Audio.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Managers/OpenableManager.h"
#include "UniqueID.h"
#include "Util.h"
#include "World/World.h"

#include "Core/Game.h"

Door::Door(uint64_t id, DoorCreateInfo& createInfo, SpawnOffset& spawnOffset) {
    m_objectId = id;
	m_createInfo = createInfo;
	m_spawnOffset = spawnOffset;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    // Sensible defaults if for whatever reason your map file was missing this field
    if (m_createInfo.type == DoorType::UNDEFINED)                           m_createInfo.type = DoorType::STANDARD_A;
    if (m_createInfo.materialTypeFront == DoorMaterialType::UNDEFINED)      m_createInfo.materialTypeFront = DoorMaterialType::RESIDENT_EVIL;
    if (m_createInfo.materialTypeBack == DoorMaterialType::UNDEFINED)       m_createInfo.materialTypeBack = DoorMaterialType::RESIDENT_EVIL;
    if (m_createInfo.materialTypeFrameFront == DoorMaterialType::UNDEFINED) m_createInfo.materialTypeFrameFront = DoorMaterialType::RESIDENT_EVIL;
    if (m_createInfo.materialTypeFrameBack == DoorMaterialType::UNDEFINED)  m_createInfo.materialTypeFrameBack = DoorMaterialType::RESIDENT_EVIL;

    Bible::ConfigureDoorMeshNodes(id, m_createInfo, &m_meshNodes);

    if (m_createInfo.deadLockedAtInit) {
        m_deadLocked = true;

        // Iterate the mesh nodes, find any openable ID, and lock the cunt
        for (const MeshNode& meshNode : m_meshNodes.GetNodes()) {
            if (meshNode.openableId != 0) {
                OpenableManager::LockOpenablebyId(meshNode.openableId);
            }
        }
    }

    UpdateFloor();
    UpdateWorldForward();
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

    // DebugDraw();

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

void Door::UpdateWorldForward() {
    Transform transform;
    transform.rotation = m_rotation;
    m_worldForward = glm::vec3(transform.to_mat4() * glm::vec4(m_localForward, 1.0f));
}

bool Door::CameraFacingDoorWorldForward(const glm::vec3& cameraPositon, const glm::vec3& cameraForward) {
    glm::vec3 toCamera = cameraPositon - GetPosition();
    glm::vec3 toDoor = GetPosition() - cameraPositon;

    bool cameraOnFrontSide = glm::dot(m_worldForward, toDoor) > 0.0f;
    bool doorInFrontOfCamera = glm::dot(cameraForward, toDoor) > 0.0f;

    return !(cameraOnFrontSide && doorInFrontOfCamera);
}

void Door::DebugDraw() {

    glm::vec4 color = GREEN;

    Player* player = Game::GetLocalPlayerByIndex(0);
    if (!player) return;

    if (CameraFacingDoorWorldForward(player->GetCameraPosition(), player->GetCameraForward())) {
        color = GREEN;
    }
    else {
        color = RED;
    }

    glm::vec3 p1 = GetPosition();
    glm::vec3 p2 = GetPosition() + m_worldForward;
    Renderer::DrawLine(p1, p2, color);
    Renderer::DrawPoint(p1, color);
    Renderer::DrawPoint(p2, color);
}


void Door::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_position = position;
}

void Door::SetRotationY(float value) {
    m_createInfo.rotation.y = value;
    m_rotation.y = value;
}

void Door::SetEditorName(const std::string& name) {
    m_createInfo.editorName = name;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetType(DoorType type) {
    m_createInfo.type = type;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetFrontMaterial(DoorMaterialType type) {
    m_createInfo.materialTypeFront = type;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetBackMaterial(DoorMaterialType type) {
    m_createInfo.materialTypeBack = type;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetFrameFrontMaterial(DoorMaterialType type) {
    m_createInfo.materialTypeFrameFront = type;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetFrameBackMaterial(DoorMaterialType type) {
    m_createInfo.materialTypeFrameBack = type;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetDeadLockState(bool value) {
    m_createInfo.hasDeadLock = value;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}

void Door::SetDeadLockedAtInitState(bool value) {
    m_createInfo.deadLockedAtInit = value;
    Bible::ConfigureDoorMeshNodes(m_objectId, m_createInfo, &m_meshNodes);
}