#include "Staircase.h"
#include "Renderer/Renderer.h"
#include "HellLogging.h"

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
    stairs.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
    stairs.rigidDynamic.convexMeshModelName = "CollisionMesh_Stairs";
    stairs.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
    stairs.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    stairs.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
    stairs.addtoNavMesh = true;

    int stepCount = 7;
    for (int i = 0; i < stepCount; i++) {
        MeshNodes& meshNodes = m_meshNodesList.emplace_back();
        meshNodes.Init(id, "Stairs", meshNodeCreateInfoSet);
        meshNodes.SetMeshMaterials("Stairs");
    }

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
    m_renderItems.clear();

    for (int i = 0; i < m_meshNodesList.size(); i++) {
      
        Transform transform;
        transform.position.y = 0.4375f * i;
        transform.position.z = 0.45f * i;              

        m_meshNodesList[i].Update(m_modelMatrix * transform.to_mat4());
        const std::vector<RenderItem>& renderItems = m_meshNodesList[i].GetRenderItems();
        m_renderItems.insert(m_renderItems.end(), renderItems.begin(), renderItems.end());
    }

    //RenderDebug();
}

void Staircase::RenderDebug() {
    glm::vec3 p1 = GetPosition();
    glm::vec3 p2 = GetPosition() + (m_worldForward * 0.5f);
    Renderer::DrawPoint(p1, YELLOW);
    Renderer::DrawPoint(p2, YELLOW);
    Renderer::DrawLine(p1, p2, YELLOW);
}

void Staircase::CleanUp() {
    for (int i = 0; i < m_meshNodesList.size(); i++) {
        m_meshNodesList[i].CleanUp();
    }
}

void Staircase::RecomputeModelMatrix() {
    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;
    m_modelMatrix = transform.to_mat4();
    m_worldForward = glm::vec3(m_modelMatrix * glm::vec4(m_localForward, 0.0f));
}