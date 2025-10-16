#include "WallSegment.h"
#include "Modelling/Clipping.h"
#include "Physics/Physics.h"
#include "Util.h"
#include "UniqueID.h"

void WallSegment::Init(glm::vec3 start, glm::vec3 end, float height, uint64_t parentObjectId, const SpawnOffset& spawnOffset) {
    m_start = start;
    m_end = end;
    m_height = height;
    m_spawnOffset = spawnOffset;

    // Normal
    glm::vec3 dir = glm::normalize(m_end - m_start);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_normal = glm::normalize(glm::cross(dir, up));

    // Corners
    m_corners = {
        m_start,                                    // Bottom-left
        m_start + glm::vec3 (0.0f, m_height, 0.0f), // Top-left
        m_end + glm::vec3 (0.0f, m_height, 0.0f),   // Top-right
        m_end                                       // Bottom-right
    };

    // AABB
    m_aabb = Util::GetAABBFromPoints(m_corners);

    // Store parent id
    m_parentObjectId = parentObjectId;
}

void WallSegment::SetMeshIndex(uint32_t index) {
    m_meshIndex = index;
}

void WallSegment::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
}

void WallSegment::CreateVertexData(std::vector<ClippingCube>& clippingCubes, float texOffsetX, float texOffsetY, float texScale) {
    m_vertices.clear();
    m_indices.clear();

    // Clip the cubes from the wall
    Clipping::SubtractCubesFromWallSegment(*this, clippingCubes, m_vertices, m_indices);

    for (Vertex& vertex : m_vertices) {
        glm::vec3 origin = glm::vec3(0, 0, 0);

        // Correct any errors introduced by Clipper used integers the under hood
        float threshold = 0.01f;
        for (const glm::vec3& originalPosition : m_corners) {
            if (std::abs(vertex.position.x - originalPosition.x) < threshold) {
                vertex.position.x = originalPosition.x;
            }
            if (std::abs(vertex.position.y - originalPosition.y) < threshold) {
                vertex.position.y = originalPosition.y;
            }
            if (std::abs(vertex.position.z - originalPosition.z) < threshold) {
                vertex.position.z = originalPosition.z;
            }
        }

        // Update UVs
        origin = glm::vec3(0);
        vertex.uv = Util::CalculateUV(vertex.position - m_spawnOffset.translation, m_normal);
        vertex.uv *= texScale;
        vertex.uv.x += texOffsetX;
        vertex.uv.y += texOffsetY;
    }

    // Update normals and tangents
    for (int i = 0; i < m_indices.size(); i += 3) {
        Vertex& v0 = m_vertices[m_indices[i + 0]];
        Vertex& v1 = m_vertices[m_indices[i + 1]];
        Vertex& v2 = m_vertices[m_indices[i + 2]];
        Util::SetNormalsAndTangentsFromVertices(v0, v1, v2);
    }

    CreatePhysicsObject();
}

void WallSegment::CreatePhysicsObject() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);

    PhysicsFilterData filterData;
    filterData.raycastGroup = RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY | CHARACTER_CONTROLLER | ITEM_PICK_UP);

    m_physicsId = Physics::CreateRigidStaticTriangleMeshFromVertexData(Transform(), m_vertices, m_indices, filterData);
    m_objectId = UniqueID::GetNextObjectId(ObjectType::WALL_SEGMENT);

    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_physicsId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::WALL_SEGMENT;
    Physics::SetRigidStaticUserData(m_physicsId, userData);
}