#include "World.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "Ragdoll/RagdollManager.h"
#include "UniqueID.h"

namespace World {
    void EvaluatePianoKeyBulletHit(Bullet& bullet);
    void SpawnBlood(const glm::vec3& position, const glm::vec3& direction);

    void ProcessBullets() {
        std::vector<Bullet>& bullets = GetBullets();
        std::vector<Bullet> newBullets;
        bool glassWasHit = false;
        bool rooDeath = false;

        for (Bullet& bullet : bullets) {

            // Did the bullet hit a piano key directly?
            if (bullet.PlaysPiano()) {
                EvaluatePianoKeyBulletHit(bullet);
            }

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

            PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDirection, rayLength, false, RaycastIgnoreFlags::PLAYER_CHARACTER_CONTROLLERS, ignoredActors);

            // On hit
            if (rayResult.hitFound) {
                PhysicsType& physicsType = rayResult.userData.physicsType;
                ObjectType& objectType = rayResult.userData.objectType;
                uint64_t physicsId = rayResult.userData.physicsId;
                uint64_t objectId = rayResult.userData.objectId;

                // Did you hit a dobermann?
                for (Dobermann& dobermann : GetDobermanns()) {
                    if (objectId == dobermann.GetRagdollV2Id()) {
                        dobermann.TakeDamage(bullet.GetDamage());
                    }

                    // Find a way to abstract this a bit nicer
                    if (bullet.CreatesDecalTexturePaintedWounds()) {
                        DecalPaintingInfo decalPaintingInfo;
                        decalPaintingInfo.rayOrigin = bullet.GetOrigin();
                        decalPaintingInfo.rayDirection = bullet.GetDirection();
                        RenderDataManager::SubmitDecalPaintingInfo(decalPaintingInfo);
                    }

                }

                // Blood
                if (objectType == ObjectType::RAGDOLL_PLAYER ||
                    objectType == ObjectType::RAGDOLL_ENEMY ||
                    objectType == ObjectType::RAGDOLL_V2 ||
                    objectType == ObjectType::SHARK) {
                    SpawnBlood(rayResult.hitPosition, -bullet.GetDirection());
                }

                // Shot a player ragdoll?
                if (objectType == ObjectType::RAGDOLL_PLAYER) {
                    Player* player = Game::GetPlayerByPlayerId(objectId);
                    if (player) {
                        player->Kill();

                        // REMOVE ME!!!! you are already doing this below. figure out better force system
                        float strength = 1000.0f;
                        glm::vec3 force = bullet.GetDirection() * strength;
                        Physics::AddFoceToRigidDynamic(physicsId, force);
                    }
                }

                if (objectType == ObjectType::SHARK) {
                    Shark* shark = World::GetSharkByObjectId(objectId);
                    if (shark) {
                        shark->GiveDamage(bullet.GetOwnerObjectId(), bullet.GetDamage());
                    }
                }


                if (objectType == ObjectType::RAGDOLL_V2) {
                    float strength = 100000.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    RagdollManager::AddForce(rayResult.userData.physicsId, force);
                }

                // Shot enemy ragdoll?
                if (objectType == ObjectType::RAGDOLL_ENEMY && !rooDeath) {

                    // Give damage to enemy
                    for (AnimatedGameObject& animatedGameObject : GetAnimatedGameObjects()) {
                        if (animatedGameObject.GetRagdollId() == objectId) {

                            if (bullet.CreatesDecalTexturePaintedWounds()) {
                                DecalPaintingInfo decalPaintingInfo;
                                decalPaintingInfo.rayOrigin = bullet.GetOrigin();
                                decalPaintingInfo.rayDirection = bullet.GetDirection();
                                RenderDataManager::SubmitDecalPaintingInfo(decalPaintingInfo);
                            }

                            for (Kangaroo& kangaroo : GetKangaroos()) {
                                if (kangaroo.GetAnimatedGameObject() == &animatedGameObject) {
                                    //kangaroo.GiveDamage(bullet.GetDamage());
                                }
                            }
                        }
                    }

                    // Spawn volumetric blood
                    //glm::vec3 position = rayResult.hitPosition;
                    //glm::vec3 front = bullet.GetDirection() * glm::vec3(-1);
                    //World::AddVATBlood(position, -bullet.GetDirection());
                }


                // Apply force if object is dynamic
                if (physicsType == PhysicsType::RIGID_DYNAMIC) {
                    float strength = 200.0f;
                    glm::vec3 force = bullet.GetDirection() * strength;
                    Physics::AddFoceToRigidDynamic(physicsId, force);
                    //std::cout << "Shot a rigid dynamic\n";
                }

                // Add decals for rigid static
                if (physicsType == PhysicsType::RIGID_STATIC) {
                    DecalCreateInfo decalCreateInfo;
                    decalCreateInfo.parentPhysicsId = physicsId;
                    decalCreateInfo.parentPhysicsType = physicsType;
                    decalCreateInfo.parentObjectId = objectId;
                    decalCreateInfo.parentObjectType = objectType;
                    decalCreateInfo.surfaceHitPosition = rayResult.hitPosition;
                    decalCreateInfo.surfaceHitNormal = rayResult.hitNormal;

                    // Plaster decal
                    if (objectType == ObjectType::WALL_SEGMENT ||
                        objectType == ObjectType::PLANE ||
                        objectType == ObjectType::DOOR ||
                        objectType == ObjectType::PIANO) {
                        decalCreateInfo.decalType = DecalType::PLASTER;
                        if (bullet.CreatesDecals()) {
                            AddDecal(decalCreateInfo);
                        }
                    }

                    // Did the bullet hit the piano itself, and thus trigger the closest key? This simulates shooting the strings inside the piano
                    if (objectType == ObjectType::PIANO && bullet.PlaysPiano()) {
                        Piano* piano = World::GetPianoByObjectId(objectId);
                        if (piano) {
                            piano->TriggerInternalNoteFromExternalBulletHit(rayResult.hitPosition);
                        }
                    }

                    // Glass decal
                    if (objectType == ObjectType::WINDOW && bullet.CreatesFolloWThroughBulletOnGlassHit()) {
                        decalCreateInfo.decalType = DecalType::GLASS;
                        AddDecal(decalCreateInfo);

                        decalCreateInfo.surfaceHitNormal *= glm::vec3(-1.0f);
                        AddDecal(decalCreateInfo);

                        // Create new bullet
                        BulletCreateInfo bulletCreateInfo;
                        bulletCreateInfo.origin = rayResult.hitPosition + bullet.GetDirection() * glm::vec3(0.05f);
                        bulletCreateInfo.direction = bullet.GetDirection();
                        bulletCreateInfo.damage = bullet.GetDamage();
                        bulletCreateInfo.weaponIndex = bullet.GetWeaponIndex();
                        bulletCreateInfo.ownerObjectId = bullet.GetOwnerObjectId();
                        newBullets.emplace_back(Bullet(bulletCreateInfo));

                        glassWasHit = true;
                    }
                }
            }
        }

        if (glassWasHit) {
            Audio::PlayAudio("GlassImpact.wav", 2.0f);
        }

        // Wipe old bullets, and replace with any new ones that got spawned from glass hits
        bullets = newBullets;;
    }

    void EvaluatePianoKeyBulletHit(Bullet& bullet) {
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            glm::vec3 rayOrigin = bullet.GetOrigin();
            glm::vec3 rayDir = bullet.GetDirection();
            float maxRayDistance = 100.0f;

            BvhRayResult result = ClosestHit(rayOrigin, rayDir, maxRayDistance, i);
            if (result.hitFound) {
                if (UniqueID::GetType(result.objectId) == ObjectType::PIANO_KEY) {
                    for (Piano& piano : World::GetPianos()) {
                        if (piano.PianoKeyExists(result.objectId)) {
                            PianoKey* pianoKey = piano.GetPianoKey(result.objectId);
                            if (pianoKey) {
                                pianoKey->PressKey();
                            }
                        }
                    }

                }
            }
        }
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