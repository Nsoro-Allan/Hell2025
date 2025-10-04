#include "Dobermann.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Ragdoll/RagdollManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "UniqueID.h"

void Dobermann::Init(DobermannCreateInfo createInfo) {
    m_objectId = UniqueID::GetNext();
    m_ragdollV2Id = RagdollManager::SpawnRagdoll(createInfo.position, createInfo.eulerDirection, "dobermann");

    g_animatedGameObjectObjectId = World::CreateAnimatedGameObject();

    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    animatedGameObject->SetSkinnedModel("Dobermann");
    animatedGameObject->SetName("Dobermann " + std::to_string(m_objectId));
    animatedGameObject->SetMeshMaterialByMeshName("Body", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Jaw", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Tongue", "DobermannMouthBlood");
    animatedGameObject->SetMeshMaterialByMeshName("Iris", "DobermannIris");
    animatedGameObject->SetPosition(createInfo.position);
    animatedGameObject->SetRagdollV2Id(m_ragdollV2Id);
    animatedGameObject->SetAnimationModeToBindPose();
    //animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_idle_sit", 1.0f);
    DisableRagdollRender();

    int32_t woundMaskIndex = Renderer::GetNextFreeWoundMaskIndexAndMarkItTaken();
    animatedGameObject->SetMeshWoundMaskTextureIndex("Body", woundMaskIndex);
    animatedGameObject->SetMeshWoundMaterialByMeshName("Body", "DobermannFullBlood");
    Logging::Debug() << "Assigned a Dobermann a 'Body' mesh wound mask index of " << woundMaskIndex;
}

void Dobermann::EnableRagdollRender() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    RagdollV2* ragdoll = RagdollManager::GetRagdollV2ById(m_ragdollV2Id);

    if (!ragdoll) return;
    if (!animatedGameObject) return;

    animatedGameObject->DisableRendering();
    ragdoll->EnableRendering();

    m_renderRagdoll = true;
}

void Dobermann::TakeDamage(uint32_t damage) {
    std::cout << "SHOT DOBERMANN\n";

    RagdollV2* ragdoll = RagdollManager::GetRagdollV2ById(m_ragdollV2Id);
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    animatedGameObject->SetAnimationModeToRagdollV2();
}

void Dobermann::DisableRagdollRender() {
    RagdollV2* ragdoll = RagdollManager::GetRagdollV2ById(m_ragdollV2Id);
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();

    if (!ragdoll) return;
    if (!animatedGameObject) return;

    animatedGameObject->EnableRendering();
    ragdoll->DisableRendering();

    m_renderRagdoll = false;
}


void Dobermann::Update(float deltaTime) {
    Renderer::DrawPoint(GetPosition(), PINK);

    if (Input::KeyPressed(HELL_KEY_I)) {
        if (m_renderRagdoll) {        
            DisableRagdollRender();
        }
        else {
            EnableRagdollRender();
        }
    }
}

AnimatedGameObject* Dobermann::GetAnimatedGameObject() {
    return World::GetAnimatedGameObjectByObjectId(g_animatedGameObjectObjectId);
}

glm::vec3 Dobermann::GetPosition() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (!animatedGameObject) return glm::vec3(0.0f);

    return animatedGameObject->GetModelMatrix()[3];

}