#include "Physics.h"
#include "Physics/Types/Ragdoll.h"
#include "Core/JSON.h"
#include "UniqueId.h"
#include "Util.h"

#include <unordered_map>

#include "Input/Input.h"

namespace Physics {
    std::unordered_map<std::string, RagdollComponents> g_ragdollComponents;
    std::unordered_map<uint64_t, Ragdoll> g_ragdolls;

    void LoadRagdollsFromDisk() {
        g_ragdollComponents.clear();

        for (FileInfo& fileInfo : Util::IterateDirectory("res/ragdolls/", { "rag" })) {
            RagdollComponents& ragdollComponents = g_ragdollComponents[fileInfo.name] = RagdollComponents();

            nlohmann::json json;
            if (!JSON::LoadJsonFromFile(json, fileInfo.path)) {
                std::cerr << "LoadRagdollsFromDisk() failed to open file '" << fileInfo.path << "'\n";
                return;
            }

            for (auto& [entityName, entity] : json["entities"].items()) {
                const auto& comps = entity["components"];

                // Rigid components
                if (comps.contains("RigidComponent")) {
                    RigidComponent rigidComponent;
                    rigidComponent.ID = entity["id"].get<int>();
                    rigidComponent.name = comps["NameComponent"]["members"]["value"].get<std::string>();

                    rigidComponent.restMatrix = comps["RestComponent"]["members"]["matrix"]["values"].get<glm::mat4>();
                    rigidComponent.scaleAbsoluteVector = comps["ScaleComponent"]["members"]["absolute"]["values"].get<glm::vec3>();

                    auto& geom = comps["GeometryDescriptionComponent"]["members"];
                    rigidComponent.radius = geom["radius"].get<float>();
                    rigidComponent.capsuleLength = geom["length"].get<float>();
                    rigidComponent.shapeType = geom["type"].get<std::string>();
                    rigidComponent.boxExtents = geom["extents"]["values"].get<glm::vec3>();
                    rigidComponent.offset = geom["offset"]["values"].get<glm::vec3>();

                    rigidComponent.rotation = geom["rotation"]["values"].get<glm::quat>();

                    auto& rigidM = comps["RigidComponent"]["members"];
                    rigidComponent.mass = rigidM["mass"].get<float>();
                    rigidComponent.friction = rigidM["friction"].get<float>();
                    rigidComponent.restitution = rigidM["restitution"].get<float>();
                    rigidComponent.linearDamping = rigidM["linearDamping"].get<float>();
                    rigidComponent.angularDamping = rigidM["angularDamping"].get<float>();
                    rigidComponent.sleepThreshold = rigidM["sleepThreshold"].get<float>();
                    rigidComponent.angularMass = rigidM["angularMass"]["values"].get<glm::vec3>();

                    std::string src = comps["MarkerUIComponent"]["members"]["sourceTransform"].get<std::string>();
                    rigidComponent.correspondingJointName = src.substr(src.rfind("|") + 1);

                    if (rigidComponent.name != "rSceneShape" && rigidComponent.correspondingJointName != "rScene") {
                        ragdollComponents.rigids.push_back(rigidComponent);
                    }
                }

                // — Joint components —
                if (comps.contains("JointComponent")) {

                    std::string rawName = comps["NameComponent"]["members"]["value"].get<std::string>();
                    bool isAbs = rawName.find("Absolute") != std::string::npos;
                    bool isRel = rawName.find("Relative") != std::string::npos;

                    std::string baseName = rawName;
                    if (isAbs) baseName.erase(baseName.find("Absolute"), 8);
                    if (isRel) baseName.erase(baseName.find("Relative"), 8);

                    JointComponent& jc = ragdollComponents.jointMap[baseName];
                    jc.name = baseName;

                    auto& jm = comps["JointComponent"]["members"];
                    glm::mat4 pFrame = jm["parentFrame"]["values"].get<glm::mat4>();
                    glm::mat4 cFrame = jm["childFrame"]["values"].get<glm::mat4>();

                    jc.parentID = jm["parent"]["value"].get<int>();
                    jc.childID = jm["child"]["value"].get<int>();

                    if (isAbs) {
                        jc.absParentFrame = pFrame;
                        jc.absChildFrame = cFrame;
                    }
                    if (isRel) {
                        jc.parentFrame = pFrame;
                        jc.childFrame = cFrame;

                        // parse drive
                        auto& d = comps["DriveComponent"]["members"];
                        jc.drive_angularDamping = d["angularDamping"].get<float>();
                        jc.drive_angularStiffness = d["angularStiffness"].get<float>();
                        jc.drive_linearDampening = d["linearDamping"].get<float>();
                        jc.drive_linearStiffness = d["linearStiffness"].get<float>();
                        jc.drive_enabled = d["enabled"].get<bool>();
                        jc.target = d["target"]["values"].get<glm::mat4>();

                        // parse limit
                        auto& L = comps["LimitComponent"]["members"];
                        float twistDeg = L.contains("twist") ? L["twist"].get<float>() : 0.0f;
                        float swing1Deg = L["swing1"].get<float>();
                        float swing2Deg = L["swing2"].get<float>();

                        jc.twist = glm::radians(twistDeg);
                        jc.swing1 = glm::radians(swing1Deg);
                        jc.swing2 = glm::radians(swing2Deg);


                        jc.limit = glm::vec3(L["x"].get<float>(),
                                             L["y"].get<float>(),
                                             L["z"].get<float>());

                        jc.limit_linearStiffness = L["linearStiffness"].get<float>();
                        jc.limit_linearDampening = L["linearDamping"].get<float>();
                        jc.limit_angularStiffness = L["angularStiffness"].get<float>();
                        jc.limit_angularDampening = L["angularDamping"].get<float>();
                       //
                       //std::cout << "jc.limit_linearStiffness: " << jc.limit_linearStiffness << "\n";
                       //std::cout << "jc.limit_linearDampening: " << jc.limit_linearDampening << "\n";
                       //std::cout << "jc.limit_angularStiffness: " << jc.limit_angularStiffness << "\n";
                       //std::cout << "jc.limit_angularDampening: " << jc.limit_angularDampening << "\n";
                       //
                       //
                       //jc.limit_linearStiffness *= 0.00000001f;
                       //jc.limit_angularStiffness *= 0.00000001f;
                       //jc.limit_linearDampening  *= 0.1f;
                       //jc.limit_angularDampening *= 0.1f;

                        //jc.limit_linearStiffness = 500.0f;
                        //jc.limit_linearDampening = 50.0f;
                        //jc.limit_angularStiffness = 200.0f;
                        //jc.limit_angularDampening = 20.0f;

                        jc.limit_linearStiffness *= 0.001f;
                        jc.limit_angularStiffness *= 0.001f;
                        jc.limit_linearDampening *= 0.001f;
                        jc.limit_angularDampening *= 0.001f;

                        jc.limit_linearStiffness = 10000;
                        jc.limit_linearDampening = 1000000;
                        jc.drive_angularStiffness = 10000;
                        jc.drive_angularDamping = 1000000;

                        if (jc.parentID != 0) {
                            ragdollComponents.joints.push_back(jc);
                        }
                    }
                }
            }

           // std::cout << "Loaded ragdoll: '" << fileInfo.name << ".rag'\n";
           // std::cout << " - rigid count: " << ragdollComponents.rigids.size() << "\n";
           // std::cout << " - joint count: " << ragdollComponents.joints.size() << "\n";
        }
    }

    inline PxTransform GlmMat4ToPxTransform(const glm::mat4& m) {
        glm::vec3 t = glm::vec3(m[3]);                  // column-major translation
        glm::quat q = glm::quat_cast(m);                // extract rotation
        return PxTransform(Physics::GlmVec3toPxVec3(t), Physics::GlmQuatToPxQuat(q));
    }

    uint64_t CreateRagdollByName(const std::string& name, float ragdollTotalWeight) {
        if (g_ragdollComponents.find(name) == g_ragdollComponents.end()) {
            std::cout << "Physics::CreateRagdollByName() failed: '" << name << "' not found\n";
            return 0;
        }

        //std::cout << "Physics::CreateRagdollByName() " << name << "\n";
        
        uint64_t ragdollID = UniqueID::GetNext(ObjectType::RAGDOLL_V1);

        Ragdoll& ragdoll = g_ragdolls[ragdollID];
        ragdoll.m_name = name;

        RagdollComponents ragdollComponents = g_ragdollComponents[name];

        for (JointComponent& joint : ragdollComponents.joints) {
            //std::cout << joint.name << " " << joint.parentID << " " << joint.childID << "\n";
            //joint.name = joint.name.substr(8);
            //joint.name = joint.name.substr(0, joint.name.size() - 8);
        }

        //std::cout << name << ": " << ragdollComponents.joints.size() << " joints\n";

        // Temp vector of RigidComponent Ids to lookup
        std::vector <uint32_t> rigidComponentIds;

        for (RigidComponent& rigidComponent : ragdollComponents.rigids) {
            PxTransform shapeOffsetTranslation = PxTransform(PxVec3(rigidComponent.offset.x, rigidComponent.offset.y, rigidComponent.offset.z));
            PxQuat pxRotation = GlmQuatToPxQuat(rigidComponent.rotation);            
            PxTransform shapeOffsetRotation = PxTransform(pxRotation);
            glm::mat4 restMatrix = rigidComponent.restMatrix;

            glm::mat4 shapeOffsetMatrix = Physics::PxMat44ToGlmMat4(shapeOffsetTranslation.transform(shapeOffsetRotation));

            if (rigidComponent.shapeType == "Capsule") {
                rigidComponent.radius *= rigidComponent.scaleAbsoluteVector.x;
                rigidComponent.capsuleLength *= rigidComponent.scaleAbsoluteVector.y;
                float halfExtent = rigidComponent.capsuleLength * 0.5f;
                float radius = rigidComponent.radius;

                PxMaterial* material = Physics::GetDefaultMaterial();
                PxCapsuleGeometry geom = PxCapsuleGeometry(radius, halfExtent);
                PxShape* pxShape = Physics::GetPxPhysics()->createShape(geom, *material, true);

                uint64_t rigidDynmamicId = Physics::CreateRigidDynamicFromPxShape(pxShape, restMatrix, shapeOffsetMatrix);
                ragdoll.m_rigidDynamicIds.push_back(rigidDynmamicId);
                ragdoll.m_correspondingBoneNames.push_back(rigidComponent.correspondingJointName);

                rigidComponentIds.push_back(rigidComponent.ID);
            }
            else if (rigidComponent.shapeType == "Box") {
                rigidComponent.boxExtents.x *= rigidComponent.scaleAbsoluteVector.x;
                rigidComponent.boxExtents.y *= rigidComponent.scaleAbsoluteVector.y;
                rigidComponent.boxExtents.z *= rigidComponent.scaleAbsoluteVector.z;
                float halfExtent = rigidComponent.capsuleLength;
                float radius = rigidComponent.radius;

                PxMaterial* material = Physics::GetDefaultMaterial();
                PxBoxGeometry geom = PxBoxGeometry(rigidComponent.boxExtents.x * 0.5f, rigidComponent.boxExtents.y * 0.5f, rigidComponent.boxExtents.z * 0.5f);
                PxShape* pxShape = Physics::GetPxPhysics()->createShape(geom, *material, true);

                uint64_t rigidDynmamicId = Physics::CreateRigidDynamicFromPxShape(pxShape, restMatrix, shapeOffsetMatrix);
                ragdoll.m_rigidDynamicIds.push_back(rigidDynmamicId);
                ragdoll.m_correspondingBoneNames.push_back(rigidComponent.correspondingJointName);

                rigidComponentIds.push_back(rigidComponent.ID);
            }
            else if (rigidComponent.shapeType == "Sphere") {
                rigidComponent.radius *= rigidComponent.scaleAbsoluteVector.x;
                float radius = rigidComponent.radius;

                PxMaterial* material = Physics::GetDefaultMaterial();
                PxSphereGeometry geom = PxSphereGeometry(radius);
                PxShape* pxShape = Physics::GetPxPhysics()->createShape(geom, *material, true);

                uint64_t rigidDynmamicId = Physics::CreateRigidDynamicFromPxShape(pxShape, restMatrix, shapeOffsetMatrix);
                ragdoll.m_rigidDynamicIds.push_back(rigidDynmamicId);
                ragdoll.m_correspondingBoneNames.push_back(rigidComponent.correspondingJointName);

                rigidComponentIds.push_back(rigidComponent.ID);
            }
            else {
                std::cout << "RigidComponent '" << rigidComponent.name << "' had invalid shape\n";
                continue;
            }
        }

        // Compute the total volume of the ragdoll
        float totalVolume = 0.0f;
        for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
            RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);

            if (rigidDynamic) {
                totalVolume += rigidDynamic->GetVolume();
            }
        }

        // Compute the mass of each shape relative to the total weight of the ragdoll
        for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
            RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);

            if (rigidDynamic) {
                float volume = rigidDynamic->GetVolume();
                float mass = ragdollTotalWeight * (volume / totalVolume);
                float density = Util::GetDensity(mass, volume);
                rigidDynamic->UpdateMassAndInertia(density);
            }
        }

        // Does this do anything?
        for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
            RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);
            rigidDynamic->GetPxRigidDynamic()->setSolverIterationCounts(8, 1);
        }

        // Set some reasonable default filter data
        PhysicsFilterData filterData;
        filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
        filterData.collisionGroup = CollisionGroup::RAGDOLL_ENEMY;
        filterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | CHARACTER_CONTROLLER);


        for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
            RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);
            if (rigidDynamic) {
                rigidDynamic->SetFilterData(filterData);
            }
        }

        PxScene* pxScene = Physics::GetPxScene();

        for (JointComponent& joint : ragdollComponents.joints) {
            if (joint.parentID == 0) continue;

            uint64_t parentRigidDynamicId = 0, childRigidDynamicId = 0;
            for (int i = 0; i < rigidComponentIds.size(); i++) {
                if (rigidComponentIds[i] == joint.parentID)
                    parentRigidDynamicId = ragdoll.m_rigidDynamicIds[i];
                else if (rigidComponentIds[i] == joint.childID)
                    childRigidDynamicId = ragdoll.m_rigidDynamicIds[i];
            }
            auto* parentRigidDynamic = GetRigidDynamicById(parentRigidDynamicId);
            auto* childRigidDynamic = GetRigidDynamicById(childRigidDynamicId);
            if (!parentRigidDynamic || !childRigidDynamic) {
                std::cout << "…failed to retrieve parent or child\n";
                continue;
            }

            PxRigidDynamic* pPx = parentRigidDynamic->GetPxRigidDynamic();
            PxRigidDynamic* cPx = childRigidDynamic->GetPxRigidDynamic();

            PxTransform absP = GlmMat4ToPxTransform(joint.absParentFrame);
            PxTransform absC = GlmMat4ToPxTransform(joint.absChildFrame);

            PxTransform localP = pPx->getGlobalPose().transformInv(absP);
            PxTransform localC = cPx->getGlobalPose().transformInv(absC);

            glm::mat4 glmLocalP = Physics::PxMat44ToGlmMat4(localP);
            glm::mat4 glmLocalC = Physics::PxMat44ToGlmMat4(localC);

            uint64_t d6JointId = Physics::CreateD6Joint(
                parentRigidDynamicId,
                childRigidDynamicId,
                joint.parentFrame,
                joint.childFrame 
            );

            ragdoll.m_d6JointIds.push_back(d6JointId);


            D6Joint* d6Joint = Physics::GetD6JointById(d6JointId);
            PxD6Joint* pxD6Joint = d6Joint->GetPxD6Joint();
            if (!d6Joint) {
                std::cout << "Physics::CreateRagdollByName() failed to retrieve d6Joint by id " << d6JointId << "\n";
                continue;
            }

           //PxD6Joint* pxD6Joint = d6Joint->GetPxD6Joint();
           //if (!pxD6Joint) {
           //    std::cout << "Physics::CreateRagdollByName() failed because a pxD6Joint was nullptr\n";
           //    continue;
           //}

            // Linear spring
            //joint.limit_linearStiffness = 10000;
            //joint.limit_linearDampening = 1000000;
            //joint.drive_angularStiffness = 10000;
            //joint.drive_angularDamping = 1000000;


            const PxSpring linearSpring = PxSpring(joint.limit_linearStiffness, joint.limit_linearDampening);

            if (joint.limit.x > -1) {
                const PxJointLinearLimitPair limitX = PxJointLinearLimitPair(-joint.limit.x, joint.limit.x, linearSpring);
                pxD6Joint->setLinearLimit(PxD6Axis::eX, limitX);
            }

            if (joint.limit.y > -1) {
                const PxJointLinearLimitPair limitY = PxJointLinearLimitPair(-joint.limit.y, joint.limit.y, linearSpring);
                pxD6Joint->setLinearLimit(PxD6Axis::eY, limitY);
            }

            if (joint.limit.z > -1) {
                const PxJointLinearLimitPair limitZ = PxJointLinearLimitPair(-joint.limit.z, joint.limit.z, linearSpring);
                pxD6Joint->setLinearLimit(PxD6Axis::eZ, limitZ);
            }

            const PxSpring angularSpring = PxSpring(joint.drive_angularStiffness, joint.drive_angularDamping);
            const PxJointAngularLimitPair twistLimit = PxJointAngularLimitPair(-joint.twist, joint.twist, angularSpring);
            const PxJointLimitCone swingLimit = PxJointLimitCone(joint.swing1, joint.swing2, angularSpring);

            pxD6Joint->setTwistLimit(twistLimit);
            pxD6Joint->setSwingLimit(swingLimit);

           // pxD6Joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
           // pxD6Joint->setConstraintFlag(PxD6JointFlag::ePROJECTION, true);
           // pxD6Joint->setProjectionLinearTolerance(0.05f);
           // pxD6Joint->setProjectionAngularTolerance(glm::radians(0.5f));

            if (joint.limit.x > 0) pxD6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
            if (joint.limit.y > 0) pxD6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
            if (joint.limit.z > 0) pxD6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);

            if (joint.limit.x < 0) pxD6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
            if (joint.limit.y < 0) pxD6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
            if (joint.limit.z < 0) pxD6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

            if (joint.twist > 0) pxD6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
            if (joint.swing1 > 0) pxD6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
            if (joint.swing2 > 0) pxD6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

            if (joint.twist < 0) pxD6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
            if (joint.swing1 < 0) pxD6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
            if (joint.swing2 < 0) pxD6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);


            if (joint.drive_enabled) {
                float driveForceLimit = FLT_MAX;    // Maximum force; ignored
                bool isAcceleration = true;         // read from the json file some day when you aren't lazy

                // Temp hack because you don't think you need drives. Do you?
                if (false) {
                    PxD6JointDrive linearDrive = PxD6JointDrive(joint.limit_linearStiffness, joint.limit_linearDampening, driveForceLimit, isAcceleration);
                    pxD6Joint->setDrive(PxD6Drive::eX, linearDrive);
                    pxD6Joint->setDrive(PxD6Drive::eY, linearDrive);
                    pxD6Joint->setDrive(PxD6Drive::eZ, linearDrive);
                }

                // Temp hack because you don't think you need drives. Do you?
                if (false) {
                    PxD6JointDrive angularDrive = PxD6JointDrive(joint.drive_angularStiffness, joint.drive_angularDamping, driveForceLimit, isAcceleration);
                    pxD6Joint->setDrive(PxD6Drive::eTWIST, angularDrive);
                    pxD6Joint->setDrive(PxD6Drive::eSWING, angularDrive);
                    pxD6Joint->setDrive(PxD6Drive::eSLERP, angularDrive);
                }

                // TODO: Allow for changes to be made to both parent and child frames without affecting the drive target
                // Currently you cannot edit the child anchorpoint!!!

                // It's been too many years since you wrote the above comment, one day unravel what the fuck you're talking about there
            }

            else {
                // Temp hack because you don't think you need drives. Do you?
                if (false) {
                    const PxD6JointDrive defaultDrive{ 0.0f, 0.0f, 0.0f, false };
                    pxD6Joint->setDrive(PxD6Drive::eX, defaultDrive);
                    pxD6Joint->setDrive(PxD6Drive::eY, defaultDrive);
                    pxD6Joint->setDrive(PxD6Drive::eZ, defaultDrive);
                    pxD6Joint->setDrive(PxD6Drive::eTWIST, defaultDrive);
                    pxD6Joint->setDrive(PxD6Drive::eSWING, defaultDrive);
                    pxD6Joint->setDrive(PxD6Drive::eSLERP, defaultDrive);
                }
            }
        }

        ragdoll.m_components = ragdollComponents;
        return ragdollID;
    }

    bool RagdollExists(uint64_t ragdollId) {
        return (g_ragdolls.find(ragdollId) != g_ragdolls.end());
    }

    Ragdoll* GetRagdollById(uint64_t ragdollId) {
        if (RagdollExists(ragdollId)) {
            return &g_ragdolls[ragdollId];
        }
        else {
            return nullptr;
        }
    }

    /*void RemoveRagdoll(uint64_t ragdollId) {
        if (RagdollExists(ragdollId)) {
            PxPhysics* pxPhysics = Physics::GetPxPhysics();
            PxScene* pxScene = Physics::GetPxScene();
            Ragdoll& ragdoll = g_ragdolls[ragdollId];

            for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
                RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);

                if (rigidDynamic) {
                    PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();

                    if (pxRigidDynamic) {
                        // Clean up its user data
                        if (pxRigidDynamic->userData) {
                            delete static_cast<PhysicsUserData*>(pxRigidDynamic->userData);
                            pxRigidDynamic->userData = nullptr;
                        }
                        // Remove it from PxScene
                        if (pxRigidDynamic->getScene() != nullptr) {
                            pxScene->removeActor(*pxRigidDynamic);
                        }
                        // Release it
                        pxRigidDynamic->release();
                        pxRigidDynamic = nullptr;
                    }
                }
            }

            for (uint64_t d6JointId : ragdoll.m_d6JointIds) {
                D6Joint* d6Joint = GetD6JointById(d6JointId);

                if (d6Joint) {
                    PxD6Joint* pxD6Joint = d6Joint->GetPxD6Joint();

                    if (pxD6Joint) {
                        pxD6Joint->release();
                        pxD6Joint = nullptr;
                    }
                }
            }

            // Remove from container
            g_ragdolls.erase(ragdollId);
        }
    }*/

    bool PxRigidDynamicBelongsToRagdoll(PxRigidDynamic* pxRigidDynamic) {
        for (auto it = g_ragdolls.begin(); it != g_ragdolls.end(); ) {
            Ragdoll& ragdoll = it->second;
            for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
                RigidDynamic* rigidDynamic = GetRigidDynamicById(rigidDynamicId);
                if (rigidDynamic) {
                    if (pxRigidDynamic == rigidDynamic->GetPxRigidDynamic()) {
                        return true;
                    }
                }
            }
            it++;
        }
        return false;
    }

    std::vector<PxRigidDynamic*> GetRagdollPxRigidDynamics(uint64_t ragdollId) {
        std::vector<PxRigidDynamic*> result;

        if (RagdollExists(ragdollId)) {
            Ragdoll& ragdoll = g_ragdolls[ragdollId];

            for (int i = 0; i < ragdoll.m_rigidDynamicIds.size(); i++) {
                RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(ragdoll.m_rigidDynamicIds[i]);
                if (rigidDynamic && rigidDynamic->GetPxRigidDynamic()) {
                    result.push_back(rigidDynamic->GetPxRigidDynamic());
                }
            }
        }
        return result;
    }

    std::vector<PxRigidActor*> GetRagdollPxRigidActors(uint64_t ragdollId) {
        std::vector<PxRigidActor*> result;

        if (RagdollExists(ragdollId)) {
            Ragdoll& ragdoll = g_ragdolls[ragdollId];

            for (int i = 0; i < ragdoll.m_rigidDynamicIds.size(); i++) {
                RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(ragdoll.m_rigidDynamicIds[i]);
                if (rigidDynamic && rigidDynamic->GetPxRigidDynamic()) {
                    result.push_back(rigidDynamic->GetPxRigidDynamic());
                }
            }
        }
        return result;
    }

    void MarkRagdollForRemoval(uint64_t ragdollId) {
        if (RagdollExists(ragdollId)) {
            Ragdoll& ragdoll = g_ragdolls[ragdollId];
            ragdoll.MarkForRemoval();
        }
    }

    void RemoveAnyRagdollsMarkedForRemoval() {
        for (auto it = g_ragdolls.begin(); it != g_ragdolls.end(); ) {
            Ragdoll& ragdoll = it->second;

            if (ragdoll.IsMarkedForRemoval()) {
                for (uint64_t rigidDynamicId : ragdoll.m_rigidDynamicIds) {
                    Physics::MarkRigidDynamicForRemoval(rigidDynamicId);
                }
                for (uint64_t d6JointId : ragdoll.m_d6JointIds) {
                    Physics::MarkD6JointForRemoval(d6JointId);
                }

                // Remove from container
                it = g_ragdolls.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    int GetRagdollCount() {
        return g_ragdolls.size();
    }

    void PrintSceneRagdollInfo() {

    }
}