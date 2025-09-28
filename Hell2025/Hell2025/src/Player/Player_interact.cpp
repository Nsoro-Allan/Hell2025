#include "Player.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Physics/Physics.h"
#include "Util/Util.h"
#include "Core/OpenStateHandlerManager.h"
#include "World/World.h"
#include <algorithm>
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"

#pragma warning(disable : 26498)

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
    m_bvhRayResult = World::ClosestHit(rayOrigin, rayDir, maxRayDistance, m_viewportIndex);

    // Store the closest of the two hits
    m_rayHitObjectType = ObjectType::UNDEFINED;
    m_rayhitObjectId = 0;
    m_rayHitPosition = glm::vec3(0.0f);

    if (m_physXRayResult.hitFound && m_bvhRayResult.hitFound) {
        float physXDistance = glm::distance(m_physXRayResult.hitPosition, GetCameraPosition());
        float bvhDistance = glm::distance(m_bvhRayResult.hitPosition, GetCameraPosition());

        if (physXDistance < bvhDistance) {
            m_rayHitObjectType = m_physXRayResult.userData.objectType;
            m_rayhitObjectId = m_physXRayResult.userData.objectId;
            m_rayHitPosition = m_physXRayResult.hitPosition;
            m_rayHitFound = true;
        }
        else {
            m_rayHitObjectType = m_bvhRayResult.objectType;
            m_rayhitObjectId = m_bvhRayResult.objectId;
            m_rayHitPosition = m_bvhRayResult.hitPosition;
            m_rayHitFound = true;
        }
    }    
    // Or if there was only a physx ray hit...
    else if (m_physXRayResult.hitFound) {
        m_rayHitObjectType = m_physXRayResult.userData.objectType;
        m_rayhitObjectId = m_physXRayResult.userData.objectId;
        m_rayHitPosition = m_physXRayResult.hitPosition;
        m_rayHitFound = true;
    }
    // Or if there was only a bvh ray hit...
    else if (m_bvhRayResult.hitFound) {
        m_rayHitObjectType = m_bvhRayResult.objectType;
        m_rayhitObjectId = m_bvhRayResult.objectId;
        m_rayHitPosition = m_bvhRayResult.hitPosition;
        m_rayHitFound = true;
    }
    else {
        // No ray hit
    }
}


void Player::UpdateInteract() {
    m_interactObjectType = ObjectType::NONE;
    m_interactObjectId = 0;

    if (!ViewportIsVisible()) return;

    // Probably make this cleaner, but for now this handles the fact you can interact while inventory is open. 
    if (InventoryIsOpen()) return;

    // If ray hit object is intractable, store it
    if (World::ObjectTypeIsInteractable(m_rayHitObjectType, m_rayhitObjectId, GetCameraPosition(), m_rayHitPosition)) {
        m_interactObjectType = m_rayHitObjectType;
        m_interactObjectId = m_rayhitObjectId;
    }


    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are
    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are
    // you broke the test below adding ragdolls, you are probably hitting it, find a way to omit the ragdoll from overlap test, although looks like you are

    // Overwrite with PhysX overlap test if an overlap with interact object is found
    if (m_rayHitObjectType != ObjectType::NONE) {
    
        glm::vec3 spherePosition = m_rayHitPosition;
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
            if (World::ObjectTypeIsInteractable(userData.objectType, userData.objectId, GetCameraPosition(), m_rayHitPosition)) {
                if (userData.objectType != ObjectType::DOOR) {
                    m_interactObjectType = userData.objectType;
                    m_interactObjectId = userData.objectId;
                }
            }
        }
    }

    // Convenience bool for setting crosshair
    m_interactFound = (m_interactObjectType != ObjectType::NONE);

    if (PressedInteract()) {

        // Pickups
        if (m_interactObjectType == ObjectType::PICK_UP) {
            PickUp* pickUp = World::GetPickUpByObjectId(m_interactObjectId);
            if (pickUp) {
                World::RemoveObject(m_interactObjectId);
                Audio::PlayAudio("ItemPickUp.wav", 1.0f);
            }
        }
        // Doors
        if (m_interactObjectType == ObjectType::DOOR) {
            Door* door = World::GetDoorByObjectId(m_interactObjectId);
            if (door) {
                door->Interact();
            }
        }
        // Piano stuff
        if (m_interactObjectType == ObjectType::PIANO_KEYBOARD_COVER) {
            Piano* piano = World::GetPianoByMeshNodeObjectId(m_interactObjectId);
            if (piano) {
                piano->InteractWithKeyboardCover();
            }
        }
        if (m_interactObjectType == ObjectType::PIANO_SHEET_MUSIC_REST) {
            Piano* piano = World::GetPianoByMeshNodeObjectId(m_interactObjectId);
            if (piano) {
                piano->InteractWithSheetMusicRestCover();
            }
        }
        if (m_interactObjectType == ObjectType::PIANO_TOP_COVER) {
            Piano* piano = World::GetPianoByMeshNodeObjectId(m_interactObjectId);
            if (piano) {
                piano->InteractWithTopCover();
            }
        }


        // Toilet stuff
        if (m_interactObjectType == ObjectType::TOILET_LID) {
            Toilet* toilet = World::GetToiletByMeshNodeObjectId(m_interactObjectId);
            if (toilet) {
                toilet->InteractWithLid();
            }
        }
        if (m_interactObjectType == ObjectType::TOILET_SEAT) {
            Toilet* toilet = World::GetToiletByMeshNodeObjectId(m_interactObjectId);
            if (toilet) {
                toilet->InteractWithSeat();
            }
        }

        if (m_interactObjectType == ObjectType::DRAWER) {
            for (Drawers& drawers: World::GetDrawers()) {
                const MeshNodes& meshNodes = drawers.GetMeshNodes();
                if (meshNodes.HasNodeWithObjectId(m_interactObjectId)) {
                    for (int i = 0; i < meshNodes.GetNodeCount(); i++) {
                        // If this mesh nodes id matches the raycasted object id, and it has a valid open handler id
                        if (meshNodes.m_objectIds[i] == m_interactObjectId) {
                            if (meshNodes.m_openHandlerStateIds[i] != 0) {
                                OpenStateHandlerManager::TriggerInteract(meshNodes.m_openHandlerStateIds[i]);
                            }
                        }
                    }
                }
            }
        }
    }

    if (PressingInteract()) {
        // Piano keys
        if (m_interactObjectType == ObjectType::PIANO_KEY) {
            PianoKey* pianoKey = World::GetPianoKeyByObjectId(m_interactObjectId);
            if (pianoKey) {
                pianoKey->PressKey();
            }
        }
    } 

    if (Input::KeyPressed(HELL_KEY_P)) {

        glm::vec3 rayOrigin = GetCameraPosition();
        glm::vec3 rayDir = GetCameraForward();
        float maxRayDistance = 100.0f;

        BvhRayResult result = World::ClosestHit(rayOrigin, rayDir, maxRayDistance, m_viewportIndex);
        if (result.hitFound) {

            // Sit at 
            if (result.objectType == ObjectType::PIANO) {
                for (Piano& potentialPiano : World::GetPianos()) {
                    //if (potentialPiano.PianoBodyPartKeyExists(result.objectId)) {
                    //    SitAtPiano(potentialPiano.GetObjectId());
                    //}

                    // FIX MEEEEEEE
                    // FIX MEEEEEEE
                    // FIX MEEEEEEE
                }
            }
        }
    }
}