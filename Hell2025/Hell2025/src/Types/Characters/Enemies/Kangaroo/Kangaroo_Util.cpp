#include "Kangaroo.h"
#include "Physics/Physics.h"
#include "Util/Util.h"

const std::string Kangaroo::GetAnimationStateAsString() {
    switch (m_animationState) {
        case KanagarooAnimationState::IDLE:             return "IDLE";
        case KanagarooAnimationState::HOP_TO_IDLE:      return "HOP_TO_IDLE";
        case KanagarooAnimationState::IDLE_TO_HOP:      return "IDLE_TO_HOP";
        case KanagarooAnimationState::HOP:              return "HOP";
        case KanagarooAnimationState::BITE:             return "BITE";
        default: return "UNKNOWN";
    }
}

void Kangaroo::CreateCharacterController(glm::vec3 position) {
    float capsuleHeight = 1.4f;
    float capsuleRadius = 0.4;

    PhysicsFilterData physicsFilterData;
    physicsFilterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    physicsFilterData.collisionGroup = CollisionGroup::CHARACTER_CONTROLLER;
    physicsFilterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | CHARACTER_CONTROLLER);

    m_characterControllerId = Physics::CreateCharacterController(666666, position, capsuleHeight, capsuleRadius, physicsFilterData);
}

bool Kangaroo::HasValidPath() {
    return m_aStar.m_finalPath.size() >= 2; 
    
    // TODO!!! Replace with smooth path
}

const std::string Kangaroo::GetDebugInfoString() {
    std::string result = "\nKANGAROO\n";

    result += " - Health: " + std::to_string(GetHealth()) + "\n";
    result += " - AnimationState: " + GetAnimationStateAsString() + "\n";
    result += " - m_position: " + Util::Vec3ToString(m_position) + "\n";
    result += " - m_rotation: " + Util::Vec3ToString(m_rotation) + "\n";
    result += " - m_forward: " + Util::Vec3ToString(m_forward) + "\n";
    result += " - m_yVelocity: " + std::to_string(m_yVelocity) + "\n";
    result += " - m_grounded: " + Util::BoolToString(m_grounded) + "\n";

    CharacterController* characterController = GetCharacterController();
    if (characterController) {
        result += " - Character Controller Position: " + Util::Vec3ToString(characterController->GetFootPosition()) + "\n";
    }

    return result + "\n";
}