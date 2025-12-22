#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

#include "Input/Input.h"
#include "World/World.h"
#include "Renderer/Renderer.h"

PickUp::PickUp(uint64_t id, const PickUpCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_objectId = id;

    PickUpInfo* pickUpInfo = Bible::GetPickUpInfoByName(createInfo.name);
    if (!pickUpInfo) return; // Should never happen

    m_initialTransform.position = createInfo.position + spawnOffset.translation;
    m_initialTransform.rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    Bible::ConfigureMeshNodesByPickUpName(id, pickUpInfo->name, m_meshNodes, true);


    //m_meshNodes.Update(m_initialTransform.to_mat4());

    //std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;
    //
    //// Shotty buckshot
    //if (GetType() == PickUpTypeOld::SHOTGUN_AMMO_BUCKSHOT) {
    //    m_meshNodes.Init(id, "Shotgun_AmmoBox", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMeshMaterials("Shotgun_AmmoBox");
    //    glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
    //    mass = 0.45f;
    //    m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    //}
    //// Shotty slug
    //else if (GetType() == PickUpTypeOld::SHOTGUN_AMMO_SLUG) {
    //    m_meshNodes.Init(id, "Shotgun_AmmoBox", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMeshMaterials("Shotgun_AmmoBoxSlug");
    //    glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
    //    mass = 0.45f;
    //    m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    //}
    //// AKS74U
    //else if (GetType() == PickUpTypeOld::AKS74U) {
    //    m_meshNodes.Init(id, "AKS74U_PickUp", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMaterialByMeshName("Mesh0", "AKS74U_0");
    //    m_meshNodes.SetMaterialByMeshName("Mesh1", "AKS74U_1");
    //    m_meshNodes.SetMaterialByMeshName("Mesh2", "AKS74U_2");
    //    m_meshNodes.SetMaterialByMeshName("Mesh3", "AKS74U_3");
    //    m_meshNodes.SetMaterialByMeshName("Mesh4", "AKS74U_4");
    //    mass = 2.7f;
    //    physicsModel = AssetManager::GetModelByName("AKS74U_PickUpConvexMesh");
    //}
    //
    //// Remington 870
    //else if (GetType() == PickUpTypeOld::REMINGTON_870) {
    //    m_meshNodes.Init(id, "World_Remington870", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMeshMaterials("Shotgun");
    //    mass = 3.2f;
    //    physicsModel = AssetManager::GetModelByName("World_Remington870_Collision_Mesh");
    //}
    //
    //// SPAS
    //else if (GetType() == PickUpTypeOld::SPAS) {
    //    m_meshNodes.Init(id, "World_SPAS", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMaterialByMeshName("SPAS12_Main", "SPAS_Main");
    //    m_meshNodes.SetMaterialByMeshName("SPAS12_Moving", "SPAS_Moving");
    //    m_meshNodes.SetMaterialByMeshName("SPAS12_Stamped", "SPAS_Stamped");
    //    mass = 3.3f;
    //    physicsModel = AssetManager::GetModelByName("World_SPAS_Collision_Mesh");
    //}
    //
    //else if (GetType() == PickUpTypeOld::GLOCK || GetType() == PickUpTypeOld::GOLDEN_GLOCK) {
    //    m_meshNodes.Init(id, "World_Glock", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMeshMaterials("Glock");
    //    mass = 0.7f;
    //    physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    //}
    //
    //else if (GetType() == PickUpTypeOld::TOKAREV) {
    //    m_meshNodes.Init(id, "World_Tokarev", emptyMeshNodeCreateInfoSet);
    //    m_meshNodes.SetMaterialByMeshName("TokarevBody", "Tokarev");
    //    m_meshNodes.SetMaterialByMeshName("TokarevGripPolymer", "TokarevGrip");
    //    mass = 0.7f;
    //    physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    //}
    //
    //else {
    //    std::cout << "Warning!: You created a pickup with an invalid type!: " << Util::PickUpTypeToString(GetType()) << "\n";
    //    return;
    //}
    //
    //// Physics object
    //if (physicsModel) {
    //    int32_t meshIndex = physicsModel->GetMeshIndices()[0];
    //    Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
    //    if (mesh) {
    //        std::span<Vertex> vertices = AssetManager::GetVerticesSpan(mesh->baseVertex, mesh->vertexCount);
    //        std::span<uint32_t> indices = AssetManager::GetIndicesSpan(mesh->baseIndex, mesh->indexCount);
    //        m_physicsId = Physics::CreateRigidDynamicFromConvexMeshVertices(m_initialTransform, vertices, indices, mass, filterData);
    //    }
    //}
    //
    //// Set PhysX user data
    //PhysicsUserData userData;
    //userData.physicsId = m_physicsId;
    //userData.objectId = m_objectId;
    //userData.physicsType = PhysicsType::RIGID_DYNAMIC;
    //userData.objectType = ObjectType::PICK_UP;
    //Physics::SetRigidDynamicUserData(m_physicsId, userData);
    //
    //
    //// Apply initial impulse
    //glm::vec3 force = createInfo.intitialForce;
    //Physics::AddFoceToRigidDynamic(m_physicsId, force);
}

void PickUp::Update(float deltaTime) {
    m_modelMatrix = m_initialTransform.to_mat4();
    m_meshNodes.Update(GetModelMatrix());

    if (m_firstFrame && m_createInfo.disablePhysicsAtSpawn) {
        m_meshNodes.SleepAllPhysics();
    }

    m_firstFrame = false;

    if (Input::RightMousePressed()) {
        m_meshNodes.WakeAllPhysics();
    }
}

void PickUp::CleanUp() {
    m_meshNodes.CleanUp();
}

void PickUp::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_initialTransform.position = position;
    m_meshNodes.ResetFirstFrame();
}

void PickUp::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_initialTransform.rotation = rotation;
    m_meshNodes.ResetFirstFrame();
}

void PickUp::SetDisabledPhysicsAtSpawnState(bool state) {
    m_createInfo.disablePhysicsAtSpawn = state;
}

void PickUp::SetRespawnState(bool state) {
    m_createInfo.respawn = state;
}