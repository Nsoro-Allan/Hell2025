#include "Player.h"
#include "Audio/Audio.h"
#include "Bible/Bible.h"
#include "Core/Game.h"
#include "Physics/Physics.h"
#include "Util/Util.h"
#include "Managers/OpenableManager.h"
#include "World/World.h"
#include <algorithm>
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "HellLogging.h"

#pragma warning(disable : 26498)

#include "Renderer/Renderer.h"
#include "UniqueID.h"

void Player::UpdateCursorRays() {
    m_physXRayResult.hitFound = false;
    m_bvhRayResult.hitFound = false;
    m_rayHitFound = false;

    if (!ViewportIsVisible()) return;

    float maxRayDistance = 1000.0f;

    // PhysX Ray
    glm::vec3 cameraRayOrigin = GetCameraPosition();
    glm::vec3 cameraRayDirection = GetCameraForward();
    m_physXRayResult = Physics::CastPhysXRay(cameraRayOrigin, cameraRayDirection, maxRayDistance, false, RaycastIgnoreFlags::PLAYER_CHARACTER_CONTROLLERS | RaycastIgnoreFlags::PLAYER_RAGDOLLS);
    
    // Bvh Ray result
    glm::vec3 rayOrigin = GetCameraPosition();
    glm::vec3 rayDir = GetCameraForward();
    m_bvhRayResult = World::ClosestHit(rayOrigin, rayDir, maxRayDistance);
}


void Player::UpdateInteract() {
    m_interactObjectId = NO_ID;
    m_interactOpenableId = NO_ID;
    m_interactCustomId = NO_ID;

    if (!ViewportIsVisible()) return;

    // Probably make this cleaner, but for now this handles the fact you can interact while inventory is open. 
    if (InventoryIsOpen()) return;

    m_interactHitPosition = glm::vec3(-9999.0f);
    bool hitFound = false;

    // Replace me with some distance check with closest point from hit object AABB
    if (m_bvhRayResult.hitFound) {
        m_interactObjectId = m_bvhRayResult.objectId;
        m_interactOpenableId = m_bvhRayResult.openableId;
        m_interactCustomId = m_bvhRayResult.customId;
        m_interactHitPosition = m_bvhRayResult.hitPosition;
        hitFound = true;
    } 

    // Now try see if the PhysX hit is closer, you need this position for the PhysX sweep tests
    if (m_physXRayResult.hitFound && m_physXRayResult.distanceToHit < m_bvhRayResult.distanceToHit) {
        m_interactObjectId = m_physXRayResult.userData.objectId;
        m_interactOpenableId = 0;
        m_interactCustomId = 0;
        m_interactHitPosition = m_physXRayResult.hitPosition;
        hitFound = true;
    }

    // Sweep test
    if (hitFound) {
        float sphereRadius = 0.15f;
        glm::vec3 spherePosition = m_interactHitPosition - GetCameraForward() * (sphereRadius * 1.25f);

        PxCapsuleGeometry overlapSphereShape = PxCapsuleGeometry(sphereRadius, 0);
        const PxTransform overlapSphereTranform = PxTransform(Physics::GlmVec3toPxVec3(spherePosition));
        PhysXOverlapReport overlapReport = Physics::OverlapTest(overlapSphereShape, overlapSphereTranform, CollisionGroup(GENERIC_BOUNCEABLE | GENERTIC_INTERACTBLE | ITEM_PICK_UP | ENVIROMENT_OBSTACLE));

        // Sort by distance to player
        sort(overlapReport.hits.begin(), overlapReport.hits.end(), [this, spherePosition](PhysXOverlapResult& lhs, PhysXOverlapResult& rhs) {
            float distanceA = glm::distance(spherePosition, lhs.objectPosition);
            float distanceB = glm::distance(spherePosition, rhs.objectPosition);
            return distanceA < distanceB;
            });

        if (overlapReport.hits.size()) {
            PhysicsUserData userData = overlapReport.hits[0].userData;

            if (World::GetPickUpByObjectId(userData.objectId)) {
                m_interactObjectId = userData.objectId;
                m_interactOpenableId = 0;
                m_interactCustomId = 0;
            }
        }
    }

    //Renderer::DrawPoint(hitPosition, GREEN);
    
    ObjectType interactObjectType = UniqueID::GetType(m_interactObjectId);

    // Convenience bool for setting crosshair
    m_interactFound = false;

    if (OpenableManager::IsInteractable(m_interactOpenableId, GetCameraPosition())) m_interactFound = true;
    if (interactObjectType == ObjectType::PIANO && m_interactCustomId != 0)         m_interactFound = true;
    if (interactObjectType == ObjectType::PICK_UP)                                  m_interactFound = true;

    // Bail if nothing to interact with
    if (!InteractFound()) return;

    // PRESSED interact key
    if (PressedInteract()) {
        if (OpenableManager::GetOpenableByOpenableId(m_interactOpenableId)) {

            std::string openableText = OpenableManager::TriggerInteract(m_interactOpenableId, GetCameraPosition(), GetCameraForward());
            if (openableText != "") {
                m_typeWriter.DisplayText(openableText);
            }
        }

        // Pickups
        if (PickUp* pickUp = World::GetPickUpByObjectId(m_interactObjectId)) {

            if (pickUp->GetType() == PickUpType::WEAPON) {
                m_inventory.GiveWeapon(pickUp->GetName());
            }
            if (pickUp->GetType() == PickUpType::AMMO) {
                m_inventory.GiveAmmo(pickUp->GetName(), Bible::GetAmmoPickUpAmount(pickUp->GetName()));
            }
            if (pickUp->GetType() == PickUpType::UNDEFINED) {
                Logging::Warning() << "Player " << m_viewportIndex << " tried to pick up a PickUp with name '" << pickUp->GetName() << "' but type '" << Util::PickUpTypeToString(pickUp->GetType()) << "'";
            }

            World::RemoveObject(m_interactObjectId);
            Audio::PlayAudio("ItemPickUp.wav", 1.0f);
        }
    }

    // PRESSING interact key
    if (PressingInteract()) {

        // Piano keys
        if (interactObjectType == ObjectType::PIANO && m_interactCustomId != 0) {
            if (Piano* piano = World::GetPianoByObjectId(m_interactObjectId)) {
                piano->PressKey(m_interactCustomId);
            }
        }
    }
    
    if (Input::KeyPressed(HELL_KEY_P)) {

        glm::vec3 rayOrigin = GetCameraPosition();
        glm::vec3 rayDir = GetCameraForward();
        float maxRayDistance = 100.0f;

        BvhRayResult result = World::ClosestHit(rayOrigin, rayDir, maxRayDistance);
        if (result.hitFound) {
            // Sit at 
            //if (result.objectType == ObjectType::PIANO) {
            //    for (Piano& potentialPiano : World::GetPianos()) {
            //        //if (potentialPiano.PianoBodyPartKeyExists(result.objectId)) {
            //        //    SitAtPiano(potentialPiano.GetObjectId());
            //        //}
            //
            //        // FIX MEEEEEEE
            //        // FIX MEEEEEEE
            //        // FIX MEEEEEEE
            //    }
            //}
        }
    }
}