#include "SpawnPoint.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"

SpawnPoint::SpawnPoint(glm::vec3 position, glm::vec3 camEuler) {
    m_position = position;
    m_camEuler = camEuler;
}

void SpawnPoint::Init() {
    m_objectId = UniqueID::GetNext(ObjectType::SPAWN_POINT);

    // Create physics shape
    Transform transform;
    transform.position = m_position;

    float mass = 1.0f;
    glm::vec3 cubeExtents = glm::vec3(1.0f);

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::NO_COLLISION;
    filterData.collidesWith = CollisionGroup::NO_COLLISION;

    m_rigidStaticId = Physics::CreateRigidStaticBoxFromExtents(transform, cubeExtents, filterData);

    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_rigidStaticId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::SPAWN_POINT;
    Physics::SetRigidStaticUserData(m_rigidStaticId, userData);
}

void SpawnPoint::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_rigidStaticId);
}

void SpawnPoint::DrawDebugCube() {
    glm::vec3 aabbMin = glm::vec3(-0.5f);
    glm::vec3 aabbMax = glm::vec3(0.5f);

    Transform transform;
    transform.position = m_position;

    AABB aabb = AABB(aabbMin, aabbMax);

    Renderer::DrawAABB(aabb, OUTLINE_COLOR, transform.to_mat4());
}