#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

#include "Input/Input.h"
#include "World/World.h"

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
        SetModel("Shotgun_AmmoBox");
        SetAllMeshMaterials("Shotgun_AmmoBox");
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, m_model->GetExtents(), mass, filterData);
    }
    // Shotty slug
    else if (m_pickUpType == PickUpType::SHOTGUN_AMMO_SLUG) {
        SetModel("Shotgun_AmmoBox");
        SetAllMeshMaterials("Shotgun_AmmoBoxSlug");
        mass = 0.45f;
        m_physicsId = Physics::CreateRigidDynamicFromBoxExtents(m_initialTransform, m_model->GetExtents(), mass, filterData);
    }
    // AKS74U
    else if (m_pickUpType == PickUpType::AKS74U) {
        SetModel("AKS74U_PickUp");
        SetMeshMaterial("Mesh0", "AKS74U_0");
        SetMeshMaterial("Mesh1", "AKS74U_1");
        SetMeshMaterial("Mesh2", "AKS74U_2");
        SetMeshMaterial("Mesh3", "AKS74U_3");
        SetMeshMaterial("Mesh4", "AKS74U_4");
        mass = 2.7f;
        physicsModel = AssetManager::GetModelByName("AKS74U_PickUpConvexMesh");
    }

    // Remington 870
    else if (m_pickUpType == PickUpType::REMINGTON_870) {
        SetModel("Shotgun_PickUp");
        SetAllMeshMaterials("Shotgun");
        mass = 3.2f;
        physicsModel = AssetManager::GetModelByName("Shotgun_PickUpConvexMesh");
    }

    // SPAS
    else if (m_pickUpType == PickUpType::SPAS) {
        SetModel("SPAS_Isolated");
        SetMeshMaterial("SPAS12_Main", "SPAS_Main");
        SetMeshMaterial("SPAS12_Moving", "SPAS_Moving");
        SetMeshMaterial("SPAS12_Stamped", "SPAS_Stamped");
        mass = 3.3f;
        physicsModel = AssetManager::GetModelByName("SPAS_ConvexMesh");
    }

    else if (m_pickUpType == PickUpType::GLOCK || m_pickUpType == PickUpType::GOLDEN_GLOCK) {
        SetModel("Glock_Isolated");
        SetAllMeshMaterials("Glock");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("Glock_Isolated_ConvexMesh");
    }


    else if (m_pickUpType == PickUpType::TOKAREV) {
        SetModel("Tokarev_Isolated");
        SetMeshMaterial("TokarevBody", "Tokarev");
        SetMeshMaterial("TokarevGripPolymer", "TokarevGrip");
        mass = 0.7f;
        physicsModel = AssetManager::GetModelByName("Glock_Isolated_ConvexMesh");
    }


    else {
        std::cout << "Warning!: You created a pickup with an invalid type!: " << createInfo.pickUpType << "\n";
        return;
    }

    // Error check the m_model
    if (!m_model) {
        std::cout << "Pickup::Init() WARNING!!!: m_model was nullptr, meaning it was not found!\n";
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
    m_objectId = UniqueID::GetNext();

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

void PickUp::SetModel(const std::string& modelName) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (!model) {
        std::cout << "PickUp::SetModel() failed: '" << modelName << "' was not found\n";
    }
    m_model = model;
    m_materialIndices.resize(m_model->GetMeshCount());
}

void PickUp::SetMeshMaterial(const std::string& meshName, const std::string& materialName) {
    if (!m_model) {
        std::cout << "PickUp::SetMeshMaterial() failed: m_model was nullptr\n";
        return;
    }
    int32_t materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        std::cout << "PickUp::SetMeshMaterial() failed: material '" << materialName << "' was not found\n";
        return;
    }

    for (int i = 0; i < m_model->GetMeshCount(); i++) {
        int32_t meshIndex = m_model->GetMeshIndices()[i];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh && mesh->GetName() == meshName) {
            m_materialIndices[i] = materialIndex;
            return;
        }
    }
    // If you made it this far, mesh name was nut found
    std::cout << "PickUp::SetMeshMaterial() failed: mesh name '" << meshName << "' was not found in m_model's mesh list\n";
}

void PickUp::SetAllMeshMaterials(const std::string& materialName) {
    int32_t materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        std::cout << "PickUp::SetAllMeshMaterials() failed: material '" << materialName << "' was not found\n";
        return;
    }
    for (int i = 0; i < m_materialIndices.size(); i++) {
        m_materialIndices[i] = materialIndex;
    }
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

    UpdateRenderItems();
}

void PickUp::CleanUp() {
    Physics::MarkRigidDynamicForRemoval(m_physicsId);
}

void PickUp::SetPosition(glm::vec3 position) {
    m_initialTransform.position = position;
}

//void PickUp::MarkForRemoval() {
//    m_markedForRemoval = true;
//}

void PickUp::UpdateRenderItems() {
    m_renderItems.clear();    
    if (!m_model) {
        std::cout << "A pickup failed to create render items because it had a nullptr m_model!\n";
        return;
    }

    for (int i = 0; i < m_model->GetMeshCount(); i++) {
        Mesh* mesh = AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i]);
        if (mesh) {
            Material* material = AssetManager::GetMaterialByIndex(m_materialIndices[i]);
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.objectType = (int)ObjectType::PICK_UP;
            renderItem.modelMatrix = GetModelMatrix();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = m_model->GetMeshIndices()[i];  
            if (m_isGold) {
                static Material* goldMaterial = AssetManager::GetMaterialByName("Gold");
                renderItem.baseColorTextureIndex = goldMaterial->m_basecolor;
                renderItem.rmaTextureIndex = goldMaterial->m_rma;
            }
            else {
                renderItem.baseColorTextureIndex = material->m_basecolor;
                renderItem.rmaTextureIndex = material->m_rma;
            }
            renderItem.normalMapTextureIndex = material->m_normal;

            Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
            Util::UpdateRenderItemAABB(renderItem);
        }
    }
}
