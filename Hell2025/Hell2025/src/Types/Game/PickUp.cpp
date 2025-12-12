#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

#include "Input/Input.h"
#include "World/World.h"
#include "Renderer/Renderer.h"

PickUp::PickUp(uint64_t id, const PickUpCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_objectId = id;

    m_initialTransform.position = createInfo.position;
    m_initialTransform.rotation = createInfo.rotation;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
    filterData.collisionGroup = CollisionGroup::ITEM_PICK_UP;
    filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

    float mass = 0.0f;
    Model* physicsModel = nullptr;

    std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;

    // Shotty buckshot
    if (GetType() == PickUpType::SHOTGUN_AMMO_BUCKSHOT) {
        m_meshNodes.Init(id, "Shotgun_AmmoBox", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMeshMaterials("Shotgun_AmmoBox");
        glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    }
    // Shotty slug
    else if (GetType() == PickUpType::SHOTGUN_AMMO_SLUG) {
        m_meshNodes.Init(id, "Shotgun_AmmoBox", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMeshMaterials("Shotgun_AmmoBoxSlug");
        glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    }
    // AKS74U
    else if (GetType() == PickUpType::AKS74U) {
        m_meshNodes.Init(id, "AKS74U_PickUp", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMaterialByMeshName("Mesh0", "AKS74U_0");
        m_meshNodes.SetMaterialByMeshName("Mesh1", "AKS74U_1");
        m_meshNodes.SetMaterialByMeshName("Mesh2", "AKS74U_2");
        m_meshNodes.SetMaterialByMeshName("Mesh3", "AKS74U_3");
        m_meshNodes.SetMaterialByMeshName("Mesh4", "AKS74U_4");
        mass = 2.7f;
        physicsModel = AssetManager::GetModelByName("AKS74U_PickUpConvexMesh");
    }

    // Remington 870
    else if (GetType() == PickUpType::REMINGTON_870) {
        m_meshNodes.Init(id, "World_Remington870", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMeshMaterials("Shotgun");
        mass = 3.2f;
        physicsModel = AssetManager::GetModelByName("World_Remington870_Collision_Mesh");
    }

    // SPAS
    else if (GetType() == PickUpType::SPAS) {
        m_meshNodes.Init(id, "World_SPAS", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMaterialByMeshName("SPAS12_Main", "SPAS_Main");
        m_meshNodes.SetMaterialByMeshName("SPAS12_Moving", "SPAS_Moving");
        m_meshNodes.SetMaterialByMeshName("SPAS12_Stamped", "SPAS_Stamped");
        mass = 3.3f;
        physicsModel = AssetManager::GetModelByName("World_SPAS_Collision_Mesh");
    }

    else if (GetType() == PickUpType::GLOCK || GetType() == PickUpType::GOLDEN_GLOCK) {
        m_meshNodes.Init(id, "World_Glock", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMeshMaterials("Glock");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    }

    else if (GetType() == PickUpType::TOKAREV) {
        m_meshNodes.Init(id, "World_Tokarev", emptyMeshNodeCreateInfoSet);
        m_meshNodes.SetMaterialByMeshName("TokarevBody", "Tokarev");
        m_meshNodes.SetMaterialByMeshName("TokarevGripPolymer", "TokarevGrip");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    }

    else {
        std::cout << "Warning!: You created a pickup with an invalid type!: " << Util::PickUpTypeToString(GetType()) << "\n";
        return;
    }

    // Physics object
    if (physicsModel) {
        int32_t meshIndex = physicsModel->GetMeshIndices()[0];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh) {
            std::span<Vertex> vertices = AssetManager::GetVerticesSpan(mesh->baseVertex, mesh->vertexCount);
            std::span<uint32_t> indices = AssetManager::GetIndicesSpan(mesh->baseIndex, mesh->indexCount);
            m_physicsId = Physics::CreateRigidDynamicFromConvexMeshVertices(m_initialTransform, vertices, indices, mass, filterData);
        }
    }

    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_physicsId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_DYNAMIC;
    userData.objectType = ObjectType::PICK_UP;
    Physics::SetRigidDynamicUserData(m_physicsId, userData);


    // Apply initial impulse
    glm::vec3 force = createInfo.intitialForce;
    Physics::AddFoceToRigidDynamic(m_physicsId, force);
}

void PickUp::Update(float deltaTime) {
    m_modelMatrix = m_initialTransform.to_mat4();

    // If physx object exists, rip the model matrix out
    if (Physics::RigidDynamicExists(m_physicsId)) {
        m_modelMatrix = Physics::GetRigidDynamicWorldMatrix(m_physicsId);
    }

    m_meshNodes.Update(GetModelMatrix());
}

void PickUp::CleanUp() {
    m_meshNodes.CleanUp();
    Physics::MarkRigidDynamicForRemoval(m_physicsId);
}

void PickUp::SetPosition(const glm::vec3& position) {
    m_initialTransform.position = position;
}