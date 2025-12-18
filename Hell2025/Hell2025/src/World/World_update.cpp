#include "World.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Pathfinding/AStarMap.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"

#include "Ragdoll/RagdollManager.h"
#include "Pathfinding/NavMesh.h"

#include "AssetManagement/AssetManager.h"

namespace World {

    void LazyDebugSpawns();
    void CalculateGPULights();

    // REMOVE ME!
    uint64_t g_testAnimatedGameObject = 0;
    AnimatedGameObject* GetDobermannTest() {
        return GetAnimatedGameObjectByObjectId(g_testAnimatedGameObject);
    }
    
    static float DegToRad(float degrees) { return degrees * (HELL_PI / 180.0f); }

    void Update(float deltaTime) {

        static int chainLinkMeshIndex = AssetManager::GetModelByName("ChainLink")->GetMeshIndices()[0];
        Mesh* chainLinkMesh = AssetManager::GetMeshByIndex(chainLinkMeshIndex);

        glm::vec3 extents = chainLinkMesh->extents;// glm::vec3(0.18f, 0.08f, 0.6f) * 1.0f;
        glm::vec3 chainOrigin = glm::vec3(36, 32.6, 36);

        static uint64_t physicsIdOrigin = 0;
        static uint64_t jointId = 0;

        static std::vector<uint64_t> physicsIds;

        //std::vector<uint64_t> linkPhysicsIds;

        static Transform scaleTransform;
        scaleTransform.scale = extents;
        static glm::mat4 scaleMatrix = scaleTransform.to_mat4();
        static glm::mat4 localOffsetTransform = chainLinkMesh->localTransform;

        glm::vec3 extentsOrigin = glm::vec3(0.05f, 0.05f, 0.05f) * 1.0f;
        static Transform scaleTransformOrigin;
        scaleTransformOrigin.scale = extentsOrigin;
        static glm::mat4 scaleMatrixOrigin = scaleTransformOrigin.to_mat4();

        Transform originTransform;
        originTransform.position = chainOrigin;
        glm::mat4 originMatrix = originTransform.to_mat4();

        static bool runOnce = true;
        if (runOnce) {
            runOnce = false;

            Transform originTransform;
            originTransform.position = chainOrigin;


            PhysicsFilterData filterData;
            filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
            filterData.collisionGroup = CollisionGroup::BULLET_CASING;
            filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

            // Origin kinematic body
            {
                physicsIdOrigin = Physics::CreateRigidDynamicFromBoxExtents(originTransform, extentsOrigin, true, filterData, glm::mat4(1.0f));
                RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(physicsIdOrigin);
            }

            // Chain link
            int linkCount = 2;

            for (int i = 0; i < linkCount; i++) {

                float magicOffsetPercentage = 0.795f;

                Transform linkTransform;
                linkTransform.position = chainOrigin;
                linkTransform.position.z += (chainLinkMesh->extents.z * magicOffsetPercentage) * i;

                glm::mat4 shapeLocalPose = glm::inverse(localOffsetTransform);
                uint64_t physicsId = Physics::CreateRigidDynamicFromBoxExtents(linkTransform, extents, true, filterData, shapeLocalPose);
                RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(physicsId);
                physicsIds.push_back(physicsId);

                PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();
                pxRigidDynamic->setMass(10.0f);

                pxRigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
                pxRigidDynamic->wakeUp();
                float mass = 10.0f;
                PxRigidBodyExt::setMassAndUpdateInertia(*pxRigidDynamic, mass);
                pxRigidDynamic->setAngularDamping(2.0f); // start 1 to 5
                pxRigidDynamic->setLinearDamping(0.1f);

                PhysicsUserData physicsUserData;
                physicsUserData.physicsId = physicsId;
                Physics::SetRigidDynamicUserData(physicsId, physicsUserData);

                //glm::mat4 parentFrame = glm::mat4(1.0f);
                //glm::mat4 childFrame = glm::mat4(1.0f);

                float linkSpacing = (chainLinkMesh->extents.z * magicOffsetPercentage);

                // Joint between parent and child should be halfway between their centers
                glm::mat4 parentFrame = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, +linkSpacing * 0.5f));
                glm::mat4 childFrame = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -linkSpacing * 0.5f));

                if (i == 0) {
                    jointId = Physics::CreateD6Joint(physicsIdOrigin, physicsId, glm::mat4(1.0f), glm::mat4(1.0f)); // identity
                }
                else {
                    uint64_t currentId = physicsIds[i];
                    uint64_t parentId = physicsIds[i-1];
                    jointId = Physics::CreateD6Joint(parentId, currentId, parentFrame, childFrame);
                }

                D6Joint* d6 = Physics::GetD6JointById(jointId);
                auto pxD6 = d6->GetPxD6Joint();

                // Start FREE on all axes
                pxD6->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
                pxD6->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
                pxD6->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

                // Allow it to rotate
                pxD6->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
                pxD6->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE); // eLIMITED this locks it weird like in maya
                pxD6->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);  // eLIMITED this locks it weird like in maya 


                physx::PxJointLimitCone swingLimit(
                    DegToRad(55.0f),   // swing1
                    DegToRad(25.0f)    // swing2
                );
                pxD6->setSwingLimit(swingLimit);

                physx::PxJointAngularLimitPair twistLimit(
                    DegToRad(-20.0f), // lower limit
                    DegToRad(20.0f)   // upper limit
                );
                pxD6->setTwistLimit(twistLimit);
            }
        }

        Renderer::DrawPoint(chainOrigin, GREEN);


        // Origin
        //if (RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(physicsIdOrigin)) {
        //    glm::mat4 modelMatrix = rigidDynamic->GetWorldTransform() * scaleMatrixOrigin;
        //
        //    Material* material = AssetManager::GetMaterialByName("White");
        //    RenderItem renderItem;
        //    renderItem.modelMatrix = modelMatrix;
        //    renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        //    renderItem.baseColorTextureIndex = material->m_basecolor;
        //    renderItem.rmaTextureIndex = material->m_rma;
        //    renderItem.normalMapTextureIndex = material->m_normal;
        //    renderItem.meshIndex = meshIndex;
        //    Util::UpdateRenderItemAABB(renderItem);
        //
        //    RenderDataManager::SubmitRenderItem(renderItem);
        //}

        // Links
        for (int i = 0; i < physicsIds.size(); i++) {
            if (RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(physicsIds[i])) {
                glm::mat4 modelMatrix = rigidDynamic->GetWorldTransform();

                Material* material = AssetManager::GetMaterialByName("Tokarev");
                RenderItem renderItem;
                renderItem.modelMatrix = modelMatrix;
                renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
                renderItem.baseColorTextureIndex = material->m_basecolor;
                renderItem.rmaTextureIndex = material->m_rma;
                renderItem.normalMapTextureIndex = material->m_normal;
                renderItem.meshIndex = chainLinkMeshIndex;
                Util::UpdateRenderItemAABB(renderItem);

                RenderDataManager::SubmitRenderItem(renderItem);

                Renderer::DrawPoint(rigidDynamic->GetWorldTransform()[3], BLUE);

            }
        }



        //std::cout << GetGameObjects().size() << "\n";

        //std::cout << "Fireplace Count: " << GetFireplaces().size() << "\n";

        NavMeshManager::Update();

        //if (Input::KeyPressed(HELL_KEY_LEFT)) {
        //    static MermaidCreateInfo createInfo = GetMermaids()[0].GetCreateInfo();
        //    createInfo.rotation.y += 0.05f;
        //    GetMermaids()[0].Init(createInfo, SpawnOffset());
        //}
        //
        //if (Input::KeyPressed(HELL_KEY_NUMPAD_3)) {
        //
        //    GetGameObjects()[0].SetPosition(Game::GetLocalPlayerByIndex(0)->GetFootPosition());
        //    for (Light& light : GetLights()) {
        //        light.ForceDirty();
        //    }
        //}
        //
        //if (Input::KeyPressed(HELL_KEY_J)) {
        //    PrintObjectCounts();
        //}

        //glm::vec3 rayOrigin = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
        //glm::vec3 rayDir = Game::GetLocalPlayerByIndex(0)->GetCameraForward();
        //glm::vec3 position = glm::vec3(1.0f);
        //float radius = 0.5f;
        //
        //bool rayHit = Util::RayIntersectsSphere(rayOrigin, rayDir, position, radius);
        //glm::vec4 color = rayHit ? GREEN : YELLOW;
        //Renderer::DrawSphere(position, radius, color);
        //
        //if (rayHit) {
        //    std::cout << "ray origin:      " << rayOrigin << "\n";
        //    std::cout << "ray dir:         " << rayDir << "\n";
        //    std::cout << "sphere position: " << position << "\n";
        //    std::cout << "sphere radius:   " << radius << "\n\n";
        //}

        // Display closest AABB to mesh nodes to player 0
        //for (GenericObject& genericObject : GetGenericObjects()) {
        //    for (const MeshNode& meshNode : genericObject.GetMeshNodes().GetNodes()) {
        //        const AABB& aabb = meshNode.worldspaceAabb;
        //        glm::vec3 closestPoint = aabb.NearestPointTo(Game::GetLocalPlayerByIndex(0)->GetCameraPosition());
        //        Renderer::DrawAABB(aabb, PINK);
        //        Renderer::DrawPoint(closestPoint, YELLOW);
        //    }
        //}

      //if (g_testAnimatedGameObject == 0) {
      //    g_testAnimatedGameObject = CreateAnimatedGameObject();
      //    AnimatedGameObject* dobermann = GetDobermannTest();
      //
      //    bool found = false;
      //    for (RagdollV2& ragdoll : RagdollManager::GetRagdolls()) {
      //        if (ragdoll.GetRagdollName() == "dobermann") {
      //            //dobermann->m_ragdollV2Id = ragdoll.GetRagdollId();
      //            found = true;
      //        }
      //    }
      //    if (!found) {
      //        Logging::Error() << "Failed to set ragdoll by name 'dobermann'";
      //    }
      //    else {
      //        Logging::Debug() << "Successfuly set ragdollV2Id to " << dobermann->m_ragdollV2Id;
      //    }
      //
      //    dobermann->SetSkinnedModel("Dobermann");
      //    //dobermann->PrintMeshNames();
      //    //dobermann->PrintNodeNames();
      //    dobermann->SetAnimationModeToBindPose();
      //    //dobermann->SetMeshMaterialByMeshName("Body", "DobermannMouthBlood");
      //    dobermann->SetPosition(glm::vec3(36.8f, 31.0f, 37.23f));
      //    dobermann->PlayAndLoopAnimation("Main", "Dobermann_idle_loop", 1.0f);
      //}
      //
      //if (Input::KeyPressed(HELL_KEY_I)) {
      //    AnimatedGameObject* dobermann = GetDobermannTest();
      //    dobermann->SetAnimationModeToRagdollV2();
      //}
      

       
        auto& ragdolls = RagdollManager::GetRagdolls();
        for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
            RagdollV2& ragdoll = it->second;

            //ragdoll.Update();

            if (Input::KeyPressed(HELL_KEY_Y)) {
                ragdoll.SetToInitialPose();
                ragdoll.DisableSimulation();

                for (Light& light : GetLights()) {
                    AABB aabb = ragdoll.GetWorldSpaceAABB();
                    if (aabb.IntersectsSphere(light.GetPosition(), light.GetRadius())) {
                        light.ForceDirty();
                    }
                }
            }

            if (Input::KeyPressed(HELL_KEY_O)) {
                ragdoll.EnableSimulation();

                for (Light& light : GetLights()) {
                    AABB aabb = ragdoll.GetWorldSpaceAABB();
                    if (aabb.IntersectsSphere(light.GetPosition(), light.GetRadius())) {
                        light.ForceDirty();
                    }
                }
            }
            ++it;
        }


        //auto pos = glm::vec3(31.0f, 30.4f, 38.25f);
        //Renderer::DrawPoint(pos, RED);

        /*
        {
        // Visualize dot product arc
            glm::vec3 forward = Game::GetLocalPlayerByIndex(0)->GetCameraForward();
            forward.y = 0.0f;
            forward = glm::normalize(forward);
            glm::vec3 origin = Game::GetLocalPlayerByIndex(0)->GetFootPosition();
            float dotThreshold = 0.7f;
            float angle = acos(dotThreshold);
            glm::mat4 rotRight = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0, 1, 0));
            glm::mat4 rotLeft = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            glm::vec3 rightEdge = glm::vec3(rotRight * glm::vec4(forward, 0.0f));
            glm::vec3 leftEdge = glm::vec3(rotLeft * glm::vec4(forward, 0.0f));
            float length = 2.0f;
            glm::vec4 color = RED;

            AnimatedGameObject* roo = GetAnimatedGameObjectByObjectId(g_rooAnimatedGameObject);
            glm::vec3 target = roo->_transform.position;
            glm::vec3 toTarget = glm::normalize(target - origin);
            float dotValue = glm::dot(forward, toTarget);
            if (dotValue >= dotThreshold) {
                color = GREEN;
            }
            Renderer::DrawLine(origin, origin + forward * length, color);     // center
            Renderer::DrawLine(origin, origin + leftEdge * length, color);    // left limit
            Renderer::DrawLine(origin, origin + rightEdge * length, color);   // right limit
        }*/

        ProcessBullets();
        LazyDebugSpawns();


        for (AnimatedGameObject& object : GetAnimatedGameObjects()) object.Update(deltaTime);
        for (BulletCasing& object : GetBulletCasings())             object.Update(deltaTime);
        for (ChristmasLights& object : GetChristmasLights())        object.Update(deltaTime);
        for (ChristmasPresent& object : GetChristmasPresents())     object.Update(deltaTime);
        for (ChristmasTree& object : GetChristmasTrees())           object.Update(deltaTime);
        for (Decal& object : GetDecals())                           object.Update();
        for (Dobermann& object : GetDobermanns())                   object.Update(deltaTime);
        for (Door& object : GetDoors())                             object.Update(deltaTime);
        for (Fence& object : GetFences())                           object.Update();
        for (Fireplace& object : GetFireplaces())                   object.Update(deltaTime);
        for (GameObject& object : GetGameObjects())                 object.Update(deltaTime);
        for (HousePlane& object : GetHousePlanes())                 //object.Update(deltaTime);
        for (GenericObject& object : GetGenericObjects())           object.Update(deltaTime);
        for (Kangaroo& object : GetKangaroos())                     object.Update(deltaTime);
        for (Light& object : GetLights())                           object.Update(deltaTime);
        for (Mermaid& object : GetMermaids())                       object.Update(deltaTime);
        for (Piano& object : GetPianos())                           object.Update(deltaTime);
        for (PickUp& object : GetPickUps())                         object.Update(deltaTime);
        for (PowerPoleSet& object : GetPowerPoleSets())             object.Update();
        for (Road& object : GetRoads())                             object.Update();
        for (Shark& object : GetSharks())                           object.Update(deltaTime);
        for (Tree& object : GetTrees())                             object.Update(deltaTime);
        for (TrimSet& object : GetTrimSets())                       object.Update();
        for (Window& object : GetWindows())                         object.Update(deltaTime);

        //lights[2].SetColor(DEFAULT_LIGHT_COLOR);
        //GetLights()[2].SetStrength(2.0f);
        //GetLights()[2].SetRadius(3.5f);

        if (Input::KeyPressed(HELL_KEY_BACKSPACE)) {
            GetDecals().clear();
        }

        CalculateGPULights();

        // Volumetric blood
        std::vector<VolumetricBloodSplatter>& volumetricBloodSplatters = GetVolumetricBloodSplatters();
        for (int i = 0; i < volumetricBloodSplatters.size(); i++) {
            VolumetricBloodSplatter& volumetricBloodSplatter = volumetricBloodSplatters[i];

            if (volumetricBloodSplatter.GetLifeTime() < 0.9f) {
                volumetricBloodSplatter.Update(deltaTime);
            }
            else {
                volumetricBloodSplatters.erase(volumetricBloodSplatters.begin() + i);
                i--;
            }
        }
    }

    void LazyDebugSpawns() {
        // AKs
        //if (Input::KeyPressed(HELL_KEY_BACKSPACE)) {
        //    PickUpCreateInfo createInfo;
        //    createInfo.position = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
        //    createInfo.position += Game::GetLocalPlayerByIndex(0)->GetCameraForward();
        //    createInfo.rotation.x = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.rotation.z = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.pickUpType = Util::PickUpTypeToString(PickUpType::AKS74U);
        //    AddPickUp(createInfo);
        //}

        // Remingtons
        //if (Input::KeyPressed(HELL_KEY_INSERT)) {
        //    PickUpCreateInfo createInfo;
        //    createInfo.position = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
        //    createInfo.position += Game::GetLocalPlayerByIndex(0)->GetCameraForward();
        //    createInfo.rotation.x = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.rotation.z = Util::RandomFloat(-HELL_PI, HELL_PI);
        //    createInfo.pickUpType = Util::PickUpTypeToString(PickUpType::REMINGTON_870);
        //    AddPickUp(createInfo);
        //}
    }

    void UpdateDoorAndWindowCubeTransforms() {
        std::vector<Transform>& transforms = GetDoorAndWindowCubeTransforms();

        transforms.clear();
        transforms.reserve(World::GetDoors().size() + GetWindows().size());

        for (Door& door : World::GetDoors()) {
            Transform& transform = transforms.emplace_back();
            transform.position = door.GetPosition();
            transform.position.y += DOOR_HEIGHT / 2;
            transform.rotation.y = door.GetRotation().y;
            transform.scale.x = 0.2f;
            transform.scale.y = DOOR_HEIGHT * 1.0f;
            transform.scale.z = 1.02f;
        }

        for (Window& window : GetWindows()) {
            float windowMidPointFromGround = 1.4f;

            Transform& transform = transforms.emplace_back();
            transform.position = window.GetPosition();
            transform.position.y += windowMidPointFromGround;
            transform.rotation.y = window.GetRotation().y;
            transform.scale.x = 0.2f;
            transform.scale.y = 1.2f;
            transform.scale.z = 0.846f;
        }
    }

    void CalculateGPULights() {
        for (int i = 0; i < GetLights().size(); i++) {
            RenderDataManager::SubmitGPULightHighRes(i);
        }
    }
}