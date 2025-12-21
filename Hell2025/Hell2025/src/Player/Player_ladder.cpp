#include "Player.h"
#include "World/World.h"

void Player::UpdateLadderIds() {
    m_ladderOverlapIndexFeet = 0;
    m_ladderOverlapIndexEyes = 0;

    for (Ladder& ladder : World::GetLadders()) {
        float sphereRadius = 0.25f;

        if (ladder.GetOverlapHitBoxAABB().IntersectsSphere(GetFootPosition(), sphereRadius)) {
            m_ladderOverlapIndexFeet = ladder.GetObjectId();
        }
        if (ladder.GetOverlapHitBoxAABB().IntersectsSphere(GetCameraPosition(), sphereRadius)) {
            m_ladderOverlapIndexEyes = ladder.GetObjectId();
        }
    }
}

bool Player::IsOverlappingLadder() {
    return (m_ladderOverlapIndexFeet != 0 && m_ladderOverlapIndexEyes != 0);
}

void Player::UpdateLadderMovement(float deltaTime) {
    float ladderClimpingSpeed = 3.5f;

    if (!PressingWalkForward() &&
        !PressingWalkBackward() &&
        !PressingWalkLeft() &&
        !PressingWalkRight()) {
        return;
    }

    if (m_ladderOverlapIndexEyes != 0 && IsMoving() && !IsCrouching()) {
        glm::vec3 ladderMovementDisplacement = glm::vec3(0.0f, 1.0f, 0.0f) * ladderClimpingSpeed * deltaTime;
        Physics::MoveCharacterController(m_characterControllerId, ladderMovementDisplacement);
    }
}