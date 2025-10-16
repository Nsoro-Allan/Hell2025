#include "RagdollV2.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "RagdollManager.h"
#include "Ragdoll_util.h"
#include "HellLogging.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"

inline PxTransform PxTransformFromRest(const RdMatrix& restM, float sceneScale) {
    PxMat44 M = RdMatrixToPxMat44(restM);

    // Extract basis columns + translation from PxMat44
    PxVec3 x(M.column0.x, M.column0.y, M.column0.z);
    PxVec3 y(M.column1.x, M.column1.y, M.column1.z);
    PxVec3 z(M.column2.x, M.column2.y, M.column2.z);
    PxVec3 t(M.column3.x, M.column3.y, M.column3.z);

    // Descaling / orthonormalization
    x = x.getNormalized();
    y = (y - x * x.dot(y)).getNormalized();
    z = x.cross(y);

    // Ensure right handed basis (no mirroring)
    if (x.cross(y).dot(z) < 0.0f) z = -z;

    // Build rotation from the orthonormal columns
    PxMat33 R(x, y, z);          // PxMat33 takes columns
    PxQuat  q(R);                // quaternion from 3x3

    // Scale TRANSLATION only
    PxVec3 p = t * sceneScale;

    return PxTransform(p, q);
}

void RagdollV2::Init(glm::vec3 spawnPosition, glm::vec3 spawnEulerRotation, const std::string& ragdollName, uint64_t ragdollId) {
    RagdollInfo* ragdollInfo = RagdollManager::GetRagdollInfoByName(ragdollName);
    if (!ragdollInfo) return;

    RagdollSolver& solver = ragdollInfo->m_solver;

    m_ragdollId = ragdollId;
    m_scale = solver.sceneScale;
    m_ragdollName = ragdollName;
    m_spawnTransform.position = spawnPosition;
    m_spawnTransform.rotation = spawnEulerRotation;
    m_meshBuffer.Reset();

    CleanUp();

    PxTransform rootPose(Physics::GlmMat4ToPxMat44(m_spawnTransform.to_mat4()));

    PxPhysics* physics = Physics::GetPxPhysics();
    PxScene* scene = Physics::GetPxScene();

    for (RagdollMarker& marker : ragdollInfo->m_markers) {
        // Store color for rendering
        glm::vec3 color = glm::vec3(marker.color.x(), marker.color.g(), marker.color.b());
        m_markerColors.push_back(color);

        // Store bone name for skinning
        m_markerBoneNames.push_back(marker.boneName);

        // Create mesh for rendering from shape
        AddMarkerMeshData(marker, ragdollInfo->m_solver);

        PxTransform restTransform = PxTransformFromRest(marker.originMatrix, m_scale);
        PxRigidDynamic* pxrigid = physics->createRigidDynamic(rootPose.transform(restTransform));

        // Kinematic/dynamic/inherit
        const bool kinematic = (marker.inputType == (RdEnum)RdBehaviour::kKinematic);
        pxrigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

        PxShape* shape = RagdollUtil::CreateShape(marker, ragdollInfo->m_solver);

        if (shape) {

            PhysicsFilterData filterData;
            filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
            filterData.collisionGroup = CollisionGroup::RAGDOLL_ENEMY;
            filterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | CHARACTER_CONTROLLER | RAGDOLL_ENEMY);

            PxFilterData pxFilterData;
            pxFilterData.word0 = (PxU32)filterData.raycastGroup;
            pxFilterData.word1 = (PxU32)filterData.collisionGroup;
            pxFilterData.word2 = (PxU32)filterData.collidesWith;
            shape->setQueryFilterData(pxFilterData);       // ray casts
            shape->setSimulationFilterData(pxFilterData);  // collisions

            pxrigid->attachShape(*shape);
            shape->release();

            // Kinematic/dynamic/inherit
            const bool kinematic = (marker.inputType == (RdEnum)RdBehaviour::kKinematic);
            pxrigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

            // Mass/inertia
            if (!kinematic) {
                const float adjustedMass = marker.mass * 100.0f; // HACK!
                PxRigidBodyExt::setMassAndUpdateInertia(*pxrigid, PxReal(adjustedMass));
            }

            if (marker.enableCCD) {
                pxrigid->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
            }

            pxrigid->setLinearDamping((float)marker.linearDamping);
            pxrigid->setAngularDamping((float)marker.angularDamping);
            pxrigid->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_GYROSCOPIC_FORCES, true);
            pxrigid->setSleepThreshold(0.005f);
            pxrigid->setStabilizationThreshold(0.01f);

            scene->addActor(*pxrigid);
            m_pxRigidDynamics.emplace_back(pxrigid);

            // From Ragdoll below
            //pxrigid->setSolverIterationCounts(
            //    std::min(255U, solver.positionIterations * solver.positionIterations), 
            //    std::min(255U, solver.velocityIterations * solver.velocityIterations)
            //);

            //if (marker.maxContactImpulse > 0) {
            //    pxrigid->setMaxContactImpulse(marker.maxContactImpulse);
            //}
            //else {
            //    pxrigid->setMaxContactImpulse(PX_MAX_F32);
            //}
            //
            //if (marker.maxDepenetrationVelocity > 0) {
            //    pxrigid->setMaxDepenetrationVelocity(marker.maxDepenetrationVelocity);
            //}
            //else {
            //    pxrigid->setMaxDepenetrationVelocity(PX_MAX_F32);
            //}

            //PxRigidBodyExt::updateMassAndInertia(*pxrigid, marker.mass);

            //float wakeCounter{ FLT_MAX };
            //pxrigid->setSleepThreshold(marker.sleepThreshold);

            //if (marker.densityCustom <= 0 && RdRegistry.all_of<RdMarkerUIComponent>(referenceEntity)) {
            //    const auto& markerUi = RdRegistry.get<RdMarkerUIComponent>(referenceEntity);
            //    PxRigidBodyExt::setMassAndUpdateInertia(*pxrigid, markerUi.mass);
            //}
            //else {
            //    PxRigidBodyExt::updateMassAndInertia(*pxrigid, marker.densityCustom);
            //}

            //if (rigid.wakeCounter > 1) {
            //    const auto time = RdRegistry.get<RdTimeComponent>(sceneEntity);
            //    wakeCounter = time.fixedTimestep
            //        * solver.timeMultiplier
            //
            //        // Account for first frame where the rigid is created
            //        * (rigid.wakeCounter - 1);
            //}
            //
            //pxrigid->setWakeCounter(wakeCounter);
            //pxrigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigid.kinematic);

            // User data
            PhysicsUserData physicsUserData;
            physicsUserData.physicsType = PhysicsType::RIGID_DYNAMIC;
            physicsUserData.objectType = ObjectType::RAGDOLL_V2;
            physicsUserData.physicsId = UniqueID::GetNextObjectId(ObjectType::RAGDOLL_V2);
            physicsUserData.objectId = m_ragdollId;
            pxrigid->userData = new PhysicsUserData(physicsUserData);
        }
    }

    m_meshBuffer.UpdateBuffers();

    std::unordered_map<std::string, PxRigidDynamic*> actorByMarker;
    for (int i = 0; i < ragdollInfo->m_markers.size(); i++) {
        actorByMarker[ragdollInfo->m_markers[i].name] = m_pxRigidDynamics[i];
    }

    const float sceneScale = (float)ragdollInfo->m_solver.sceneScale;

    // Pull per joint limit springs
    float linK = (float)ragdollInfo->m_solver.linearLimitStiffness;
    float linC = (float)ragdollInfo->m_solver.linearLimitDamping;
    float angK = (float)ragdollInfo->m_solver.angularLimitStiffness;
    float angC = (float)ragdollInfo->m_solver.angularLimitDamping;

    // Apply legacy compatibility 10000x scaling with overflow guards
    const float MAXV = std::numeric_limits<float>::max();
    if (linK < MAXV / 1000.0f) linK *= 1000.0f;
    if (linC < MAXV / 1000.0f) linC *= 1000.0f;
    if (angK < MAXV / 1000.0f) angK *= 1000.0f;
    if (angC < MAXV / 1000.0f) angC *= 1000.0f;

    // Now build springs 
    const PxSpring linearSpring(linK, linC);
    const PxSpring angularSpring(angK, angC);

    #define LOCK_NEGATIVE_LINEAR 1

    auto setLinearAxis = [&](PxD6Joint* d6, PxD6Axis::Enum axis, float lim) {
        #if LOCK_NEGATIVE_LINEAR
        if (lim > 0.0f) {
            d6->setMotion(axis, PxD6Motion::eLIMITED);
            d6->setLinearLimit(axis, PxJointLinearLimitPair(-lim, lim, linearSpring));
        }
        else if (lim < 0.0f) {
            d6->setMotion(axis, PxD6Motion::eLOCKED);
        }
        #else
        if (lim > 0.0f) {
            d6->setMotion(axis, PxD6Motion::eLIMITED);
            d6->setLinearLimit(axis, PxJointLinearLimitPair(-lim, lim, linearSpring));
        }
        else if (lim == 0.0f) {
            d6->setMotion(axis, PxD6Motion::eLOCKED);
        }
        else {
            d6->setMotion(axis, PxD6Motion::eFREE);
        }
        #endif
    };

    for (RagdollJoint& j : ragdollInfo->m_joints)
    {
        auto itP = actorByMarker.find(j.parentName);
        auto itC = actorByMarker.find(j.childName);
        if (itP == actorByMarker.end() || itC == actorByMarker.end()) {
            Logging::Warning() << "[D6] Missing actors for joint " << j.name;
            continue;
        }

        PxRigidActor* parent = itP->second;
        PxRigidActor* child = itC->second;

        // parent/child local frames from JSON, scale translation only
        PxTransform lp(Physics::GlmMat4ToPxMat44(RdMatrixToGlmMat4(j.parentFrame)));
        PxTransform lc(Physics::GlmMat4ToPxMat44(RdMatrixToGlmMat4(j.childFrame)));
        if (sceneScale != 1.0f) { lp.p *= sceneScale; lc.p *= sceneScale; }

        PxD6Joint* d6 = PxD6JointCreate(*physics, parent, lp, child, lc);
        if (!d6) { Logging::Error() << "[D6] Create failed for " << j.name; continue; }

        // Start FREE on all axes
        d6->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
        d6->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
        d6->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
        d6->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
        d6->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
        d6->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

        // Linear limits
        setLinearAxis(d6, PxD6Axis::eX, (float)j.limitLinear.x());
        setLinearAxis(d6, PxD6Axis::eY, (float)j.limitLinear.y());
        setLinearAxis(d6, PxD6Axis::eZ, (float)j.limitLinear.z());

        // Angular limits
        const float twist = (float)j.limitRange.x(); // radians
        const float swing1 = (float)j.limitRange.y();
        const float swing2 = (float)j.limitRange.z();

        if (twist > 0.0f) {
            d6->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
            d6->setTwistLimit(PxJointAngularLimitPair(-twist, twist, angularSpring));
        }
        else if (twist < 0.0f) {
            d6->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
        }

        if (swing1 > 0.0f && swing2 > 0.0f) {
            d6->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
            d6->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
            d6->setSwingLimit(PxJointLimitCone(swing1, swing2, angularSpring));
        }
        else {
            if (swing1 < 0.0f) d6->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
            if (swing2 < 0.0f) d6->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);
            // don't set swing limit
        }

        itP->second->setSolverIterationCounts(12, 4);
        itC->second->setSolverIterationCounts(12, 4);

        m_pxD6Joints.push_back(d6);
    }

    DisableSimulation();
}

void RagdollV2::Update() {
    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        PxTransform pxTransform = pxRigidDynamic->getGlobalPose();
        PxMat44 pxMatrix(pxTransform);
        glm::mat4 matrix = Physics::PxMat44ToGlmMat4(pxMatrix);
    }
}

void RagdollV2::AddForce(uint64_t physicsId, glm::vec3 force) {
    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        PhysicsUserData* physicsUserData = static_cast<PhysicsUserData*>(pxRigidDynamic->userData);
        if (!physicsUserData) continue;

        if (physicsUserData->physicsId == physicsId) {
            if (!m_simulationEnabled) {
                Audio::PlayAudio("Death0.wav", 1.0f);
            }

            EnableSimulation();
            pxRigidDynamic->addForce(PxVec3(force.x, force.y, force.z), PxForceMode::eFORCE, true);

            return;
        }
    }
}

void RagdollV2::DisableSimulation() {
    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        if (!pxRigidDynamic) continue;
        pxRigidDynamic->setLinearVelocity(PxVec3(0.0f));
        pxRigidDynamic->setAngularVelocity(PxVec3(0.0f));
        pxRigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
    }
    m_simulationEnabled = false;
}

void RagdollV2::EnableSimulation() {
    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        if (!pxRigidDynamic) continue;

        pxRigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
        pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
        pxRigidDynamic->wakeUp();
    }
    m_simulationEnabled = true;
}

void RagdollV2::SetToInitialPose() {
    RagdollInfo* ragdollInfo = RagdollManager::GetRagdollInfoByName(m_ragdollName);
    if (!ragdollInfo) return;

    const PxTransform spawnTransform(Physics::GlmMat4ToPxMat44(m_spawnTransform.to_mat4()));
    PxTransform rootPose(Physics::GlmMat4ToPxMat44(m_spawnTransform.to_mat4()));

    const size_t count = std::min(m_pxRigidDynamics.size(), ragdollInfo->m_markers.size());
    for (size_t i = 0; i < count; ++i) {
        const RagdollMarker& marker = ragdollInfo->m_markers[i];
        PxTransform restTransform = PxTransformFromRest(marker.originMatrix, m_scale);
        m_pxRigidDynamics[i]->setGlobalPose(rootPose.transform(restTransform));
        m_pxRigidDynamics[i]->setLinearVelocity(PxVec3(0));
        m_pxRigidDynamics[i]->setAngularVelocity(PxVec3(0));
    }
}

void RagdollV2::CleanUp() {
    PxPhysics* pxPhysics = Physics::GetPxPhysics();
    PxScene* pxScene = Physics::GetPxScene();

    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        if (pxRigidDynamic) {
            // Remove user data
            if (pxRigidDynamic->userData) {
                delete static_cast<PhysicsUserData*>(pxRigidDynamic->userData);
                pxRigidDynamic->userData = nullptr;
            }
            // Remove actor
            pxScene->removeActor(*pxRigidDynamic);
        }
    }
    m_pxRigidDynamics.clear();

    for (PxD6Joint* pxD6Joint : m_pxD6Joints) {
        if (pxD6Joint) {
            pxD6Joint->release();
        }
    }m_pxD6Joints.clear();
}

bool RagdollV2::IsInMotion() {
    const float linearThreshold = 0.01f;
    const float angularThreshold = 0.01f;
    const float linearThresholdSq = linearThreshold * linearThreshold;
    const float angularThresholdSq = angularThreshold * angularThreshold;

    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        if (!pxRigidDynamic) continue;
        if (pxRigidDynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) continue;
        if (pxRigidDynamic->isSleeping()) continue;

        const PxVec3 v = pxRigidDynamic->getLinearVelocity();
        const PxVec3 w = pxRigidDynamic->getAngularVelocity();
        if (v.magnitudeSquared() > linearThresholdSq || w.magnitudeSquared() > angularThresholdSq) {
            return true;
        }
    }
    return false;
}

AABB RagdollV2::GetWorldSpaceAABB() {
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());

    for (PxRigidDynamic* pxRigidDynamic : m_pxRigidDynamics) {
        if (pxRigidDynamic) {
            const PxU32 nbShapes = pxRigidDynamic->getNbShapes();
            if (!nbShapes) continue;
            std::vector<PxShape*> shapes(nbShapes);
            pxRigidDynamic->getShapes(shapes.data(), nbShapes);

            const PxTransform pose = pxRigidDynamic->getGlobalPose();
            for (PxShape * s : shapes) {
                const PxBounds3 b = PxShapeExt::getWorldBounds(*s, *pxRigidDynamic, 1.0f);
                glm::vec3 bmin(b.minimum.x, b.minimum.y, b.minimum.z);
                glm::vec3 bmax(b.maximum.x, b.maximum.y, b.maximum.z);
                min = glm::min(min, bmin);
                max = glm::max(max, bmax);
            }
        }
    }

    return AABB(min, max);
}

glm::vec3 RagdollV2::GetMarkerColorByRigidIndex(uint32_t index) const {
    if (index >= m_markerColors.size()) {
        Logging::Error() << "RagdollV2::GetMarkerColorByRigidIndex() failed, index " << index << " out of range of size " << m_pxRigidDynamics.size();
        return glm::vec3(1.0f);
    }
    return m_markerColors[index];
}

glm::mat4 RagdollV2::GetModelMatrixByRigidIndex(uint32_t index) const {
    if (index >= m_pxRigidDynamics.size()) {
        Logging::Error() << "RagdollV2::GetModelMatrixByRigidIndex() failed, index " << index << " out of range of size " << m_pxRigidDynamics.size();
        return glm::mat4(1.0f);
    }
    Transform scaleTransform;
    scaleTransform.scale = glm::vec3(m_scale);
    return Physics::PxMat44ToGlmMat4(m_pxRigidDynamics[index]->getGlobalPose()) * scaleTransform.to_mat4();
}

void RagdollV2::AddMarkerMeshData(RagdollMarker& marker, RagdollSolver& solver) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(marker.convexMeshVertices.size());
    indices.reserve(marker.convexMeshIndices.size());

    const RdGeometryDescriptionComponent& desc = marker.geometryDescriptionComponent;

    // Apply local shape transfrm
    auto applyLocalShapeXform = [&](size_t begin) {
        glm::mat4 R(1.0f);
        R = glm::rotate(R, (float)desc.rotation.z(), glm::vec3(0, 0, 1));
        R = glm::rotate(R, (float)desc.rotation.y(), glm::vec3(0, 1, 0));
        R = glm::rotate(R, (float)desc.rotation.x(), glm::vec3(1, 0, 0));

        const float s = (float)solver.sceneScale;
        glm::vec3 T((float)desc.offset.x(),
                    (float)desc.offset.y(),
                    (float)desc.offset.z());
        T /= s;

        for (size_t i = begin; i < vertices.size(); ++i) {
            glm::vec3 p = vertices[i].position;
            p = glm::vec3(R * glm::vec4(p, 1.0f));
            vertices[i].position = p + T;
        }
    };

    if (desc.type == RdGeometryType::kConvexHull) {
        for (RdPoint& point : marker.convexMeshVertices) {
            Vertex& vertex = vertices.emplace_back();
            vertex.position.x = point.x();
            vertex.position.y = point.y();
            vertex.position.z = point.z();
        }
        for (RdUint& index : marker.convexMeshIndices) {
            indices.emplace_back(index);
        }
    }
    else if (desc.type == RdGeometryType::kBox) {
        const float hx = (float)desc.extents.x() * 0.5f;
        const float hy = (float)desc.extents.y() * 0.5f;
        const float hz = (float)desc.extents.z() * 0.5f;

        const size_t vbase = vertices.size();

        auto addFace = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            const uint32_t base = (uint32_t)vertices.size();
            Vertex v0; Vertex v1; Vertex v2; Vertex v3;
            const glm::vec3 invS = glm::vec3(1.0f / (float)solver.sceneScale);
            v0.position = p0 * invS; v1.position = p1 * invS; v2.position = p2 * invS; v3.position = p3 * invS;
            v0.uv = { 0,0 }; v1.uv = { 1,0 }; v2.uv = { 1,1 }; v3.uv = { 0,1 };
            vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
            indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
            indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
        };

        const glm::vec3 v000(-hx, -hy, -hz), v001(-hx, -hy, +hz), v010(-hx, +hy, -hz), v011(-hx, +hy, +hz);
        const glm::vec3 v100(+hx, -hy, -hz), v101(+hx, -hy, +hz), v110(+hx, +hy, -hz), v111(+hx, +hy, +hz);

        addFace(v100, v110, v111, v101); // +X
        addFace(v001, v011, v010, v000); // -X
        addFace(v010, v011, v111, v110); // +Y
        addFace(v100, v101, v001, v000); // -Y
        addFace(v101, v111, v011, v001); // +Z
        addFace(v000, v010, v110, v100); // -Z

        applyLocalShapeXform(vbase);
    }

    else if (desc.type == RdGeometryType::kCapsule) {
        const float r = (float)desc.radius/ solver.sceneScale;
        const float half = 0.5f * (float)desc.length / solver.sceneScale;
        const unsigned int hemisphereRings = 12;
        const unsigned int cylinderRings = 1;
        const unsigned int segments = 24;

        const size_t vbase = vertices.size();

        const double PI = 3.14159265358979323846;
        const double TAU = 6.28318530717958647692;
        const double PI_HALF = 1.57079632679489661923;

        const double hemisphereRingAngleIncrement = PI_HALF / (double)hemisphereRings;

        auto append = [&](glm::dvec3 p, glm::dvec3 n) {
            glm::vec3 pos = glm::vec3((float)p.x, (float)(p.y * r), (float)(p.z * r));
            float u = (float)(std::atan2(p.z, p.y) / TAU);
            if (u < 0.0f) u += 1.0f;
            float v = (float)((p.x + (half + r)) / (2.0f * (half + r)));
            Vertex vtx;
            vtx.position = pos;
            vtx.normal = glm::normalize(glm::vec3((float)n.x, (float)n.y, (float)n.z));
            vtx.uv = glm::vec2(u, v);
            vertices.push_back(vtx);
        };

        auto capVertex = [&](double x, double normalX) {
            append({ x, 0.0, 0.0 }, { normalX, 0.0, 0.0 });
        };

        auto hemisphereVertexRings = [&](unsigned int count, double centerX, double startRingAngle, double ringAngleIncrement) {
            const double segInc = TAU / (double)segments;
            for (unsigned int i = 0; i != count; ++i) {
                const double a = startRingAngle + (double)i * ringAngleIncrement;
                const double s = std::sin(a);
                const double c = std::cos(a);
                for (unsigned int j = 0; j != segments; ++j) {
                    const double b = (double)j * segInc;
                    const double sb = std::sin(b), cb = std::cos(b);
                    append({ centerX + s * (double)r, c * sb, c * cb },
                           { s,                        c * sb, c * cb });
                }
            }
        };

        auto cylinderVertexRings = [&](const unsigned int count, const double startX, const glm::dvec2& inc) {
            const glm::dvec2 baseNormal = { inc.y, -inc.x };
            glm::dvec2 base = { 1.0, startX };
            const double segInc = TAU / (double)segments;
            for (unsigned int i = 0; i != count; ++i) {
                for (unsigned int j = 0; j != segments; ++j) {
                    const double b = (double)j * segInc;
                    const double sb = std::sin(b), cb = std::cos(b);
                    append({ base.y, base.x * sb, base.x * cb },
                           { baseNormal.y, baseNormal.x * sb, baseNormal.x * cb });
                }
                base.x += inc.x;
                base.y += inc.y;
            }
        };

        auto bottomFaceRing = [&]() {
            const unsigned int tip = (unsigned int)vbase;
            for (unsigned int j = 0; j != segments; ++j) {
                unsigned int topRight = (j != segments - 1) ? vbase + 1 + j + 1 : vbase + 1;
                unsigned int bottom = tip;
                unsigned int topLeft = vbase + 1 + j;
                indices.push_back(topRight);
                indices.push_back(bottom);
                indices.push_back(topLeft);
            }
        };

        auto faceRings = [&](unsigned int count, unsigned int offset) {
            const unsigned int vertexSegments = segments;
            for (unsigned int i = 0; i != count; ++i) {
                for (unsigned int j = 0; j != segments; ++j) {
                    const unsigned int bottomLeft = (unsigned int)vbase + offset + i * vertexSegments + j;
                    const unsigned int bottomRight = (j != segments - 1)
                        ? (unsigned int)vbase + offset + i * vertexSegments + j + 1
                        : (unsigned int)vbase + offset + i * vertexSegments;
                    const unsigned int topLeft = bottomLeft + vertexSegments;
                    const unsigned int topRight = bottomRight + vertexSegments;
                    indices.push_back(bottomRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);
                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(topLeft);
                }
            }
        };

        auto topFaceRing = [&]() {
            const unsigned int tip = (unsigned int)vertices.size() - 1;
            const unsigned int ringStart = tip - segments;
            for (unsigned int j = 0; j != segments; ++j) {
                unsigned int topRight = (j != segments - 1) ? ringStart + j + 1 : ringStart;
                unsigned int bottom = tip;
                unsigned int topLeft = ringStart + j;
                indices.push_back(topLeft);
                indices.push_back(bottom);
                indices.push_back(topRight);
            }
        };

        capVertex(-(double)(half + r), -1.0);
        hemisphereVertexRings(hemisphereRings - 1, -(double)half, -PI_HALF + hemisphereRingAngleIncrement, hemisphereRingAngleIncrement);
        cylinderVertexRings(cylinderRings + 1, -(double)half, glm::dvec2{ 0.0, 2.0 * (double)half / (double)cylinderRings });
        hemisphereVertexRings(hemisphereRings - 1, +(double)half, hemisphereRingAngleIncrement, hemisphereRingAngleIncrement);
        capVertex(+(double)(half + r), 1.0);

        bottomFaceRing();
        faceRings(hemisphereRings * 2 - 2 + cylinderRings, 1);
        topFaceRing();

        applyLocalShapeXform(vbase);
    }

    else if (desc.type == RdGeometryType::kSphere) {
        const float r = (float)desc.radius / solver.sceneScale;
        const int lat = 16, lon = 24;

        const size_t vbase = vertices.size();

        for (int y = 0; y <= lat; ++y) {
            float v = (float)y / (float)lat;
            float a1 = v * 3.14159265359f;
            float sy = std::cos(a1);
            float sr = std::sin(a1);

            for (int x = 0; x <= lon; ++x) {
                float u = (float)x / (float)lon;
                float a2 = u * 6.28318530718f;
                float cx = std::cos(a2);
                float sx = std::sin(a2);

                Vertex vert;
                vert.position = { r * sr * cx, r * sy, r * sr * sx };
                vert.uv = { u, v };
                vertices.push_back(vert);
            }
        }

        auto idx = [&](int x, int y) { return (uint32_t)(vbase + y * (lon + 1) + x); };

        for (int y = 0; y < lat; ++y) {
            for (int x = 0; x < lon; ++x) {
                uint32_t a = idx(x, y);
                uint32_t b = idx(x + 1, y);
                uint32_t c = idx(x + 1, y + 1);
                uint32_t d = idx(x, y + 1);
                indices.push_back(a); indices.push_back(b); indices.push_back(c);
                indices.push_back(a); indices.push_back(c); indices.push_back(d);
            }
        }

        applyLocalShapeXform(vbase);
    }

    // Generate normals/tangents
    for (int i = 0; i < indices.size(); i += 3) {
        Vertex* vert0 = &vertices[indices[i]];
        Vertex* vert1 = &vertices[indices[i + 1]];
        Vertex* vert2 = &vertices[indices[i + 2]];

        glm::vec3 deltaPos1 = vert1->position - vert0->position;
        glm::vec3 deltaPos2 = vert2->position - vert0->position;
        glm::vec2 deltaUV1 = vert1->uv - vert0->uv;
        glm::vec2 deltaUV2 = vert2->uv - vert0->uv;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vert0->tangent = tangent;
        vert1->tangent = tangent;
        vert2->tangent = tangent;

        glm::vec3 normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
        vert0->normal = normal;
        vert1->normal = normal;
        vert2->normal = normal;
    }

    m_meshBuffer.AddMesh(vertices, indices, marker.name);
    //Logging::Debug() << "Added " << marker.shapeType << " vertex data: " << marker.name << " " << vertices.size() << " verts " << indices.size() << " indices";
}

void RagdollV2::SetRigidGlobalPosesFromAnimatedGameObject(AnimatedGameObject* animatedGameObject) {
    if (!animatedGameObject) {
        std::cout << "RagdollV2::SetRigidGlobalPosesFromAnimatedGameObject() failed because AnimatedGameObject was nullptr\n";
        return;
    }

    for (int i = 0; i < m_markerBoneNames.size(); i++) {
        const std::string& markerBoneName = m_markerBoneNames[i];

        for (const auto& entry : animatedGameObject->GetSkinnedModel()->m_boneMapping) {
            const std::string& boneName = entry.first;
            unsigned int boneIndex = entry.second;

            if (markerBoneName == boneName) {
                PxRigidDynamic* pxRigidDynamic = m_pxRigidDynamics[i];

                if (pxRigidDynamic) {
                    glm::mat4 objectMatrixWorld = animatedGameObject->GetModelMatrix();
                    glm::mat4 boneMatrixLocal = animatedGameObject->GetAnimatedTransformByBoneName(boneName);
                    glm::mat4 boneMatrixWorld = objectMatrixWorld * boneMatrixLocal;

                    PxTransform pxTransform = PxTransform(Physics::GlmMat4ToPxMat44(boneMatrixWorld));
                    pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
                    pxRigidDynamic->setGlobalPose(pxTransform);
                    break;
                }
                else {
                    Logging::Error() << "pxRigidDynamic for " << markerBoneName << " is nullptr";
                }
            }
        }
    }
}

void RagdollV2::EnableRendering() {
    m_renderingEnabled = true;
}

void RagdollV2::DisableRendering() {
    m_renderingEnabled = false;
}