#include "Ladder.h"
#include "Renderer/Renderer.h"

Ladder::Ladder(uint64_t id, LadderCreateInfo& createInfo, SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;

    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

    MeshNodeCreateInfo& ladder = meshNodeCreateInfoSet.emplace_back();
    ladder.meshName = "Ladder";
    ladder.materialName = "Ladder";
    ladder.rigidDynamic.createObject = true;
    ladder.rigidDynamic.kinematic = true;
    ladder.rigidDynamic.shapeType = PhysicsShapeType::BOX;
    ladder.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
    ladder.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    ladder.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
    ladder.addtoNavMesh = true;

    m_meshNodes.Init(id, "Ladder", meshNodeCreateInfoSet);
    m_meshNodes.EnableCSMShadows();

    RecomputeModelMatrix();
}

void Ladder::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_position = position;
    RecomputeModelMatrix();
}

void Ladder::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_rotation = rotation;
    RecomputeModelMatrix();
}

void Ladder::Update(float deltaTime) {
    m_meshNodes.Update(m_modelMatrix);

    // Clean me up
    glm::vec3 boundsMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 boundsMax = glm::vec3(std::numeric_limits<float>::lowest());
    for (const RenderItem& renderItem : m_meshNodes.GetRenderItems()) {
        boundsMin = glm::min(boundsMin, glm::vec3(renderItem.aabbMin));
        boundsMax = glm::max(boundsMax, glm::vec3(renderItem.aabbMax));
    }
    float shrink = 0.125;
    boundsMin.x += shrink;
    boundsMin.z += shrink;
    boundsMax.x -= shrink;
    boundsMax.z -= shrink;
    boundsMax.y += 1.1;
    m_overlapHitAABB = AABB(boundsMin, boundsMax);
    // Clean me up

    //RenderDebug();
}

void Ladder::RenderDebug() {
    glm::vec3 p1 = GetPosition();
    glm::vec3 p2 = GetPosition() + (m_worldForward * 0.5f);
    Renderer::DrawPoint(p1, YELLOW);
    Renderer::DrawPoint(p2, YELLOW);
    Renderer::DrawLine(p1, p2, YELLOW);
    Renderer::DrawAABB(m_overlapHitAABB, YELLOW);
}

void Ladder::CleanUp() {
    m_meshNodes.CleanUp();
}

void Ladder::RecomputeModelMatrix() {
    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;
    m_modelMatrix = transform.to_mat4();
    m_worldForward = glm::vec3(m_modelMatrix * glm::vec4(m_localForward, 0.0f));
}