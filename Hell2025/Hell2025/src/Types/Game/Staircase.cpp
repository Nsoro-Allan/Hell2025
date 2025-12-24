#include "Staircase.h"
#include "Renderer/Renderer.h"

Staircase::Staircase(uint64_t id, StaircaseCreateInfo& createInfo, SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

    MeshNodeCreateInfo& stairs = meshNodeCreateInfoSet.emplace_back();
    stairs.meshName = "Stairs";
    stairs.materialName = "Stairs";
    stairs.rigidDynamic.createObject = true;
    stairs.rigidDynamic.kinematic = true;
    stairs.rigidDynamic.shapeType = PhysicsShapeType::BOX;
    stairs.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
    stairs.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    stairs.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
    stairs.addtoNavMesh = true;

    m_meshNodes.Init(id, "Stairs", meshNodeCreateInfoSet);
    m_meshNodes.SetMeshMaterials("Stairs");

    RecomputeModelMatrix();
}

void Staircase::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_position = position;
    RecomputeModelMatrix();
}

void Staircase::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_rotation = rotation;
    RecomputeModelMatrix();
}

void Staircase::Update(float deltaTime) {
    m_meshNodes.Update(m_modelMatrix);
    RenderDebug();
}

void Staircase::RenderDebug() {
    glm::vec3 p1 = GetPosition();
    glm::vec3 p2 = GetPosition() + (m_worldForward * 0.5f);
    Renderer::DrawPoint(p1, YELLOW);
    Renderer::DrawPoint(p2, YELLOW);
    Renderer::DrawLine(p1, p2, YELLOW);
}

void Staircase::CleanUp() {
    m_meshNodes.CleanUp();
}

void Staircase::RecomputeModelMatrix() {
    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;
    m_modelMatrix = transform.to_mat4();
    m_worldForward = glm::vec3(m_modelMatrix * glm::vec4(m_localForward, 0.0f));
}