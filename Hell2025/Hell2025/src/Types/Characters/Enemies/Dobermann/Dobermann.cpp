#include "Dobermann.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Ragdoll/RagdollManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "UniqueID.h"

void Dobermann::Init(DobermannCreateInfo createInfo) {
    m_objectId = UniqueID::GetNextObjectId(ObjectType::DOBERMANN);
    //m_ragdollV2Id = RagdollManager::SpawnRagdoll(createInfo.position, createInfo.eulerDirection, "manikin2");
    m_ragdollV2Id = RagdollManager::SpawnRagdoll(createInfo.position, createInfo.eulerDirection, "dobermann_new"); 

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
    animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_Lay", 1.0f);
    DisableRagdollRender();

    int32_t woundMaskIndex = Renderer::GetNextFreeWoundMaskIndexAndMarkItTaken();
    animatedGameObject->SetMeshWoundMaskTextureIndex("Body", woundMaskIndex);
    animatedGameObject->SetMeshWoundMaterialByMeshName("Body", "DobermannFullBlood");
    Logging::Debug() << "Assigned a Dobermann a 'Body' mesh wound mask index of " << woundMaskIndex;
}

void Dobermann::TakeDamage(uint32_t damage) {
    RagdollV2* ragdoll = RagdollManager::GetRagdollV2ById(m_ragdollV2Id);
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    animatedGameObject->SetAnimationModeToRagdollV2();
}

void Dobermann::EnableRagdollRender() {
    m_renderRagdoll = true;
}

void Dobermann::DisableRagdollRender() {
    m_renderRagdoll = false;
}

void Dobermann::Update(float deltaTime) {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    RagdollV2* ragdoll = RagdollManager::GetRagdollV2ById(m_ragdollV2Id);

    if (!ragdoll) return;
    if (!animatedGameObject) return;

    if (Input::KeyPressed(HELL_KEY_I)) {
        if (m_renderRagdoll) {
            DisableRagdollRender();
        }
        else {
            EnableRagdollRender();
        }
    }

    if (Input::KeyPressed(HELL_KEY_Y)) {
        ragdoll->SetToInitialPose();
        animatedGameObject->SetAnimationModeToAnimated();
        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_Lay", 1.0f);
    }

    static bool gettingUp = false;

    if (Input::KeyPressed(HELL_KEY_LEFT)) {
        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_Lay", 1.0f);
        gettingUp = false;
    }
    if (Input::KeyPressed(HELL_KEY_RIGHT)) {
        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_Walk", 1.0f);
        gettingUp = false;
    }
    if (Input::KeyPressed(HELL_KEY_UP)) {
        animatedGameObject->PlayAnimation("MainLayer", "Dobermann_Lay_to_Walk", 1.0f);
        gettingUp = true;
    }
    if (Input::KeyPressed(HELL_KEY_DOWN)) {
        gettingUp = false;
        animatedGameObject->PlayAnimation("MainLayer", "Dobermann_Stretch_to_Lay", 1.0f);
    }

    if (animatedGameObject->IsAllAnimationsComplete() && gettingUp) {
        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Dobermann_Walk", 1.0f);
    }

    // Ragdoll rendering
    if (m_renderRagdoll) {
        animatedGameObject->DisableRendering();
        ragdoll->EnableRendering();
    }
    else {
        animatedGameObject->EnableRendering();
        ragdoll->DisableRendering();

    }
    //Renderer::DrawPoint(GetPosition(), PINK);

}

AnimatedGameObject* Dobermann::GetAnimatedGameObject() {
    return World::GetAnimatedGameObjectByObjectId(g_animatedGameObjectObjectId);
}

glm::vec3 Dobermann::GetPosition() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (!animatedGameObject) return glm::vec3(0.0f);

    return animatedGameObject->GetModelMatrix()[3];

}