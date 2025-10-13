#include "Window.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"
#include "Renderer/RenderDataManager.h"
#include "UniqueID.h"
#include "Util.h"
#include <unordered_map>

void Window::Init(WindowCreateInfo createInfo) {
    m_createInfo = createInfo;

    Transform transform;
    transform.position = m_createInfo.position;
    transform.rotation = m_createInfo.rotation;  
    m_modelMatrix = transform.to_mat4();   
        
    m_interiorMaterial = AssetManager::GetMaterialByName("Window");
    m_exteriorMaterial = AssetManager::GetMaterialByName("WindowExterior");

    m_model = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("Window"));
    m_glassModel = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("WindowGlass"));

    // Glass PhysX shapes
    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::NO_COLLISION;
    filterData.collidesWith = CollisionGroup::NO_COLLISION;

    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);

    m_physicsId = Physics::CreateRigidStaticTriangleMeshFromModel(transform, "WindowGlassPhysX", filterData);
    m_objectId = UniqueID::GetNext(ObjectType::WINDOW);

    // Set PhysX user data
    PhysicsUserData userData;
    userData.physicsId = m_physicsId;
    userData.objectId = m_objectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::WINDOW;
    Physics::SetRigidStaticUserData(m_physicsId, userData);
}

void Window::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
}

void Window::SetPosition(glm::vec3 position) {
    m_createInfo.position = position;
    
    // TODO: abstract away duplciated code here and in init()
    Transform transform;
    transform.position = m_createInfo.position;
    transform.rotation = m_createInfo.rotation;
    m_modelMatrix = transform.to_mat4();
    Physics::SetRigidStaticGlobalPose(m_physicsId, m_modelMatrix);
}

void Window::Update(float deltaTime) {
    UpdateRenderItems();
}

void Window::UpdateRenderItems() {
    m_renderItems.clear();
    m_glassRenderItems.clear();
    
    // Bail if models are invalid
    if (!m_model) return;
    if (!m_glassModel) return;

    // Handle missing materials
    if (!m_interiorMaterial) {
        m_interiorMaterial = AssetManager::GetDefaultMaterial();
    }
    if (!m_exteriorMaterial) {
        m_exteriorMaterial = AssetManager::GetDefaultMaterial();
    }

    static std::unordered_map<std::string, Material*> meshMaterialMap;
    if (meshMaterialMap.empty()) {       
        meshMaterialMap["SM_Window_01a"] = m_interiorMaterial;
        meshMaterialMap["SM_Window_FaceA"] = m_interiorMaterial;
        meshMaterialMap["SM_Window_FaceA_Bottom"] = m_interiorMaterial;
        meshMaterialMap["SM_Window_FaceA_Top"] = m_interiorMaterial;
        meshMaterialMap["SM_Window_FaceB"] = m_exteriorMaterial;
        meshMaterialMap["SM_Window_FaceB_Bottom"] = m_exteriorMaterial;
        meshMaterialMap["SM_Window_FaceB_Top"] = m_exteriorMaterial;
    }

    // Window render items
    for (const uint32_t& meshIndex : m_model->GetMeshIndices()) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.modelMatrix = m_modelMatrix;
        renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.objectType = Util::EnumToInt(ObjectType::WINDOW);

        // Dirty. Fix me. 
        // Very error prone. Returns invalid pointer if name not found;
        Material* material = meshMaterialMap[mesh->GetName()];

        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }
        
    // Glass render items
    for (const uint32_t& meshIndex : m_glassModel->GetMeshIndices()) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        RenderItem& renderItem = m_glassRenderItems.emplace_back();
        renderItem.modelMatrix = m_modelMatrix;
        renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.baseColorTextureIndex = m_exteriorMaterial->m_basecolor;
        renderItem.rmaTextureIndex = m_exteriorMaterial->m_rma;
        renderItem.normalMapTextureIndex = m_exteriorMaterial->m_normal;
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }
}

void Window::SubmitRenderItems() {
    RenderDataManager::SubmitRenderItems(m_renderItems);
    if (Editor::GetSelectedObjectId() == m_objectId) {
        RenderDataManager::SubmitOutlineRenderItems(m_renderItems);
    }
}