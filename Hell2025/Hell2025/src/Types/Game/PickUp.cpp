#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

#include "Input/Input.h"
#include "World/World.h"
#include "Renderer/Renderer.h"

void PickUp::Init(PickUpCreateInfo createInfo) {
    m_initialTransform.position = createInfo.position;
    m_initialTransform.rotation = createInfo.rotation;
    m_pickUpType = Util::StringToPickUpType(createInfo.pickUpType);
    m_isGold = createInfo.isGold;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ITEM_PICK_UP;
    filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

    float mass = 0.0f;
    Model* physicsModel = nullptr;

    // Shotty buckshot
    if (m_pickUpType == PickUpType::SHOTGUN_AMMO_BUCKSHOT) {
        m_meshNodes.InitFromModel("Shotgun_AmmoBox");
        m_meshNodes.SetMeshMaterials("Shotgun_AmmoBox");
        glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    }
    // Shotty slug
    else if (m_pickUpType == PickUpType::SHOTGUN_AMMO_SLUG) {
        m_meshNodes.InitFromModel("Shotgun_AmmoBox");
        m_meshNodes.SetMeshMaterials("Shotgun_AmmoBoxSlug");
        glm::vec3 modelExtents = AssetManager::GetModelByName("Shotgun_AmmoBox")->GetExtents();
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, modelExtents, mass, filterData);
    }
    // AKS74U
    else if (m_pickUpType == PickUpType::AKS74U) {
        m_meshNodes.InitFromModel("AKS74U_PickUp");
        m_meshNodes.SetMaterialByMeshName("Mesh0", "AKS74U_0");
        m_meshNodes.SetMaterialByMeshName("Mesh1", "AKS74U_1");
        m_meshNodes.SetMaterialByMeshName("Mesh2", "AKS74U_2");
        m_meshNodes.SetMaterialByMeshName("Mesh3", "AKS74U_3");
        m_meshNodes.SetMaterialByMeshName("Mesh4", "AKS74U_4");
        mass = 2.7f;
        physicsModel = AssetManager::GetModelByName("AKS74U_PickUpConvexMesh");
    }

    // Remington 870
    else if (m_pickUpType == PickUpType::REMINGTON_870) {
        m_meshNodes.InitFromModel("World_Remington870");
        m_meshNodes.SetMeshMaterials("Shotgun");
        mass = 3.2f;
        physicsModel = AssetManager::GetModelByName("World_Remington870_Collision_Mesh");
    }

    // SPAS
    else if (m_pickUpType == PickUpType::SPAS) {
        m_meshNodes.InitFromModel("World_SPAS");
        m_meshNodes.SetMaterialByMeshName("SPAS12_Main", "SPAS_Main");
        m_meshNodes.SetMaterialByMeshName("SPAS12_Moving", "SPAS_Moving");
        m_meshNodes.SetMaterialByMeshName("SPAS12_Stamped", "SPAS_Stamped");
        mass = 3.3f;
        physicsModel = AssetManager::GetModelByName("World_SPAS_Collision_Mesh");
    }

    else if (m_pickUpType == PickUpType::GLOCK || m_pickUpType == PickUpType::GOLDEN_GLOCK) {
        m_meshNodes.InitFromModel("World_Glock");
        m_meshNodes.SetMeshMaterials("Glock");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    }


    else if (m_pickUpType == PickUpType::TOKAREV) {
        m_meshNodes.InitFromModel("World_Tokarev");
        m_meshNodes.SetMaterialByMeshName("TokarevBody", "Tokarev");
        m_meshNodes.SetMaterialByMeshName("TokarevGripPolymer", "TokarevGrip");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("World_Glock_Collision_Mesh");
    }


    else {
        std::cout << "Warning!: You created a pickup with an invalid type!: " << createInfo.pickUpType << "\n";
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

    // Get next unique ID
    m_objectId = UniqueID::GetNextGlobal();

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

PickUpCreateInfo PickUp::GetCreateInfo() {
    PickUpCreateInfo createInfo;
    createInfo.position = m_initialTransform.position;
    createInfo.rotation = m_initialTransform.rotation;
    createInfo.pickUpType = Util::PickUpTypeToString(m_pickUpType);
    return createInfo;
}

void PickUp::Update(float deltaTime) {
    m_modelMatrix = m_initialTransform.to_mat4();

    // If physx object exists, rip the model matrix out
    if (Physics::RigidDynamicExists(m_physicsId)) {
        m_modelMatrix = Physics::GetRigidDynamicWorldMatrix(m_physicsId);
    }

    m_meshNodes.UpdateRenderItems(GetModelMatrix());
}

void PickUp::CleanUp() {
    Physics::MarkRigidDynamicForRemoval(m_physicsId);
}

void PickUp::SetPosition(glm::vec3 position) {
    m_initialTransform.position = position;
}