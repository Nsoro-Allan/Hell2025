#include "World.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"

#include "Ragdoll/RagdollManager.h"
#include "UniqueID.h"

#include "BVH/Cpu/CpuBvh.h"

namespace World {
    float g_fleshHitHitTimer = 0.0f; // sound can only play if this is less or equal to 0.0f
    constexpr float g_flashHitAudioDelay = 0.2f; // sound can only play if this is less or equal to 0.0f
    bool g_awaitingFleshAudio = false;

    void SpawnBlood(const glm::vec3& position, const glm::vec3& direction);

    void TriggerFleshHit() {
        if (g_fleshHitHitTimer <= 0.0f) {
            g_fleshHitHitTimer = g_flashHitAudioDelay;
            Game::PlayFleshImpactAudio();
        }
    }


    void ProcessBullets() {

        if (Input::KeyPressed(HELL_KEY_NUMPAD_5)) {
            std::cout << "Light count: " << World::GetLights().size() << "\n";
        }

        g_fleshHitHitTimer -= Game::GetDeltaTime();
        g_fleshHitHitTimer = std::max(g_fleshHitHitTimer, 0.0f);

        std::vector<Bullet>& bullets = GetBullets();
        std::vector<Bullet> newBullets;
        bool glassWasHit = false;
        bool rooDeath = false;

        for (Bullet& bullet : bullets) {
            // Cast PhysX ray
            glm::vec3 rayOrigin = bullet.GetOrigin();
            glm::vec3 rayDirection = bullet.GetDirection();
            float rayLength = bullet.GetRayLength();

            std::vector<PxRigidActor*> ignoredActors;

            Player* player = Game::GetPlayerByPlayerId(bullet.GetOwnerObjectId());
            if (player) {
                auto ragdollActors = Physics::GetRagdollPxRigidActors(player->GetRagdollId());
                ignoredActors.insert(ignoredActors.end(), ragdollActors.begin(), ragdollActors.end());
            }

            PhysXRayResult physXRayResult = Physics::CastPhysXRay(rayOrigin, rayDirection, rayLength, false, RaycastIgnoreFlags::PLAYER_CHARACTER_CONTROLLERS, ignoredActors);
            BvhRayResult bvhRayResult = World::ClosestHit(rayOrigin, bullet.GetDirection(), rayLength);

            // Defaults
            bool hitFound = false;
            uint64_t objectId = 0;
            uint64_t physicsId = 0;
            int32_t localMeshNodeIndex = -1;
            glm::vec3 hitPosition = glm::vec3(0.0f);
            glm::vec3 hitNormal = glm::vec3(0.0f);

            // BVH hit
            if (bvhRayResult.hitFound) {
                hitFound = true;
                objectId = bvhRayResult.objectId;
                physicsId = 0;
                localMeshNodeIndex = bvhRayResult.localMeshNodeIndex;
                hitPosition = bvhRayResult.hitPosition;
                hitNormal = bvhRayResult.hitNormal;
            }

            // PhysX hit
            if (physXRayResult.hitFound) {
                float distToPhysXHit = glm::distance(physXRayResult.hitPosition, bullet.GetOrigin());

                // Overwrite only if PhysX hit is closer
                if (!bvhRayResult.hitFound || bvhRayResult.hitFound && distToPhysXHit < bvhRayResult.distanceToHit) {
                    hitFound = true;
                    objectId = physXRayResult.userData.objectId;
                    physicsId = physXRayResult.userData.physicsId;
                    localMeshNodeIndex = -1;
                    hitPosition = physXRayResult.hitPosition;
                    hitNormal = physXRayResult.hitNormal;
                }
            }

            // Hit found?
            if (hitFound) {

                // Retrieve the hit MeshNode, this could be nullptr if the hit was a physics object
                MeshNode* meshNode = World::GetMeshNodeByObjectIdAndLocalNodeIndex(objectId, localMeshNodeIndex);

                bool glassHit = meshNode && (meshNode->blendingMode == BlendingMode::GLASS ||
                                             meshNode->blendingMode == BlendingMode::MIRROR ||
                                             meshNode->blendingMode == BlendingMode::STAINED_GLASS);

                bool createDecal = (meshNode && meshNode->decalType != DecalType::UNDEFINED) ||
                                   (Physics::GetRigidStaitcById(physicsId) != nullptr);

                if (!bullet.CreatesDecals()) {
                    createDecal = false;
                }

                //bool createBlood = (Physics::GetRagdollById(physicsId) != nullptr) ||
                //                   (RagdollManager::GetRagdollV2ById(physicsId) != nullptr);
                //
                //if (Physics::GetRigidDynamicById(physicsId) != nullptr) {
                //    createBlood = true;
                //    std::cout << "Shot rigid dynamic\n";
                //}


                // Create the decal
                if (createDecal) {
                    Decal2CreateInfo decalCreateInfo;
                    decalCreateInfo.surfaceHitPosition = hitPosition;
                    decalCreateInfo.parentObjectId = objectId;
                    decalCreateInfo.localMeshNodeIndex = localMeshNodeIndex;
                    decalCreateInfo.surfaceHitNormal = hitNormal;
                    AddDecal2(decalCreateInfo);

                    // Create second decal on opposite side if glass was hit + spawn a new bullet
                    if (glassHit) {
                        decalCreateInfo.surfaceHitNormal *= glm::vec3(-1.0f);
                        AddDecal2(decalCreateInfo);

                        if (bullet.CreatesFolloWThroughBulletOnGlassHit() && meshNode->blendingMode != BlendingMode::MIRROR) {
                            BulletCreateInfo bulletCreateInfo;
                            bulletCreateInfo.origin = hitPosition + bullet.GetDirection() * glm::vec3(0.05f);
                            bulletCreateInfo.direction = bullet.GetDirection();
                            bulletCreateInfo.damage = bullet.GetDamage();
                            bulletCreateInfo.weaponIndex = bullet.GetWeaponIndex();
                            bulletCreateInfo.ownerObjectId = bullet.GetOwnerObjectId();
                            newBullets.emplace_back(Bullet(bulletCreateInfo));
                        }

                        Game::PlayGlassHitAudio();
                    }
                }

                // Trigger the closest piano note on piano hit
                if (bullet.PlaysPiano()) {
                    if (Piano* piano = World::GetPianoByObjectId(objectId)) {
                        piano->TriggerInternalNoteFromExternalBulletHit(hitPosition);
                    }
                }

                // Dobermann hit
                for (Dobermann& dobermann : GetDobermanns()) {
                    if (objectId == dobermann.GetRagdollV2Id()) {
                        dobermann.TakeDamage(bullet.GetDamage());
                        SpawnBlood(hitPosition, -bullet.GetDirection());
                        TriggerFleshHit();
                    }

                    //if (objectType == ObjectType::RAGDOLL_V2) {
                        //float strength = 100000.0f;
                        //glm::vec3 force = bullet.GetDirection() * strength;
                        //RagdollManager::AddForce(physicsId, force);
                    //}
                }

                // Kangaroo hit
                for (AnimatedGameObject& animatedGameObject : GetAnimatedGameObjects()) {
                    if (animatedGameObject.GetRagdollId() == objectId) {

                        for (Kangaroo& kangaroo : GetKangaroos()) {
                            if (kangaroo.GetAnimatedGameObject() == &animatedGameObject) {
                                SpawnBlood(hitPosition, -bullet.GetDirection());
                                TriggerFleshHit();
                            }
                        }
                    }
                }

                // Shark Kit
                if (Shark* shark = World::GetSharkByObjectId(objectId)) {
                    shark->GiveDamage(bullet.GetOwnerObjectId(), bullet.GetDamage());
                    SpawnBlood(hitPosition, -bullet.GetDirection());
                    TriggerFleshHit();
                }

                // Apply physics force      (THIS DOES NOT WORK FOR BVH PHYSICS HITS YOU THINK?????)
                if (physicsId != 0) {
                    float strength = 25000.0f;
                    strength = 15000.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    RagdollManager::AddForce(physicsId, force);
                    Physics::AddFoceToRigidDynamic(physicsId, force);
                    std::cout << "applied physx force\n";
                }

                // Player ragdoll?
                // REWRITE ME TO NOT BE SO SHIT!!! Don't rely on object type
                // REWRITE ME TO NOT BE SO SHIT!!! Don't rely on object type
                // REWRITE ME TO NOT BE SO SHIT!!! Don't rely on object type
                if (physXRayResult.userData.objectType == ObjectType::RAGDOLL_PLAYER) {
                    Player* player = Game::GetPlayerByPlayerId(objectId);
                    if (player) {

                        // Head shot hack
                        if (Ragdoll* ragdoll = player->GetRagdoll()) {
                            int max = std::min(ragdoll->m_rigidDynamicIds.size(), ragdoll->m_correspondingBoneNames.size());
                            for (int i = 0; i < max; i++) {
                                if (ragdoll->m_rigidDynamicIds[i] == physXRayResult.userData.physicsId) {
                                    if (ragdoll->m_correspondingBoneNames[i] == "CC_Base_Head") {
                                        player->Kill(true);
                                    }
                                }
                            }
                        }

                        player->GiveDamage(bullet.GetDamage(), bullet.GetOwnerObjectId());
                        TriggerFleshHit();

                        // REMOVE ME!!!! you are already doing this below. figure out better force system
                        float strength = 1000.0f;
                        glm::vec3 force = bullet.GetDirection() * strength;
                        Physics::AddFoceToRigidDynamic(physicsId, force);
                        SpawnBlood(hitPosition, -bullet.GetDirection());
                    }
                }

                // Decal texture painting
                if (bullet.CreatesDecalTexturePaintedWounds()) {
                    DecalPaintingInfo decalPaintingInfo;
                    decalPaintingInfo.rayOrigin = bullet.GetOrigin();
                    decalPaintingInfo.rayDirection = bullet.GetDirection();
                    RenderDataManager::SubmitDecalPaintingInfo(decalPaintingInfo);
                }

                // This is probably sketchy...
                if (PickUp* pickUp = World::GetPickUpByObjectId(objectId)) {
                    float strength = 250.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    pickUp->GetMeshNodes().AddForceToPhsyics(force);
                }
                if (GenericObject* object = World::GetGenericObjectById(objectId)) {
                    float strength = 250.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    object->GetMeshNodes().AddForceToPhsyics(force);
                }

                // TEST FOR CHAIN
                //if (rayResult.hitFound) {
                //    PhysicsType& physicsType = rayResult.userData.physicsType;
                //    ObjectType& objectType = rayResult.userData.objectType;
                //    uint64_t physicsId = rayResult.userData.physicsId;
                //    uint64_t objectId = rayResult.userData.objectId;
                //
                //    // Apply force if object is dynamic
                //    if (physicsType == PhysicsType::RIGID_DYNAMIC) {
                //        //float strength = 200.0f;
                //        float strength = 1000.0f;
                //        glm::vec3 force = bullet.GetDirection() * strength;
                //        Physics::AddFoceToRigidDynamic(physicsId, force);
                //        std::cout << "Shot a rigid dynamic TEST\n";
                //    }
                //}
            }

            // On hit
            PhysXRayResult rayResult;
            if (rayResult.hitFound) {
                PhysicsType& physicsType = rayResult.userData.physicsType;
                ObjectType& objectType = rayResult.userData.objectType;
                uint64_t physicsId = rayResult.userData.physicsId;
                uint64_t objectId = rayResult.userData.objectId;
            
                // Blood
                //if (objectType == ObjectType::RAGDOLL_PLAYER ||
                //    objectType == ObjectType::RAGDOLL_ENEMY ||
                //    objectType == ObjectType::RAGDOLL_V2 ||
                //    objectType == ObjectType::SHARK) {
                //    SpawnBlood(rayResult.hitPosition, -bullet.GetDirection());
                //}
            
                // Apply force if object is dynamic
                if (physicsType == PhysicsType::RIGID_DYNAMIC) {
                    //float strength = 200.0f;
                    float strength = 1000.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    Physics::AddFoceToRigidDynamic(physicsId, force);
                    std::cout << "Shot a rigid dynamic\n";
                }
            }
        }

        // Wipe old bullets, and replace with any new ones that got spawned from glass hits
        bullets = newBullets;
    }

    void SpawnBlood(const glm::vec3& position, const glm::vec3& direction) {
        // Create VAT blood then and there
        AddVATBlood(position, direction);

        // For the screenspace decal blood, cast a ray directly down and create it at the ray hit position
        glm::vec3 rayOrigin = position;
        glm::vec3 rayDirection = glm::vec3(0.0f, -1.0f, 0.0f);
        float rayLength = 100;
        PhysXRayResult rayResult = Physics::CastPhysXRayStaticEnvironment(rayOrigin, rayDirection, rayLength);

        if (rayResult.hitFound) {
            ScreenSpaceBloodDecalCreateInfo decalCreateInfo;
            decalCreateInfo.position = rayResult.hitPosition;
            decalCreateInfo.direction = direction;
            AddScreenSpaceBloodDecal(decalCreateInfo);
        }
    }
}