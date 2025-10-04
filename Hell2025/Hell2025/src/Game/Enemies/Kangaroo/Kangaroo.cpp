#include "Kangaroo.h"
#include "Input/Input.h"
#include "Physics/Physics.h"
#include "Pathfinding/AStarMap.h"
#include "World/World.h"
#include "Core/Game.h"
#include "Renderer/Renderer.h"
#include "HellLogging.h"
#include "Timer.hpp"

void Kangaroo::Init(KangarooCreateInfo createInfo) {
    m_createInfo = createInfo;

    Respawn();
    
    if (m_animatedGameObjectId == 0) {
        m_animatedGameObjectId = World::CreateAnimatedGameObject();
        AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
        animatedGameObject->SetSkinnedModel("Kangaroo");
        animatedGameObject->SetRotationY(HELL_PI);
        animatedGameObject->SetAnimationModeToBindPose();
        animatedGameObject->SetName("Roo");
        animatedGameObject->SetRagdoll("Kangaroo", 1500.0f);
        animatedGameObject->SetAllMeshMaterials("Kangaroo");
        animatedGameObject->SetMeshMaterialByMeshName("LeftEye_Iris", "KangarooIris");
        animatedGameObject->SetMeshMaterialByMeshName("RightEye_Iris", "KangarooIris");
        animatedGameObject->DisableDrawingForMeshByMeshName("LeftEye_Sclera");
        animatedGameObject->DisableDrawingForMeshByMeshName("RightEye_Sclera");

        Ragdoll* ragdoll = Physics::GetRagdollById(animatedGameObject->GetRagdollId());
        if (ragdoll) {
            ragdoll->SetPhysicsData(animatedGameObject->GetRagdollId(), ObjectType::RAGDOLL_ENEMY);
        }

        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Kangaroo_Idle", 1.0f);

        int32_t woundMaskIndex = Renderer::GetNextFreeWoundMaskIndexAndMarkItTaken();
        animatedGameObject->SetMeshWoundMaskTextureIndex("Body", woundMaskIndex);
        Logging::Debug() << "Assigned a Kangaroo a 'Body' mesh wound mask index of " << woundMaskIndex;

        CreateCharacterController(m_createInfo.position);
    }
}

void Kangaroo::Respawn() {
    m_position = m_createInfo.position;
    m_rotation = m_createInfo.rotation;
    m_forward = glm::vec3(-1.0f, 0.0f, 0.0f);
    m_alive = true;
    m_health = m_maxHealth;
    m_yVelocity = 0;

    m_agroState = KanagarooAgroState::CHILLING;
    m_movementState = KanagarooMovementState::IDLE;
    m_animationState = KanagarooAnimationState::IDLE;
    m_woundTextureNeedsClearing = true;

    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->SetPosition(m_position);
        animatedGameObject->SetRotationX(m_rotation.x);
        animatedGameObject->SetRotationY(m_rotation.y);
        animatedGameObject->SetRotationZ(m_rotation.z);

        animatedGameObject->SetAnimationModeToAnimated();
        animatedGameObject->PlayAndLoopAnimation("MainLayer", "Kangaroo_Idle", 1.0f);
    }

    CharacterController* characterController = GetCharacterController();
    if (characterController) {
        characterController->SetPosition(m_createInfo.position);
    }
}

AnimatedGameObject* Kangaroo::GetAnimatedGameObject(){
    return World::GetAnimatedGameObjectByObjectId(m_animatedGameObjectId);
}

void Kangaroo::Kill() {
    if (m_alive) {
        Audio::PlayAudio("Kangaroo_Death.wav", 1.0f);

        AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
        animatedGameObject->SetAnimationModeToRagdoll();
        m_health = 0;
        m_alive = false;
        m_agroState = KanagarooAgroState::KANGAROO_DEAD;
        m_animationState = KanagarooAnimationState::RAGDOLL;
        m_movementState = KanagarooMovementState::KANGAROO_DEAD;
        std::cout << "Killed kangaroo\n";
    }
}

void Kangaroo::GiveDamage(int damage) {
    m_health -= damage;
    Player* player = Game::GetLocalPlayerByIndex(0);
    glm::vec3 playerPosition = player->GetCameraPosition();
    GoToTarget(playerPosition);
    PlayFleshAudio();
    return;
    m_agroState = KanagarooAgroState::ANGRY;
}

void Kangaroo::CleanUp() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->CleanUp();
    }
}

void Kangaroo::SetAgroState(KanagarooAgroState state) {
    m_agroState = state;
}

void Kangaroo::SetMovementState(KanagarooMovementState state) {
    m_movementState = state;
}

void Kangaroo::PlayAnimation(const std::string& animationName, float speed) {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->PlayAnimation("MainLayer", animationName, speed);
    }
}

void Kangaroo::PlayAndLoopAnimation(const std::string& animationName, float speed) {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->PlayAndLoopAnimation("MainLayer", animationName, speed);
    }
}

bool Kangaroo::AnimationIsComplete() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (!animatedGameObject) return false;
    return animatedGameObject->IsAllAnimationsComplete();
}

CharacterController* Kangaroo::GetCharacterController() {
    return Physics::GetCharacterControllerById(m_characterControllerId);
}

glm::vec2 Kangaroo::GetGridPosition() {
    return AStarMap::GetCellCoordsFromWorldSpacePosition(m_position);
}

std::vector<glm::ivec2> Kangaroo::GetPath() {
    return m_aStar.GetPath();
}

void Kangaroo::MarkWoundTextureAsCleared() {
    m_woundTextureNeedsClearing = false;
}