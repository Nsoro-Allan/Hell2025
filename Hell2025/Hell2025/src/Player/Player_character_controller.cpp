#include "Player.h"
#include "Physics/Physics.h"

void Player::CreateCharacterController(const glm::vec3& position) {

    float capsuleHeight = PLAYER_CAPSULE_HEIGHT;
    capsuleHeight = m_viewHeightStanding - PLAYER_CAPSULE_RADIUS - PLAYER_CAPSULE_RADIUS;

    float capsuleRadius = PLAYER_CAPSULE_RADIUS;

    PhysicsFilterData physicsFilterData;
    physicsFilterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    physicsFilterData.collisionGroup = CollisionGroup::CHARACTER_CONTROLLER;
    physicsFilterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | CHARACTER_CONTROLLER);

    m_characterControllerId = Physics::CreateCharacterController(m_playerId, position, capsuleHeight, capsuleRadius, physicsFilterData);
}

void Player::SetFootPosition(glm::vec3 position) {
    CharacterController* characterControler = Physics::GetCharacterControllerById(m_characterControllerId);
    if (characterControler) {
        PxController* pxControler = characterControler->GetPxController();
        PxExtendedVec3 pxVec3 = PxExtendedVec3(position.x, position.y, position.z);
        pxControler->setFootPosition(pxVec3);
    }
}

PxShape* Player::GetCharacterControllerShape() {

    CharacterController* characterControler = Physics::GetCharacterControllerById(m_characterControllerId);
    if (characterControler) {
        PxController* pxControler = characterControler->GetPxController();

        PxShape* shape;
        pxControler->getActor()->getShapes(&shape, 1);
        return shape;
    } 
    else {
        return nullptr;
    }
}

PxRigidDynamic* Player::GetCharacterControllerActor() {
    CharacterController* characterControler = Physics::GetCharacterControllerById(m_characterControllerId);
    if (characterControler) {
        PxController* pxControler = characterControler->GetPxController();

        return pxControler->getActor();
    }
    else {
        return nullptr;
    }
}

