#include "Dobermann.h"
#include "Ragdoll/RagdollManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "UniqueID.h"

void Dobermann::Init(DobermannCreateInfo createInfo) {
    m_objectId = UniqueID::GetNext();

    //m_temporaryPosition = createInfo.position;
    
    g_animatedGameObjectObjectId = World::CreateAnimatedGameObject();
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    animatedGameObject->SetSkinnedModel("Dobermann");
    animatedGameObject->SetName("Dobermann " + std::to_string(m_objectId));
    animatedGameObject->SetMeshMaterialByMeshName("Body", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Jaw", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Tongue", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Iris", "DobermannIris");

    animatedGameObject->SetPosition(createInfo.position);
    animatedGameObject->PrintMeshNames();


    RagdollManager::SpawnRagdoll(createInfo.position, createInfo.eulerDirection, "dobermann");

    //animatedGameObject->SetMeshMaterialByMeshName("Iris", "DobermannMouthBlood");

    //animatedGameObject->PlayAndLoopAnimation("MainLayer", "Shark_Swim", 1.0f);



    //dobermann->SetSkinnedModel("Dobermann");
    ////dobermann->PrintMeshNames();
    ////dobermann->PrintNodeNames();
    //dobermann->SetAnimationModeToBindPose();
    //dobermann->SetPosition(glm::vec3(36.8f, 31.0f, 37.23f));
    //dobermann->PlayAndLoopAnimation("Main", "Dobermann_idle_loop", 1.0f);

}

void Dobermann::Update(float deltaTime) {
    Renderer::DrawPoint(GetPosition(), PINK);
}

AnimatedGameObject* Dobermann::GetAnimatedGameObject() {
    return World::GetAnimatedGameObjectByObjectId(g_animatedGameObjectObjectId);
}

glm::vec3 Dobermann::GetPosition() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (!animatedGameObject) return glm::vec3(0.0f);

    return animatedGameObject->GetModelMatrix()[3];

}