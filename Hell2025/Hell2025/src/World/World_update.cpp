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

namespace World {

    void LazyDebugSpawns();
    void CalculateGPULights();

    // REMOVE ME!
    uint64_t g_testAnimatedGameObject = 0;
    AnimatedGameObject* GetDobermannTest() {
        return GetAnimatedGameObjectByObjectId(g_testAnimatedGameObject);
    }
    
    void Update(float deltaTime) {

        //std::cout << "Fireplace Count: " << GetFireplaces().size() << "\n";

        NavMesh::RenderDebug();

        if (Input::KeyPressed(HELL_KEY_LEFT)) {
            static MermaidCreateInfo createInfo = GetMermaids()[0].GetCreateInfo();
            createInfo.rotation.y += 0.05f;
            GetMermaids()[0].Init(createInfo, SpawnOffset());
        }

        if (Input::KeyPressed(HELL_KEY_NUMPAD_3)) {

            GetGameObjects()[0].SetPosition(Game::GetLocalPlayerByIndex(0)->GetFootPosition());
            for (Light& light : GetLights()) {
                light.ForceDirty();
            }
        }

        if (Input::KeyPressed(HELL_KEY_J)) {
            PrintObjectCounts();
        }

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

            ragdoll.Update();

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

        for (Door& door : GetDoors()) door.Update(deltaTime);
        for (GenericObject& drawers : GetGenericObjects()) drawers.Update(deltaTime); 

        std::vector<AnimatedGameObject>& animatedGameObjects = GetAnimatedGameObjects();
        std::vector<BulletCasing>& bulletCasings = GetBulletCasings();
        std::vector<Bullet>& bullets = GetBullets();
        std::vector<GameObject>& gameObjects = GetGameObjects();
        std::vector<Light>& lights = GetLights();
        std::vector<Piano>& pianos = GetPianos();
        std::vector<PickUp>& pickUps = GetPickUps();
        std::vector<Tree>& trees = GetTrees();

        for (AnimatedGameObject& animatedGameObject : animatedGameObjects) {
            animatedGameObject.Update(deltaTime);
        }

        for (BulletCasing& bulletCasing : bulletCasings) {
            bulletCasing.Update(deltaTime);
        }

        for (ChristmasLights& christmasLights : GetChristmasLights()) {
            christmasLights.Update(deltaTime);
        }

        for (ChristmasPresent& christmasPresent : GetChristmasPresents()) {
            christmasPresent.Update(deltaTime);
        }

        for (ChristmasTree& christmasTree : GetChristmasTrees()) {
            christmasTree.Update(deltaTime);
        }


        for (Dobermann& dobermann : GetDobermanns()) {
            dobermann.Update(deltaTime);
        }


        for (Fence& fence : GetFences()) {
            fence.Update();
        }

        for (GameObject& gameObject : gameObjects) {
            gameObject.Update(deltaTime);
        }

        for (HousePlane& housePlane : GetHousePlanes()) {
            //housePlane.DrawVertices(GREEN);
            //housePlane.DrawEdges(GREEN);
        }

        for (Kangaroo& kangaroo : GetKangaroos()) {
            kangaroo.Update(deltaTime);
        }

        for (Mermaid& mermaid : GetMermaids()) {
            mermaid.Update(deltaTime);
        }

        for (PickUp& pickUp : pickUps) {
            pickUp.Update(deltaTime);
        }

        for (Piano& piano : pianos) {
            piano.Update(deltaTime);
        }

        for (PowerPoleSet& powerPoleSet : GetPowerPoleSets()) {
            powerPoleSet.Update();
        }

        for (Road& road : GetRoads()) {
            road.Update();
        }
        for (Shark& shark : GetSharks()) {
            shark.Update(deltaTime);
        }

        for (Tree& tree : trees) {
            tree.Update(deltaTime);
        }

        for (Fireplace& fireplace : GetFireplaces()) {
            fireplace.Update(deltaTime);
        }

        for (TrimSet& trimSet : GetTrimSets()) {
            if (trimSet.GetType() == TrimSetType::CEILING) {
                trimSet.RenderDebug(RED);
            }
            if (trimSet.GetType() == TrimSetType::MIDDLE) {
                trimSet.RenderDebug(GREEN);
            }
            trimSet.Update();
        }

        for (Window& window : GetWindows()) {
            window.Update(deltaTime);
        }

        for (Wall& wall : GetWalls()) {
            //  wall.CreateTrims();                         // REMOVE ME FOR THE LOVE OF JEBUS!!!
            // Nothing as of yet. Probably ever.
        }

        // Update lights last. That way their dirty state reflects the state of all 
        // objects whom may have potentially moved within their radius
        for (Light& light : lights) {
            light.Update(deltaTime);
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