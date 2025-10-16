#include "Physics.h"
#include "Physics/Types/CharacterController.h"
#include <unordered_map>
#include <vector>
#include "Util.h"
#include "UniqueID.h"
#include "Input/Input.h"

namespace Physics {
    std::unordered_map<uint64_t, CharacterController> g_characterControllers;

    uint64_t CreateCharacterController(uint64_t parentObjectId, glm::vec3 position, float height, float radius, PhysicsFilterData physicsFilterData) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();

        PxMaterial* material = Physics::GetDefaultMaterial();
        PxCapsuleControllerDesc* desc = new PxCapsuleControllerDesc;
        desc->setToDefault();
        desc->height = height;
        desc->radius = radius;
        desc->position = PxExtendedVec3(position.x, position.y + (height / 2) + (radius * 2), position.z);
        desc->material = material;
        desc->stepOffset = 0.25f;
        desc->contactOffset = 0.001;
        desc->scaleCoeff = .99f;
        desc->reportCallback = &Physics::GetCharacterControllerHitCallback();
        desc->slopeLimit = cosf(glm::radians(85.0f));

        PxController* pxController = Physics::GetCharacterControllerManager()->createController(*desc);

        PxShape* shape;
        pxController->getActor()->getShapes(&shape, 1);

        PxFilterData filterData;
        filterData.word0 = (PxU32)physicsFilterData.raycastGroup;
        filterData.word1 = (PxU32)physicsFilterData.collisionGroup;
        filterData.word2 = (PxU32)physicsFilterData.collidesWith;

        PhysicsUserData physicsUserData;
        physicsUserData.objectId = parentObjectId;
        physicsUserData.objectType = ObjectType::CHARACTER_CONTROLLER;
        physicsUserData.physicsId = 0;
        physicsUserData.physicsType = PhysicsType::CHARACTER_CONTROLLER;
        pxController->getActor()->userData = new PhysicsUserData(physicsUserData);

        // Create CharacterController
        uint64_t physicsID = UniqueID::GetNextObjectId(ObjectType::PHYSICS_RIGID_CHARACTER_CONTROLLER);
        CharacterController& characterController = g_characterControllers[physicsID];

        // Update its pointers
        characterController.SetPxController(pxController);

        return physicsID;
    }

    CharacterController* GetCharacterControllerById(uint64_t characterControllerId) {
        if (CharacterControllerExists(characterControllerId)) {
            return &g_characterControllers[characterControllerId];
        }
        else {
            return nullptr;
        }
    }

    void RemoveAnyCharacterControllerMarkedForRemoval() {
        PxScene* pxScene = Physics::GetPxScene();

        for (auto it = g_characterControllers.begin(); it != g_characterControllers.end(); ) {
            uint64_t characterControllerId = it->first;
            CharacterController& characterController = it->second;
            if (characterController.IsMarkedForRemoval()) {

                // Retrieve pointers
                PxController* pxController = characterController.GetPxController();
                pxController->release();
                pxController = nullptr;

                // Remove from container
                it = g_characterControllers.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void MarkCharacterControllerForRemoval(uint64_t characterControllerId) {
        if (CharacterControllerExists(characterControllerId)) {
            CharacterController& CharacterController = g_characterControllers[characterControllerId];
            CharacterController.MarkForRemoval();
            return;
        }
    }

    bool CharacterControllerExists(uint64_t characterControllerId) {
        return (g_characterControllers.find(characterControllerId) != g_characterControllers.end());

    }

    int GetCharacterControllerCount() {
        return g_characterControllers.size();
    }

    AABB GetCharacterControllerAABB(uint64_t characterControllerId) {
        if (CharacterControllerExists(characterControllerId)) {
            CharacterController& characterController = g_characterControllers[characterControllerId];

            PxController* pxController = characterController.GetPxController();

            if (pxController) {
                PxRigidDynamic* actor = pxController->getActor();
                PxBounds3 bounds = actor->getWorldBounds();
                glm::vec3 aabbMin = glm::vec3(bounds.minimum.x, bounds.minimum.y, bounds.minimum.z);
                glm::vec3 aabbMax = glm::vec3(bounds.maximum.x, bounds.maximum.y, bounds.maximum.z);
                return AABB(aabbMin, aabbMax);
            }
        }

        std::cout << "Physics::GetCharacterControllerAABB() failed: " << characterControllerId << " not found, or it had an invalid PxController pointer\n";
        return AABB();
    }

    void MoveCharacterController(uint64_t characterControllerId, glm::vec3 displacement) {
        if (!CharacterControllerExists(characterControllerId)) return;
       
        CharacterController& characterController = g_characterControllers[characterControllerId];
        characterController.Move(displacement); 
    }

    glm::vec3 GetCharacterControllerPosition(uint64_t characterControllerId) {
        if (!CharacterControllerExists(characterControllerId)) {
            std::cout << "Physics::GetCharacterControlPosition() failed: " << characterControllerId << " not found! WARNING VEC3(0.0f) returned\n";
            return glm::vec3(0.0f);
        }
        
        CharacterController& characterController = g_characterControllers[characterControllerId];
        PxController* pxController = characterController.GetPxController();

        if (!pxController) {
            std::cout << "Physics::GetCharacterControlPosition() failed: pxController was nullptr\n";
            return glm::vec3(0.0f);
        }

        return Physics::PxVec3toGlmVec3(pxController->getFootPosition());
    }

    const std::unordered_map<uint64_t, CharacterController>& GetCharacterControllers() {
        return g_characterControllers;
    }
}