#include "Player.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Physics/Physics.h"
#include "Util/Util.h"
#include "Managers/OpenableManager.h"
#include "World/World.h"
#include <algorithm>
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"



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


    //m_rayHitObjectType = UniqueID::GetType(m_bvhRayResult.objectId);
    //m_rayhitObjectId = m_bvhRayResult.objectId;
    //m_rayHitPosition = m_bvhRayResult.hitPosition;

    // Store the closest of the two hits
    //m_rayHitObjectType = ObjectType::UNDEFINED;
    //m_rayhitObjectId = 0;
    //m_rayHitPosition = glm::vec3(0.0f);
    //
    //if (m_physXRayResult.hitFound && m_bvhRayResult.hitFound) {
    //    float physXDistance = glm::distance(m_physXRayResult.hitPosition, GetCameraPosition());
    //    float bvhDistance = glm::distance(m_bvhRayResult.hitPosition, GetCameraPosition());
    //
    //    if (physXDistance < bvhDistance) {
    //        m_rayHitObjectType = m_physXRayResult.userData.objectType;
    //        m_rayhitObjectId = m_physXRayResult.userData.objectId;
    //        m_rayHitPosition = m_physXRayResult.hitPosition;
    //        m_rayHitFound = true;
    //    }
    //    else {
    //        m_rayHitObjectType = m_bvhRayResult.objectType;
    //        m_rayhitObjectId = m_bvhRayResult.objectId;
    //        m_rayHitPosition = m_bvhRayResult.hitPosition;
    //        m_rayHitFound = true;
    //    }
    //}    
    //// Or if there was only a physx ray hit...
    //else if (m_physXRayResult.hitFound) {
    //    m_rayHitObjectType = m_physXRayResult.userData.objectType;
    //    m_rayhitObjectId = m_physXRayResult.userData.objectId;
    //    m_rayHitPosition = m_physXRayResult.hitPosition;
    //    m_rayHitFound = true;
    //}
    //// Or if there was only a bvh ray hit...
    //else if (m_bvhRayResult.hitFound) {
    //    m_rayHitObjectType = m_bvhRayResult.objectType;
    //    m_rayhitObjectId = m_bvhRayResult.objectId;
    //    m_rayHitPosition = m_bvhRayResult.hitPosition;
    //    m_rayHitFound = true;
    //}
    //else {
    //    // No ray hit
    //}
}


void Player::UpdateInteract() {
    m_interactObjectId = NO_ID;
    m_interactOpenableId = NO_ID;
    m_interactCustomId = NO_ID;

    if (!ViewportIsVisible()) return;

    // Probably make this cleaner, but for now this handles the fact you can interact while inventory is open. 
    if (InventoryIsOpen()) return;

    // Rewrite me in such a way to consider pickups and openable ids only, you'll need to think how doors and piano keys will work with that...
    //{
    //    // If ray hit object is intractable, store it       
    //    if (World::ObjectTypeIsInteractable(m_rayHitObjectType, m_rayhitObjectId, GetCameraPosition(), m_rayHitPosition)) {
    //        m_interactObjectType = m_rayHitObjectType;
    //        m_interactObjectId = m_rayhitObjectId;
    //    }
    //}

    // If it's not an openable, maybe it's parent is? Search and stop at the first parent
    //ObjectType hitObjectType = UniqueID::GetType(m_rayhitObjectId);
    //if (hitObjectType == ObjectType::GENERIC_OBJECT) {
    //   // std::cout << "generic hit " << m_rayhitObjectId << "\n";
    //
    //    //if(GenericObject * parent = World::GetGenericObjects().get(m_interactObjectId)) {
    //    //    parent->GetMeshNodes().SetTransformByMeshName(openable.m_meshName, openable.m_transform);
    //    //    continue;
    //    //}
    //}

    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are
    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are
    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are

    // Overwrite with PhysX overlap test if an overlap with interact object is found
    // //if (m_rayHitObjectType != ObjectType::NO_TYPE) {
    // 
    //     glm::vec3 spherePosition = m_rayHitPosition;
    //     float sphereRadius = 0.15f;
    //     PxCapsuleGeometry overlapSphereShape = PxCapsuleGeometry(sphereRadius, 0);
    //     const PxTransform overlapSphereTranform = PxTransform(Physics::GlmVec3toPxVec3(spherePosition));
    //     PhysXOverlapReport overlapReport = Physics::OverlapTest(overlapSphereShape, overlapSphereTranform, CollisionGroup(GENERIC_BOUNCEABLE | GENERTIC_INTERACTBLE | ITEM_PICK_UP | ENVIROMENT_OBSTACLE));
    // 
    //     // Sort by distance to player
    //     sort(overlapReport.hits.begin(), overlapReport.hits.end(), [this, spherePosition](PhysXOverlapResult& lhs, PhysXOverlapResult& rhs) {
    //         float distanceA = glm::distance(spherePosition, lhs.objectPosition);
    //         float distanceB = glm::distance(spherePosition, rhs.objectPosition);
    //         return distanceA < distanceB;
    //     });
    // 
    //     if (overlapReport.hits.size()) {
    //         PhysicsUserData userData = overlapReport.hits[0].userData;
    //         if (World::ObjectTypeIsInteractable(userData.objectType, userData.objectId, GetCameraPosition(), m_rayHitPosition)) {
    //             if (userData.objectType != ObjectType::DOOR) {
    //                 m_interactObjectType = userData.objectType;
    //                 m_interactObjectId = userData.objectId;
    //             }
    //         }
    //     }
    // //}

    glm::vec3 hitPosition = glm::vec3(0.0f);
    bool hitFound = false;

    // Replace me with some distance check with closest point from hit object AABB
    if (m_bvhRayResult.hitFound) {
        m_interactObjectId = m_bvhRayResult.objectId;
        m_interactOpenableId = m_bvhRayResult.openableId;
        m_interactCustomId = m_bvhRayResult.customId;

        hitPosition = m_bvhRayResult.hitPosition;
        hitFound = true;
    } 

    // Now try see if the PhysX hit is closer, you need this position for the PhysX sweep tests
    if (m_physXRayResult.hitFound && m_physXRayResult.distanceToHit < m_bvhRayResult.distanceToHit) {
        m_interactObjectId = m_physXRayResult.userData.objectId;
        m_interactOpenableId = 0;
        m_interactCustomId = 0;

        hitPosition = m_physXRayResult.hitPosition;
        hitFound = true;
    }

    // Sweep test
    if (hitFound) {
        glm::vec3 spherePosition = hitPosition;
        float sphereRadius = 0.15f;
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
            OpenableManager::TriggerInteract(m_interactOpenableId);
        }

        // Pickups
        if (PickUp* pickUp = World::GetPickUpByObjectId(m_interactObjectId)) {
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