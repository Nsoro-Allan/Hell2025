#include "Shark.h"
#include "Physics/Physics.h"

void Shark::PlayAnimation(const std::string& animationName, float speed) {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->PlayAnimation("MainLayer", animationName, 1.0f);
    }
}

void Shark::PlayAndLoopAnimation(const std::string& animationName, float speed) {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        animatedGameObject->PlayAndLoopAnimation("MainLayer", animationName, 1.0f);
    }
}

int Shark::GetAnimationFrameNumber() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        return animatedGameObject->GetAnimationFrameNumber("MainLayer");
    }
    else {
        return 0;
    }
}

glm::vec3 Shark::GetMouthPosition3D() {
    Ragdoll* ragdoll = GetRadoll();
    if (!ragdoll) return glm::vec3(0.0f);

    glm::mat4 headBoneTransform = ragdoll->GetRigidWorlTransform("BN_Head_00");
    return headBoneTransform[3];
}

glm::vec3 Shark::GetMouthPosition2D() {
    return GetMouthPosition3D() * glm::vec3(1.0f, 0.0f, 1.0f);
}

glm::vec3 Shark::GetSpinePosition(int index) {
    if (index >= 0 && index < SHARK_SPINE_SEGMENT_COUNT) {
        return m_spinePositions[index];
    }
    else {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

float Shark::GetDistanceMouthToTarget3D() {
    float fallback = 9999.0f;
    if (m_movementState == SharkMovementState::ARROW_KEYS ||
        m_movementState == SharkMovementState::STOPPED) {
        return fallback;
    }
    return glm::distance(GetMouthPosition3D(), m_targetPosition);
}

glm::vec3 Shark::GetEvadePoint3D() {
    return GetSpinePosition(0) + (GetForwardVector() * glm::vec3(-0.0f));
}

glm::vec3 Shark::GetEvadePoint2D() {
    return GetEvadePoint3D() * glm::vec3(1.0f, 0.0f, 1.0f);
}

bool Shark::IsBehindEvadePoint(glm::vec3 position) {
    glm::vec3 position2D = position * glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 evadePoint2D = GetEvadePoint2D();

    glm::vec3 directionToPosition = position2D - evadePoint2D;
    if (glm::length(directionToPosition) < 1e-6f) {
        return false;
    }
    directionToPosition = glm::normalize(directionToPosition);

    glm::vec3 forwardVector = glm::normalize(GetForwardVector());

    float dotResult = glm::dot(directionToPosition, forwardVector);

    bool isBehind = dotResult < 0.0f;

    return isBehind;
}
