#include "Physics.h"
#include "Core/Game.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"

namespace Physics {

    void SubmitDebugLinesToRenderer(const DebugRenderMode& debugRenderMode) {
        PxScene* pxScene = GetPxScene();
        std::vector<PxRigidActor*> ignoreList;

        PxU32 constraintCount = pxScene->getNbConstraints();
        if (!constraintCount) return;

        std::vector<PxConstraint*> constraints(constraintCount);
        pxScene->getConstraints(constraints.data(), constraintCount);

        for (PxConstraint* constraint : constraints) {
            constraint->setFlag(PxConstraintFlag::eVISUALIZATION, true);
        }

        //pxScene->setFlag(PxSceneFlag::eENABLE_DEBUG_VISUALIZATION, true);
        pxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0.05f);
        pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
        pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

        // Ignore player ragdolls
        auto playerRagdolls = Physics::GetIgnoreList(RaycastIgnoreFlags::PLAYER_RAGDOLLS);
       // ignoreList.insert(ignoreList.end(), playerRagdolls.begin(), playerRagdolls.end());

        // Prepare
        PxU32 nbActors = pxScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
        if (nbActors) {
            std::vector<PxRigidActor*> actors(nbActors);
            pxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
            for (PxRigidActor* actor : actors) {
                PxShape* shape;
                actor->getShapes(&shape, 1);
                actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
                if (debugRenderMode == DebugRenderMode::PHYSX_RAYCAST) {
                    if (shape->getQueryFilterData().word0 == RaycastGroup::RAYCAST_DISABLED) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
                else if (debugRenderMode == DebugRenderMode::PHYSX_COLLISION) {
                    if (shape->getQueryFilterData().word1 == CollisionGroup::NO_COLLISION) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
                else if (debugRenderMode == DebugRenderMode::RAGDOLLS) {
                    PxRigidDynamic* pxRigidDynamic = static_cast<PxRigidDynamic*>(actor);
                    if (PxRigidDynamicBelongsToRagdoll(pxRigidDynamic)) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
                    }
                    else {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
                for (PxRigidActor* ignoredActor : ignoreList) {
                    if (ignoredActor == actor) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }

                //actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
            }
        }
        const PxRenderBuffer& renderBuffer = pxScene->getRenderBuffer();
        int vertexCount = renderBuffer.getNbLines() * 2;

        static std::vector<DebugVertex3D> vertices;
        vertices.clear();
        vertices.resize(vertexCount);

        for (unsigned int i = 0; i < renderBuffer.getNbLines(); i++) {
            const PxDebugLine& pxLine = renderBuffer.getLines()[i];

            glm::vec4 color;
            switch (debugRenderMode) {
                case DebugRenderMode::PHYSX_ALL:        color = GREEN;      break;
                case DebugRenderMode::PHYSX_COLLISION:  color = LIGHT_BLUE; break;
                case DebugRenderMode::RAGDOLLS:         color = LIGHT_BLUE; break;
                case DebugRenderMode::PHYSX_RAYCAST:    color = RED;        break;
                default: color = WHITE; break;
            }

            Renderer::DrawLine(Physics::PxVec3toGlmVec3(pxLine.pos0), Physics::PxVec3toGlmVec3(pxLine.pos1), color);
        }















        // Player ragdolls 
        std::vector<Viewport>& viewports = ViewportManager::GetViewports();

        for (int i = 0; i < 4; i++) {
            if (!viewports[i].IsVisible()) continue;

            // Prepare
            PxU32 nbActors = pxScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
            if (nbActors) {
                std::vector<PxRigidActor*> actors(nbActors);
                pxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

                for (PxRigidActor* actor : actors) {
                 //  actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);

                   //PhysicsUserData* userData = (PhysicsUserData*)actor->userData;
                   //if (userData && userData->objectType == ObjectType::RAGDOLL_PLAYER) {
                   //
                   //    // Does this ragdoll rigid belong to this viewports players ragdoll
                   //    bool found = false;
                   //
                   //    if (Player* player = Game::GetLocalPlayerByIndex(i)) {
                   //
                   //        Ragdoll* ragdoll = player->GetRagdoll();
                   //        if (!ragdoll) continue;
                   //
                   //        for (uint64_t rigidDynamicId : ragdoll->m_rigidDynamicIds) {
                   //            RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(rigidDynamicId);
                   //
                   //            if (rigidDynamic) {
                   //                PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();
                   //
                   //                if (pxRigidDynamic == actor) {
                   //                    actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
                   //                }
                   //            }
                   //        }
                   //    }
                   //}
                }
            }

           //const PxRenderBuffer& renderBuffer = pxScene->getRenderBuffer();
           //int vertexCount = renderBuffer.getNbLines() * 2;
           //
           //vertices.clear();
           //vertices.resize(vertexCount);
           //
           //for (unsigned int i = 0; i < renderBuffer.getNbLines(); i++) {
           //    const PxDebugLine& pxLine = renderBuffer.getLines()[i];
           //
           //    glm::vec4 color;
           //    switch (debugRenderMode) {
           //        case DebugRenderMode::PHYSX_ALL:        color = GREEN;      break;
           //        case DebugRenderMode::RAGDOLLS:         color = LIGHT_BLUE; break;
           //        default: color = WHITE; break;
           //    }
           //
           //   // Renderer::DrawLine(Physics::PxVec3toGlmVec3(pxLine.pos0), Physics::PxVec3toGlmVec3(pxLine.pos1), color, false, -1, i);
           //}
        }



    }

    std::string GetObjectCountsAsString() {
        std::string result = "PhysX Object counts\n";
        result += "- D6 Joints: " + std::to_string(GetD6JointCount()) + "\n";
        result += "- Height Fields: " + std::to_string(GetHeightFieldCount()) + "\n";
        result += "- Ragdolls: " + std::to_string(GetRagdollCount()) + "\n";
        result += "- Rigid Dynamics: " + std::to_string(GetRigidDynamicCount()) + "\n";
        result += "- Rigid Statics: " + std::to_string(GetRigidStaticCount()) + "\n";
        return result;
    }
}