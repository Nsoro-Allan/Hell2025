#include "GameObject.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Util.h"
#include "UniqueID.h"

#include "Bvh/Cpu/CpuBvh.h"

GameObject::GameObject(GameObjectCreateInfo createInfo) {
    m_createInfo = createInfo;
    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;
    m_transform.scale = createInfo.scale;

    SetModel(createInfo.modelName);

    if (createInfo.modelName == "Bench2") {
        PhysicsFilterData filterData;
        filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
        filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
        filterData.collidesWith = (CollisionGroup)(CHARACTER_CONTROLLER | BULLET_CASING | ITEM_PICK_UP);
        m_physicsId = Physics::CreateRigidStaticConvexMeshFromModel(m_transform, "Bench_ConvexHulls", filterData);
    }

    m_objectId = UniqueID::GetNext();
}

GameObjectCreateInfo GameObject::GetCreateInfo() {
    return m_createInfo;
}

void GameObject::Update(float deltaTime) {
    
    if (m_meshNodes.GetModelName() == "Fence") {

        m_transform.rotation.x = 0.01f;

        for (const Bone& bone : m_meshNodes.GetArmature().bones) {

            const glm::mat4 rest = GetModelMatrix() * bone.localRestPose;
            glm::vec3 p = rest[3];

            Renderer::DrawPoint(p, RED);
        }

    }

}

void GameObject::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_physicsId);
    m_meshNodes.CleanUp();
}

void GameObject::SetPosition(glm::vec3 position) {
    m_transform.position = position;
}

void GameObject::SetRotation(glm::vec3 rotation) {
    m_transform.rotation = rotation;
}

void GameObject::SetRotationY(float rotation) {
    m_transform.rotation.y = rotation;
}

void GameObject::SetScale(glm::vec3 scale) {
    m_transform.scale = scale;
}



void GameObject::SetModel(const std::string& name) {
    Model* model = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName(name.c_str()));
    if (model) {
        m_meshNodes.InitFromModel(model);
    }
    else {
        m_meshNodes.CleanUp();
        std::cout << "GameObject::SetModel() failed to set model '" << name << "', it does not exist.\n";
    }
}

void GameObject::SetMeshMaterial(const std::string& meshName, const std::string& materialName) {
    m_meshNodes.SetMaterialByMeshName(meshName, materialName);
}

void GameObject::SetMeshBlendingMode(const std::string& meshName, BlendingMode blendingMode) {
    m_meshNodes.SetBlendingModeByMeshName(meshName, blendingMode);
}

void GameObject::SetName(const std::string& name) {
    m_name = name;
}

void GameObject::PrintMeshNames() {
    m_meshNodes.PrintMeshNames();
}

void GameObject::BeginFrame() {
    m_selected = false;
}

void GameObject::MarkAsSelected() {
    m_selected = true;
}

bool GameObject::IsSelected() {
    return m_selected;
}

#include "Input/Input.h"

void GameObject::UpdateRenderItems() {
    static float rot = 0;
    rot += 1 / 60.0f;

    Transform transform;
    transform.rotation.x = rot;
    m_meshNodes.SetTransformByMeshName("Yamaha_Keyboard.Cover", transform);

    m_meshNodes.UpdateRenderItems(GetModelMatrix());
}

void GameObject::SetConvexHullsFromModel(const std::string modelName) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (!model) return;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = CollisionGroup::CHARACTER_CONTROLLER;

    PxFilterData pxFilterData;
    pxFilterData.word0 = (PxU32)filterData.raycastGroup;
    pxFilterData.word1 = (PxU32)filterData.collisionGroup;
    pxFilterData.word2 = (PxU32)filterData.collidesWith;

    //PxRigidDynamic* pxRigidDynamic = m_rigidDynamic.GetPxRigidDynamic();

    // Create rigid dynamic
    PxPhysics* pxPhysics = Physics::GetPxPhysics();
    PxScene* pxScene = Physics::GetPxScene();

    

    PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(m_transform.rotation));
    PxTransform pxTransform = PxTransform(PxVec3(m_transform.position.x, m_transform.position.y, m_transform.position.z), quat);
    PxRigidDynamic* pxRigidDynamic = pxPhysics->createRigidDynamic(pxTransform);
    //pxRigidDynamic->attachShape(*pxShape);
    //PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, density);
    pxScene->addActor(*pxRigidDynamic);


    for (uint32_t meshIndex : model->GetMeshIndices()) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);

        std::span<Vertex> vertices = AssetManager::GetVerticesSpan(mesh->baseVertex, mesh->vertexCount);
        PxShape* pxShape = Physics::CreateConvexShapeFromVertexList(vertices);

        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions
        //collisionShapes.push_back(shape);

        pxRigidDynamic->attachShape(*pxShape);
    }
}

glm::vec3 GameObject::GetPosition() const {
    return m_transform.position;
}

glm::vec3 GameObject::GetEulerRotation() const {
    return m_transform.rotation;
}

glm::vec3 GameObject::GetScale() const {
    return m_transform.scale;
}

const glm::mat4 GameObject::GetModelMatrix() {
    return m_transform.to_mat4();
}

/*
const glm::vec3 GameObject::GetObjectCenter() {
    glm::vec3 aabbCenter = (m_model->GetAABBMin() + m_model->GetAABBMax()) * 0.5f;
    return GetModelMatrix() * glm::vec4(aabbCenter, 1.0f);
}

const glm::vec3 GameObject::GetObjectCenterOffsetFromOrigin() {
    return GetObjectCenter() - glm::vec3(GetModelMatrix()[3]);
}
*/