#include "HousePlane.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "UniqueID.h"
#include "Util.h"

HousePlane::HousePlane(uint64_t id, const HousePlaneCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;

    m_createInfo = createInfo;

    m_createInfo.p0 += spawnOffset.translation; // is this correct/safe?
    m_createInfo.p1 += spawnOffset.translation; // is this correct/safe?
    m_createInfo.p2 += spawnOffset.translation; // is this correct/safe?
    m_createInfo.p3 += spawnOffset.translation; // is this correct/safe?

    m_material = AssetManager::GetMaterialByName(m_createInfo.materialName);

    UpdateVertexDataFromCreateInfo();
}

void HousePlane::UpdateVertexDataFromCreateInfo() {
    m_p0 = m_createInfo.p0;
    m_p1 = m_createInfo.p1;
    m_p2 = m_createInfo.p2;
    m_p3 = m_createInfo.p3;

    m_vertices.clear();
    m_indices.clear();

    // Vertices
    m_vertices.push_back(m_p0);
    m_vertices.push_back(m_p1);
    m_vertices.push_back(m_p2);
    m_vertices.push_back(m_p3);

    // Indices
    m_indices = { 0, 1, 2, 2, 3, 0 };

    // Update UVs
    for (Vertex& vertex : m_vertices) {
        glm::vec3 origin = glm::vec3(0, 0, 0);
        origin = glm::vec3(0);
        vertex.uv = Util::CalculateUV(vertex.position, glm::vec3(0.0f, 1.0f, 0.0f));
        vertex.uv *= m_createInfo.textureScale;
        vertex.uv.x += m_createInfo.textureOffsetU;
        vertex.uv.y += m_createInfo.textureOffsetV;
    }

    // Update normals and tangents
    for (int i = 0; i < m_indices.size(); i += 3) {
        Vertex& v0 = m_vertices[m_indices[i + 0]];
        Vertex& v1 = m_vertices[m_indices[i + 1]];
        Vertex& v2 = m_vertices[m_indices[i + 2]];
        Util::SetNormalsAndTangentsFromVertices(v0, v1, v2);
    }

    Physics::MarkRigidStaticForRemoval(m_physicsId);
    CreatePhysicsObject();

    // Calculate worldspace center
    m_worldSpaceCenter = (m_p0 + m_p1 + m_p2 + m_p3) / 4.0f;
}

void HousePlane::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
    m_vertices.clear();
    m_indices.clear();
    m_objectId = 0;
    m_physicsId = 0;
    m_p0 = glm::vec3(0.0f);
    m_p1 = glm::vec3(0.0f);
    m_p2 = glm::vec3(0.0f);
    m_p3 = glm::vec3(0.0f);
    Material* m_material = nullptr;
}

void HousePlane::UpdateWorldSpaceCenter(glm::vec3 worldSpaceCenter) {
    glm::vec3 offset = worldSpaceCenter - m_worldSpaceCenter;
    m_createInfo.p0 += offset;
    m_createInfo.p1 += offset;
    m_createInfo.p2 += offset;
    m_createInfo.p3 += offset; 
    UpdateVertexDataFromCreateInfo();
}

void HousePlane::SetMaterial(const std::string& materialName) {
    m_createInfo.materialName = materialName;
    m_material = AssetManager::GetMaterialByName(materialName);
}

void HousePlane::SetMeshIndex(uint32_t index) {
    m_meshIndex = index;
}

void HousePlane::SetTextureScale(float value) {
    m_createInfo.textureScale = value;
    UpdateVertexDataFromCreateInfo();
}

void HousePlane::SetTextureOffsetU(float value) {
    m_createInfo.textureOffsetU = value;
    UpdateVertexDataFromCreateInfo();
}

void HousePlane::SetTextureOffsetV(float value) {
    m_createInfo.textureOffsetV = value;
    UpdateVertexDataFromCreateInfo();
}

void HousePlane::CreatePhysicsObject() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);

    PhysicsFilterData filterData;
    filterData.raycastGroup = RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY | CHARACTER_CONTROLLER | ITEM_PICK_UP);

    m_physicsId = Physics::CreateRigidStaticTriangleMeshFromVertexData(Transform(), m_vertices, m_indices, filterData);
   
    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_physicsId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::HOUSE_PLANE;
    Physics::SetRigidStaticUserData(m_physicsId, userData);
}

void HousePlane::SubmitRenderItem() {
    Mesh* mesh = World::GetHouseMeshByIndex(m_meshIndex);
    if (!mesh) return;

    HouseRenderItem renderItem;
    renderItem.baseColorTextureIndex = m_material->m_basecolor;
    renderItem.normalMapTextureIndex = m_material->m_normal;
    renderItem.rmaTextureIndex = m_material->m_rma;
    renderItem.baseVertex = mesh->baseVertex;
    renderItem.baseIndex = mesh->baseIndex;
    renderItem.vertexCount = mesh->vertexCount;
    renderItem.indexCount = mesh->indexCount;
    renderItem.aabbMin = glm::vec4(mesh->aabbMin, 0.0f);
    renderItem.aabbMax = glm::vec4(mesh->aabbMax, 0.0f);
    RenderDataManager::SubmitRenderItem(renderItem);
}

void HousePlane::DrawVertices(glm::vec4 color) {
    Renderer::DrawPoint(m_p0, color);
    Renderer::DrawPoint(m_p1, color);
    Renderer::DrawPoint(m_p2, color);
    Renderer::DrawPoint(m_p3, color);
}

void HousePlane::DrawEdges(glm::vec4 color) {
    Renderer::DrawLine(m_p0, m_p1, color);
    Renderer::DrawLine(m_p1, m_p2, color);
    Renderer::DrawLine(m_p2, m_p3, color);
    Renderer::DrawLine(m_p3, m_p0, color);
}