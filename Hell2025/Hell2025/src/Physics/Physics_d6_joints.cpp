#include "Physics.h"
#include "Physics/Types/D6Joint.h"
#include <unordered_map>
#include <vector>
#include "Util.h"
#include "UniqueID.h"
#include "Input/Input.h"

namespace Physics {
    std::unordered_map<uint64_t, D6Joint> g_d6Joints;

    uint64_t CreateD6Joint(uint64_t parentRigidDynamicId, uint64_t childRigidDynamicId, glm::mat4 parentFrame, glm::mat4 childFrame) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();

        RigidDynamic* parentRigidDynamic = GetRigidDynamicById(parentRigidDynamicId);
        RigidDynamic* childRigidDynamic = GetRigidDynamicById(childRigidDynamicId);

        if (!parentRigidDynamic || !childRigidDynamic) {
            std::cout << "Physics::CreateD6Joint() failed to retrieve parent or child from rigid dynamic ids\n";
            return 0;
        }

        PxTransform pxParentFrame = PxTransform(Physics::GlmMat4ToPxMat44(parentFrame));
        PxTransform pxChildFrame = PxTransform(Physics::GlmMat4ToPxMat44(childFrame));

        PxD6Joint* pxD6joint = PxD6JointCreate(*pxPhysics, parentRigidDynamic->GetPxRigidDynamic(), pxParentFrame, childRigidDynamic->GetPxRigidDynamic(), pxChildFrame);
        
        // Do you really want this?
        pxD6joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
        pxD6joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, false);

        // Create D6Joint
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        D6Joint& d6Joint = g_d6Joints[physicsID];

        // Update its pointers
        d6Joint.SetPxD6Joint(pxD6joint);

        return physicsID;
    }

    D6Joint* GetD6JointById(uint64_t d6JointId) {
        if (D6JointExists(d6JointId)) {
            return &g_d6Joints[d6JointId];
        }
        else {
            return nullptr;
        }
    }

    void RemoveAnyD6JointMarkedForRemoval() {
        PxScene* pxScene = Physics::GetPxScene();

        for (auto it = g_d6Joints.begin(); it != g_d6Joints.end(); ) {
            uint64_t d6JointId = it->first;
            D6Joint& d6Joint = it->second;
            if (d6Joint.IsMarkedForRemoval()) {

                // Retrieve pointers
                PxD6Joint* pxD6joint = d6Joint.GetPxD6Joint(); 
                pxD6joint->release();
                pxD6joint = nullptr;

                // Remove from container
                it = g_d6Joints.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    
    void MarkD6JointForRemoval(uint64_t d6JointId) {
        if (D6JointExists(d6JointId)) {
            D6Joint& d6Joint = g_d6Joints[d6JointId];
            d6Joint.MarkForRemoval();
            return;
        }
    }

    bool D6JointExists(uint64_t d6JointId) {
        return (g_d6Joints.find(d6JointId) != g_d6Joints.end());

    }

    int GetD6JointCount() {
        return g_d6Joints.size();
    }

    void PrintSceneD6JointInfo() {

    }
}