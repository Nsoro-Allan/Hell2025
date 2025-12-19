#include "Physics.h"
#include "UniqueID.h"
#include "Physics/Types/RigidDynamic.h"
#include <unordered_map>
#include <vector>
#include "Util.h"
#include "Timer.hpp"

namespace Physics {

    std::unordered_map<uint64_t, RigidDynamic> g_rigidDynamics;
    std::vector<AABB> g_activeRigidDynamicAABBs;

    void UpdateActiveRigidDynamicAABBList() {
        g_activeRigidDynamicAABBs.clear();
        for (auto it = g_rigidDynamics.begin(); it != g_rigidDynamics.end(); ) {
            RigidDynamic& rigidDynamic = it->second;
            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            if (pxRigidDynamic && !pxRigidDynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
                g_activeRigidDynamicAABBs.push_back(rigidDynamic.GetAABB());
            }
            it++;
        }
    }

    void UpdateAllRigidDynamics(float deltaTime) {
        //Timer timer("UpdateAllRigidDynamics");
        //std::cout << "g_rigidDynamics.size(): " << g_rigidDynamics.size() << "\n";
        for (auto it = g_rigidDynamics.begin(); it != g_rigidDynamics.end(); ) {
            RigidDynamic& rigidDynamic = it->second;
            rigidDynamic.Update(deltaTime);
            it++;
        }
    }

    uint64_t CreateRigidDynamicFromConvexMeshVertices(Transform transform, const std::span<Vertex>& vertices, const std::span<uint32_t>& indices, float mass, PhysicsFilterData filterData, glm::vec3 initialForce, glm::vec3 initialTorque) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();
        PxMaterial* material = Physics::GetDefaultMaterial();

        float volume = Util::GetConvexHullVolume(vertices, indices);
        float density = Util::GetDensity(mass, volume);

        PxFilterData pxFilterData;
        pxFilterData.word0 = (PxU32)filterData.raycastGroup;
        pxFilterData.word1 = (PxU32)filterData.collisionGroup;
        pxFilterData.word2 = (PxU32)filterData.collidesWith;

        ///////////////////////////////////////////////////////////////////////////////////////////////
        
        // Create convex shape
        std::vector<PxVec3> pxVertices;
        for (Vertex& vertex : vertices) {
            pxVertices.push_back(Physics::GlmVec3toPxVec3(vertex.position));
        }

        PxConvexMeshDesc convexDesc;
        convexDesc.points.count = pxVertices.size();
        convexDesc.points.stride = sizeof(PxVec3);
        convexDesc.points.data = pxVertices.data();
        convexDesc.flags = PxConvexFlag::eSHIFT_VERTICES | PxConvexFlag::eCOMPUTE_CONVEX;
        //  s
        PxTolerancesScale scale;
        PxCookingParams params(scale);

        PxDefaultMemoryOutputStream buf;
        PxConvexMeshCookingResult::Enum result;
        if (!PxCookConvexMesh(params, convexDesc, buf, &result)) {
            std::cout << "some convex mesh shit failed\n";
            return 0;
        }
        PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
        PxConvexMesh* convexMesh = pxPhysics->createConvexMesh(input);
        PxConvexMeshGeometryFlags flags(~PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
        PxConvexMeshGeometry geometry(convexMesh, PxMeshScale(PxVec3(1.0f)), flags);

        PxShape* pxShape = pxPhysics->createShape(geometry, *material);
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // Create rigid dynamic
        PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
        PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
        PxRigidDynamic* pxRigidDynamic = pxPhysics->createRigidDynamic(pxTransform);
        pxRigidDynamic->attachShape(*pxShape);
        PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, density);
        pxScene->addActor(*pxRigidDynamic);

        // Apply impulse
        PxVec3 force = PxVec3(initialForce.x, initialForce.y, initialForce.z);
        pxRigidDynamic->addForce(force, PxForceMode::eIMPULSE);

        // Apply torque
        PxVec3 torque = PxVec3(initialTorque.x, initialTorque.y, initialTorque.z);
        pxRigidDynamic->addTorque(torque);

        // Create RigidDynamic
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsID];

        // Update its pointers
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.SetPxShape(pxShape);

        return physicsID;
    }

    uint64_t CreateRigidDynamicFromBoxExtents(Transform transform, glm::vec3 boxExtents, float mass, PhysicsFilterData filterData, glm::vec3 initialForce, glm::vec3 initialTorque) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();
        PxMaterial* material = Physics::GetDefaultMaterial();

        float halfWidth = boxExtents.x * 0.5f;
        float halfHeight = boxExtents.y * 0.5f;
        float halfDepth = boxExtents.z * 0.5f;
        float volume = Util::GetCubeVolume(halfWidth, halfHeight, halfDepth);
        float density = Util::GetDensity(mass, volume);

        PxFilterData pxFilterData;
        pxFilterData.word0 = (PxU32)filterData.raycastGroup;
        pxFilterData.word1 = (PxU32)filterData.collisionGroup;
        pxFilterData.word2 = (PxU32)filterData.collidesWith;

        // Create shape
        PxShape* pxShape = pxPhysics->createShape(PxBoxGeometry(halfWidth, halfHeight, halfDepth), *material, true);
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions

        // Create rigid dynamic
        PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
        PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
        PxRigidDynamic* pxRigidDynamic = pxPhysics->createRigidDynamic(pxTransform);
        pxRigidDynamic->attachShape(*pxShape);
        PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, density);
        pxScene->addActor(*pxRigidDynamic);

        // Apply impulse
        PxVec3 force = PxVec3(initialForce.x, initialForce.y, initialForce.z);
        pxRigidDynamic->addForce(force, PxForceMode::eIMPULSE);

        // Apply torque
        PxVec3 torque = PxVec3(initialTorque.x, initialTorque.y, initialTorque.z);
        pxRigidDynamic->addTorque(torque);

        // Create DynamicBox
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsID];

        // Update its pointers
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.SetPxShape(pxShape);

        return physicsID;
    }

    uint64_t CreateRigidDynamicFromBoxExtents(const Transform& transform, const glm::vec3& boxExtents, bool kinematic, float mass, PhysicsFilterData filterData, const Transform& localOffset) {
        return CreateRigidDynamicFromBoxExtents(transform, boxExtents, kinematic, mass, filterData, localOffset.to_mat4());
    }

    uint64_t CreateRigidDynamicFromBoxExtents(const Transform& transform, const glm::vec3& boxExtents, bool kinematic, float mass, PhysicsFilterData filterData, const glm::mat4& localOffset) {
        return CreateRigidDynamicFromBoxExtents(transform.to_mat4(), boxExtents, kinematic, mass, filterData, localOffset);
    }

    uint64_t CreateRigidDynamicFromBoxExtents(const glm::mat4& transform, const glm::vec3& boxExtents, bool kinematic, float mass, PhysicsFilterData filterData, const Transform& localOffset) {
        return CreateRigidDynamicFromBoxExtents(transform, boxExtents, kinematic, mass, filterData, localOffset.to_mat4());
    }

    uint64_t CreateRigidDynamicFromBoxExtents(const glm::mat4& transform, const glm::vec3& boxExtents, bool kinematic, float mass, PhysicsFilterData filterData, const glm::mat4& localOffset) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();
        PxMaterial* material = Physics::GetDefaultMaterial();

        float halfWidth = boxExtents.x * 0.5f;
        float halfHeight = boxExtents.y * 0.5f;
        float halfDepth = boxExtents.z * 0.5f;

        PxFilterData pxFilterData;
        pxFilterData.word0 = (PxU32)filterData.raycastGroup;
        pxFilterData.word1 = (PxU32)filterData.collisionGroup;
        pxFilterData.word2 = (PxU32)filterData.collidesWith;

        // Create shape
        PxShape* pxShape = pxPhysics->createShape(PxBoxGeometry(halfWidth, halfHeight, halfDepth), *material, true);
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions
        pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        pxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

        PxTransform localOffsetTransform = PxTransform(GlmMat4ToPxMat44(localOffset));
        pxShape->setLocalPose(localOffsetTransform);

        // Create rigid dynamic
        PxTransform pxTransform = PxTransform(Physics::GlmMat4ToPxMat44(transform));
        PxRigidDynamic* pxRigidDynamic = pxPhysics->createRigidDynamic(pxTransform);
        pxRigidDynamic->attachShape(*pxShape);
        pxScene->addActor(*pxRigidDynamic);

        // Kinematic flag
        pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

        // Mass stuff
        float volume = Util::GetCubeVolume(halfWidth, halfHeight, halfDepth);
        float density = Util::GetDensity(mass, volume);
        PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, density);

        // Create DynamicBox
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsID];

        // Update its pointers
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.SetPxShape(pxShape);

        return physicsID;
    }

    uint64_t CreateRigidDynamicFromPxShape(PxShape* pxShape, glm::mat4 initialPose, glm::mat4 shapeOffsetMatrix) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();

        // Create rigid dynamic
        PxTransform pxTransform = PxTransform(GlmMat4ToPxMat44(initialPose));

        PxRigidDynamic* pxRigidDynamic = pxPhysics->createRigidDynamic(pxTransform);
        pxRigidDynamic->attachShape(*pxShape);
        pxScene->addActor(*pxRigidDynamic);

        // Create RigidDynamic
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsID];

        PxMat44 localShapeMatrix = GlmMat4ToPxMat44(shapeOffsetMatrix);
        PxTransform localShapeTransform(localShapeMatrix);
        pxShape->setLocalPose(localShapeTransform);

        // Update its pointers
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.SetPxShape(pxShape);
/*
        std::cout << " - created " << Physics::GetPxShapeTypeAsString(pxShape) << "\n";

        if (pxShape == nullptr) {
            std::cerr << "ERROR: PxPhysics::createShape returned NULL!" << std::endl;
            // The shape was not created by PhysX, possibly due to invalid geometry params
            // or other issues that didn't trigger the error callback but resulted in failure.
        }



        if (pxShape) {
            physx::PxTransform localPose = pxShape->getLocalPose();
            if (!localPose.isValid()) { // Checks for finite q and p, and normalized q
                std::cerr << "WARNING: Shape " << pxShape << " has an invalid local pose!" << std::endl;
                std::cerr << "  Pose P: (" << localPose.p.x << "," << localPose.p.y << "," << localPose.p.z << ")" << std::endl;
                std::cerr << "  Pose Q: (" << localPose.q.x << "," << localPose.q.y << "," << localPose.q.z << "," << localPose.q.w << ")" << std::endl;
            }
        }
        physx::PxRigidActor* actor = nullptr;
        if (pxShape) {
            actor = pxShape->getActor();
            if (actor == nullptr) {
                std::cout << "INFO: Shape " << pxShape << " is not attached to any PxRigidActor." << std::endl;
                // This means it cannot be in a scene.
                // This might happen if you create a shape but haven't attached it to an actor yet,
                // or if the actor it was attached to has been released and the shape detached.
            }
        }
        else {
         // Shape itself is null, so it can't be in a scene.
        }
        if (actor) {
            physx::PxScene* scenesActorIsIn = actor->getScene();
            if (scenesActorIsIn != nullptr) {
                // The actor (and thus its shape) is in a scene.
                // You can compare it to your expected scene instance:
                if (scenesActorIsIn == pxScene) { // gMyExpectedPxScene is your PxScene*
                    std::cout << "INFO: Shape " << pxShape << " (actor " << actor << ") IS in the expected scene." << std::endl;
                }
                else {
                    std::cerr << "WARNING: Shape " << pxShape << " (actor " << actor << ") IS in a scene, but NOT the expected one. Scene ptr: " << scenesActorIsIn << std::endl;
                }
            }
            else {
                std::cout << "INFO: Shape " << pxShape << "'s actor (" << actor << ") is NOT currently in any PxScene." << std::endl;
                // This means pxScene->addActor(*actor) was either not called, failed implicitly,
                // or pxScene->removeActor(*actor) was called.
            }
        }

        if (pxShape) { // Ensure pxShape is not null first
            physx::PxGeometryHolder geomHolder = pxShape->getGeometry(); // Get the geometry holder
            physx::PxGeometryType::Enum geomType = geomHolder.getType(); // Get the actual type

            // You can specifically exclude types you don't want to check, like eHEIGHTFIELD
            if (geomType == physx::PxGeometryType::eHEIGHTFIELD) {
                // std::cout << "INFO: Shape " << pxShape << " is a HeightField, skipping detailed geometry param check." << std::endl;
            }
            else {
             // Proceed with checks for other types
                switch (geomType) {
                    case physx::PxGeometryType::eSPHERE: {
                        const physx::PxSphereGeometry& sphereGeom = geomHolder.sphere(); // Access the sphere geometry
                        if (sphereGeom.radius <= 0.0f || !physx::PxIsFinite(sphereGeom.radius)) {
                            std::cerr << "WARNING: Shape " << pxShape << " (Sphere) has invalid radius: " << sphereGeom.radius << std::endl;
                        }
                        break;
                    }
                    case physx::PxGeometryType::eCAPSULE: {
                        const physx::PxCapsuleGeometry& capsuleGeom = geomHolder.capsule(); // Access the capsule geometry
                        if (capsuleGeom.radius <= 0.0f || !physx::PxIsFinite(capsuleGeom.radius) ||
                            capsuleGeom.halfHeight <= 0.0f || !physx::PxIsFinite(capsuleGeom.halfHeight)) {
                            std::cerr << "WARNING: Shape " << pxShape << " (Capsule) has invalid dimensions. Radius: " << capsuleGeom.radius << ", HalfHeight: " << capsuleGeom.halfHeight << std::endl;
                        }
                        break;
                    }
                    case physx::PxGeometryType::eBOX: {
                        const physx::PxBoxGeometry& boxGeom = geomHolder.box(); // Access the box geometry
                        if (boxGeom.halfExtents.x <= 0.0f || !physx::PxIsFinite(boxGeom.halfExtents.x) ||
                            boxGeom.halfExtents.y <= 0.0f || !physx::PxIsFinite(boxGeom.halfExtents.y) ||
                            boxGeom.halfExtents.z <= 0.0f || !physx::PxIsFinite(boxGeom.halfExtents.z)) {
                            std::cerr << "WARNING: Shape " << pxShape << " (Box) has invalid extents: (" << boxGeom.halfExtents.x << ", " << boxGeom.halfExtents.y << ", " << boxGeom.halfExtents.z << ")" << std::endl;
                        }
                        break;
                    }
                    case physx::PxGeometryType::eCONVEXMESH:
                        // For PxConvexMeshGeometry, you might check if its PxConvexMesh* is null
                        // or if its scale is valid. The mesh itself is created separately.
                        // const physx::PxConvexMeshGeometry& convexGeom = geomHolder.convexMesh();
                        // if (convexGeom.convexMesh == nullptr) std::cerr << "WARNING: Shape " << pxShape << " (ConvexMesh) has null mesh pointer." << std::endl;
                        // if (!convexGeom.scale.isValid() || !convexGeom.scale.isFinite())  std::cerr << "WARNING: Shape " << pxShape << " (ConvexMesh) has invalid scale." << std::endl;
                    break;
                    case physx::PxGeometryType::eTRIANGLEMESH:
                        // Similar checks for PxTriangleMeshGeometry
                        // const physx::PxTriangleMeshGeometry& triGeom = geomHolder.triangleMesh();
                        // if (triGeom.triangleMesh == nullptr) std::cerr << "WARNING: Shape " << pxShape << " (TriangleMesh) has null mesh pointer." << std::endl;
                        // if (!triGeom.scale.isValid() || !triGeom.scale.isFinite())  std::cerr << "WARNING: Shape " << pxShape << " (TriangleMesh) has invalid scale." << std::endl;
                    break;
                    case physx::PxGeometryType::ePLANE:
                        // PxPlaneGeometry doesn't have dimensions like radius/extents to validate in this context,
                        // it's defined by a normal and distance, implicitly infinite.
                    break;
                    default:
                    std::cout << "INFO: Shape " << pxShape << " has unhandled geometry type " << geomType << std::endl;
                    break;
                }
            }
        }*/

        return physicsID;
    }
       
    bool RigidDynamicExists(uint64_t rigidDynamicId) {
        return (g_rigidDynamics.find(rigidDynamicId) != g_rigidDynamics.end());
    }

    bool RigidDynamicIsKinematic(uint64_t rigidDynamicId) {
        if (!RigidDynamicExists(rigidDynamicId)) return false;

        RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
        PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();

        if (!pxRigidDynamic) return false;
        return pxRigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
    }

    glm::mat4 GetRigidDynamicWorldMatrix(uint64_t rigidDynamicId) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            if (pxRigidDynamic) {
                return Physics::PxMat44ToGlmMat4(pxRigidDynamic->getGlobalPose());
            }
        }
        return glm::mat4(1);
    }

    void MarkRigidDynamicForRemoval(uint64_t rigidDynamicId) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            rigidDynamic.MarkForRemoval();
        }
    }

    void AddFoceToRigidDynamic(uint64_t rigidDynamicId, glm::vec3 force) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            PxVec3 pxForce = Physics::GlmVec3toPxVec3(force);
            pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
            pxRigidDynamic->addForce(pxForce);
        }
    }

    void SetRigidDynamicGlobalPose(uint64_t rigidDynamicId, glm::mat4 globalPoseMatrix) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            PxMat44 pxMatrix = GlmMat4ToPxMat44(globalPoseMatrix);
            PxTransform pxTransform = PxTransform(pxMatrix);
            pxRigidDynamic->setGlobalPose(pxTransform);
        }
        else {
            std::cout << "Phyics::SetRigidDynamicGlobalPose() failed to set global pose: id " << rigidDynamicId << " not found\n";
        }
    }

    //void RemoveRigidDynamic(uint64_t rigidDynamicId) {
    //    if (RigidDynamicExists(rigidDynamicId)) {
    //        PxPhysics* pxPhysics = Physics::GetPxPhysics();
    //        PxScene* pxScene = Physics::GetPxScene();
    //        RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
    //        PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
    //        PxShape* pxShape = rigidDynamic.GetPxShape();
    //
    //        // Remove rigid
    //        if (pxRigidDynamic) {
    //
    //            // Clean up its user data
    //            if (pxRigidDynamic->userData) {
    //                delete static_cast<PhysicsUserData*>(pxRigidDynamic->userData);
    //                pxRigidDynamic->userData = nullptr;
    //            }
    //            // Remove it from PxScene
    //            if (pxRigidDynamic->getScene() != nullptr) {
    //                pxScene->removeActor(*pxRigidDynamic);
    //            }
    //            // Release it
    //            pxRigidDynamic->release();
    //            pxRigidDynamic = nullptr;
    //        }
    //
    //        // Remove shape
    //        if (pxShape) {
    //            pxShape->release();
    //            pxShape = nullptr;
    //        }
    //
    //        // Remove from container
    //        g_rigidDynamics.erase(rigidDynamicId);
    //    }
    //}

    void RemoveAnyRigidDynamicMarkedForRemovalOLD() {
        PxScene* pxScene = Physics::GetPxScene();

        for (auto it = g_rigidDynamics.begin(); it != g_rigidDynamics.end(); ) {
            RigidDynamic& rigidDynamic = it->second;
            if (rigidDynamic.IsMarkedForRemoval()) {
                // Retrieve pointers
                PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
                PxShape* pxShape = rigidDynamic.GetPxShape();

                // Remove the actor from the scene if active
                if (pxRigidDynamic && rigidDynamic.HasActivePhysics()) {
                    if (pxRigidDynamic->getScene() != nullptr) {
                        pxScene->removeActor(*pxRigidDynamic);
                    }
                }

                // Release the shape
                if (pxShape) {
                    pxShape->release();
                }

                // Release the actor
                if (pxRigidDynamic) {
                    pxRigidDynamic->release();
                }

                // Remove from container
                it = g_rigidDynamics.erase(it);
            }
            else {
                ++it;
            }
        }
    }


    // Safer the than above.. maybe

    void RemoveAnyRigidDynamicMarkedForRemoval() {
        for (auto it = g_rigidDynamics.begin(); it != g_rigidDynamics.end(); ) {
            RigidDynamic& rigidDynamic = it->second;

            if (!rigidDynamic.IsMarkedForRemoval()) { ++it; continue; }

            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            PxShape* pxShape = rigidDynamic.GetPxShape();

            if (pxRigidDynamic && pxRigidDynamic->getScene()) {
                pxRigidDynamic->getScene()->removeActor(*pxRigidDynamic);
            }

            if (pxShape) {
                pxShape->release();
                rigidDynamic.SetPxShape(nullptr);
            }

            if (pxRigidDynamic) {
                rigidDynamic.SetPxRigidDynamic(nullptr);
                pxRigidDynamic->release();
            }

            it = g_rigidDynamics.erase(it);
        }
    }

    void ActivateRigidDynamicPhysics(uint64_t rigidDynamicId) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            rigidDynamic.ActivatePhsyics();
        }
    }

    void DeactivateRigidDynamicPhysics(uint64_t rigidDynamicId) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            rigidDynamic.DeactivatePhysics();
        }
    }    

    void SetRigidDynamicUserData(uint64_t rigidDynamicId, PhysicsUserData physicsUserData) {
        if (RigidDynamicExists(rigidDynamicId)) {
            RigidDynamic& rigidDynamic = g_rigidDynamics[rigidDynamicId];
            PxRigidDynamic* pxRigidDynamic = rigidDynamic.GetPxRigidDynamic();
            pxRigidDynamic->userData = new PhysicsUserData(physicsUserData);
        }
    }

    const std::vector<AABB>& GetActiveRididDynamicAABBs() {
        return g_activeRigidDynamicAABBs;
    }

    RigidDynamic* GetRigidDynamicById(uint64_t rigidDynamicId) {
        if (RigidDynamicExists(rigidDynamicId)) {
            return &g_rigidDynamics[rigidDynamicId];
        }
        else {
            return nullptr;
        }
    }

    int GetRigidDynamicCount() {
        return g_rigidDynamics.size();
    }

    void PrintSceneRigidInfo() {
        std::cout << " Rigid Dynamics\n\n";

        for (auto it = g_rigidDynamics.begin(); it != g_rigidDynamics.end(); ) {
            uint64_t id = it->first;
            RigidDynamic& rigidDynamic = it->second;

            std::cout << " " << id << " - ";
            std::cout << "[" << GetPxShapeTypeAsString(rigidDynamic.GetPxShape()) << "] ";
            //std::cout << "position: " << rigidDynamic.GetCurrentPosition() << " ";
            std::cout << "\n";

            it++;
        }
    }
}