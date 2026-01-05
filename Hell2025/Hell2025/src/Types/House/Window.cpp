#include "Window.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "UniqueID.h"
#include "Util.h"

Window::Window(uint64_t id, const WindowCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_objectId = id;

    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

    MeshNodeCreateInfo& trimInterior = meshNodeCreateInfoSet.emplace_back();
    trimInterior.meshName = "TrimInterior";
    trimInterior.materialName = "Window";

    MeshNodeCreateInfo& trimExterior = meshNodeCreateInfoSet.emplace_back();
    trimExterior.meshName = "TrimExterior";
    trimExterior.materialName = "WindowExterior";

    MeshNodeCreateInfo& frameExteriorBottom = meshNodeCreateInfoSet.emplace_back();
    frameExteriorBottom.meshName = "FrameExteriorBottom";
    frameExteriorBottom.materialName = "WindowExterior";

    MeshNodeCreateInfo& frameExteriorTop = meshNodeCreateInfoSet.emplace_back();
    frameExteriorTop.meshName = "FrameExteriorTop";
    frameExteriorTop.materialName = "WindowExterior";

    MeshNodeCreateInfo& frameInteriorBottom = meshNodeCreateInfoSet.emplace_back();
    frameInteriorBottom.meshName = "FrameInteriorBottom";
    frameInteriorBottom.materialName = "Window";

    MeshNodeCreateInfo& frameInteriorTop = meshNodeCreateInfoSet.emplace_back();
    frameInteriorTop.meshName = "FrameInteriorTop";
    frameInteriorTop.materialName = "Window";

    MeshNodeCreateInfo& glassInteriorTop = meshNodeCreateInfoSet.emplace_back();
    glassInteriorTop.meshName = "GlassInteriorTop";
    glassInteriorTop.materialName = "WindowExterior";
    glassInteriorTop.blendingMode = BlendingMode::GLASS;
    glassInteriorTop.decalType = DecalType::GLASS;

    MeshNodeCreateInfo& glassInteriorBottom = meshNodeCreateInfoSet.emplace_back();
    glassInteriorBottom.meshName = "GlassInteriorBottom";
    glassInteriorBottom.materialName = "WindowExterior";
    glassInteriorBottom.blendingMode = BlendingMode::GLASS;
    glassInteriorBottom.decalType = DecalType::GLASS;

    MeshNodeCreateInfo& glassExteriorTop = meshNodeCreateInfoSet.emplace_back();
    glassExteriorTop.meshName = "GlassExteriorTop";
    glassExteriorTop.materialName = "WindowExterior";
    glassExteriorTop.blendingMode = BlendingMode::GLASS;
    glassExteriorTop.decalType = DecalType::GLASS;

    MeshNodeCreateInfo& glassExteriorBottom = meshNodeCreateInfoSet.emplace_back();
    glassExteriorBottom.meshName = "GlassExteriorBottom";
    glassExteriorBottom.materialName = "WindowExterior";
    glassExteriorBottom.blendingMode = BlendingMode::GLASS;
    glassExteriorBottom.decalType = DecalType::GLASS;

    MeshNodeCreateInfo& lock = meshNodeCreateInfoSet.emplace_back();
    lock.meshName = "Lock";
    lock.materialName = "Window";

    m_meshNodes.Init(m_objectId, "Window2", meshNodeCreateInfoSet);
    m_meshNodes.CastCSMShadows();
    m_meshNodes.Update(m_transform.to_mat4());

    // Glass PhysX shapes
    //PhysicsFilterData filterData;
    //filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    //filterData.collisionGroup = CollisionGroup::NO_COLLISION;
    //filterData.collidesWith = CollisionGroup::NO_COLLISION;
    //
    //filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    //filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
    //
    //m_physicsId = Physics::CreateRigidStaticTriangleMeshFromModel(m_transform, "WindowGlassPhysX", filterData);
    //
    //// Set PhysX user data
    //PhysicsUserData userData;
    //userData.physicsId = m_physicsId;
    //userData.objectId = m_objectId;
    //userData.physicsType = PhysicsType::RIGID_STATIC;
    //userData.objectType = ObjectType::WINDOW;
    //Physics::SetRigidStaticUserData(m_physicsId, userData);
}

void Window::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
}

void Window::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_transform.position = position;
    m_meshNodes.Update(m_transform.to_mat4());
    Physics::SetRigidStaticWorldTransform(m_physicsId, m_transform.to_mat4());
}

void Window::SetRotationY(float value) {
    m_createInfo.rotation.y = value;
    m_meshNodes.Update(m_transform.to_mat4());
}

void Window::Update(float deltaTime) {
    // Nothing as of yet
}