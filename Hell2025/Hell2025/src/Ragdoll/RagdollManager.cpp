#include "RagdollManager.h"

#include <fstream>
#include <sstream>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include <iostream>
#include "HellLogging.h"
#include "Api.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/schema.h"

#include "RagdollV2.h"
#include "Ragdoll_util.h"
#include "UniqueID.h"

inline RdEnum toInputType(std::string type) {
    return type == "Inherit" ? static_cast<short>(RdBehaviour::kInherit) : type == "Kinematic" ? static_cast<short>(RdBehaviour::kKinematic) : static_cast<short>(RdBehaviour::kDynamic);
}

inline std::string LastBone(const std::string& path) {
    const size_t p = path.find_last_of('|');
    return p == std::string::npos ? path : path.substr(p + 1);
}

inline void sanitizePath(std::string& s) {
    // Trim whitespace
    auto notspace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());

    // Replace '/' with '|' and strip '\r'
    for (char& c : s) {
        if (c == '/') c = '|';
    }
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());

    // Strip leading/trailing pipes
    while (!s.empty() && s.front() == '|') s.erase(s.begin());
    while (!s.empty() && s.back() == '|') s.pop_back();
}

inline std::string lastSegment(std::string s) {
    sanitizePath(s);
    if (s.empty()) return {};
    const auto pos = s.rfind('|');
    return (pos == std::string::npos) ? s : s.substr(pos + 1);
}

namespace RagdollManager {
    std::unordered_map<std::string, RagdollInfo> g_ragdollInfoSet; // Maps RagdollInfo to filename
    std::unordered_map<uint64_t, RagdollV2> g_ragdolls;
    MeshBuffer g_meshBuffer;

    void LoadFile(const FileInfo& fileInfo);
    void LoadMarkers(RagdollInfo& ragdoll, rapidjson::Document& doc);
    void LoadJoints(RagdollInfo& ragdoll, rapidjson::Document& doc);
    void LoadSolver(RagdollInfo& ragdoll, rapidjson::Document& doc);

    void Init() {
        Logging::Init() << "RagdollManager::Init()";
        for (FileInfo& fileInfo : Util::IterateDirectory("res/", { "rag" })) {
            LoadFile(fileInfo);
        }

        SpawnRagdoll(glm::vec3(36, 31, 36), glm::vec3(0.0f, 0.2f, 0.0f), "manikin");
        SpawnRagdoll(glm::vec3(37, 31, 36), glm::vec3(0.0f, -0.4f, 0.0f), "manikin");
        //SpawnRagdoll(glm::vec3(37, 31, 36), glm::vec3(0.0f, -0.4f, 0.0f), "dobermann");
        //Logging::Init() << "RagdollManager::Init()";
    }

    void LoadFile(const FileInfo& fileInfo) {
        std::ifstream file(fileInfo.path, std::ios::binary);
        if (!file) {
            Logging::Error() << fileInfo.path << " not found";
            return;
        }
        std::string jsonString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        rapidjson::Document doc;
        doc.Parse(jsonString.c_str());

        RagdollInfo& ragdollInfo = g_ragdollInfoSet[fileInfo.name] = RagdollInfo();
        LoadSolver(ragdollInfo, doc);
        LoadMarkers(ragdollInfo, doc);
        LoadJoints(ragdollInfo, doc);
    }


    void LoadSolver(RagdollInfo& ragdoll, rapidjson::Document& doc) {
        RdJsonRegistry registry{ doc };

        for (const auto& m : doc["entities"].GetObject()) {
            RdString jentity = m.name.GetString();

            JsonComponent nameComponent = registry.get(jentity, "NameComponent");
            std::string name = nameComponent.getString("value");

            if (registry.has(jentity, "SolverUIComponent")) {

                RagdollSolver& solver = ragdoll.m_solver;

                auto SolverComponent = registry.get(jentity, "SolverComponent");
                solver.positionIterations = SolverComponent.getInteger("positionIterations");
                solver.substeps = SolverComponent.getInteger("substeps");
                solver.gravity = SolverComponent.getVector("gravity");

                solver.sceneScale = { 1.0 };
                if (SolverComponent.has("sceneScale")) {
                    solver.sceneScale = SolverComponent.getFloat("sceneScale");
                }
                else {
                    solver.sceneScale = SolverComponent.getFloat("spaceMultiplier");
                }

                auto SolverUIComponent = registry.get(jentity, "SolverUIComponent");
                solver.linearLimitStiffness = SolverUIComponent.getFloat("linearLimitStiffness");
                solver.linearLimitDamping = SolverUIComponent.getFloat("linearLimitDamping");
                solver.angularLimitStiffness = SolverUIComponent.getFloat("angularLimitStiffness");
                solver.angularLimitDamping = SolverUIComponent.getFloat("angularLimitDamping");
            }
        }
    }

    void LoadMarkers(RagdollInfo& ragdoll, rapidjson::Document& doc) {
        RdJsonRegistry registry{ doc };

        for (const auto& m : doc["entities"].GetObject()) {
            RdString entity = m.name.GetString();

            // Pre 2022.01.01 the solver was a rigid body too (ground plane)
            if (registry.has(entity, "SolverComponent")) continue;
            if (!registry.has(entity, "MarkerUIComponent")) continue;

            JsonComponent color = registry.get(entity, "ColorComponent");
            JsonComponent convexMesh = registry.get(entity, "ConvexMeshComponents");
            JsonComponent geometryDescription = registry.get(entity, "GeometryDescriptionComponent");
            JsonComponent markerUI = registry.get(entity, "MarkerUIComponent");
            JsonComponent nameComponent = registry.get(entity, "NameComponent");
            JsonComponent restComponent = registry.get(entity, "RestComponent");
            JsonComponent rigid = registry.get(entity, "RigidComponent");
            JsonComponent scaleComponent = registry.get(entity, "ScaleComponent");
            JsonComponent sceneComponent = registry.get(entity, "SceneComponent");
            JsonComponent subs = registry.get(entity, "SubEntitiesComponent");

            RdString relEntity = subs.getEntity("relative");
            JsonComponent limit = registry.get(relEntity, "LimitComponent");
            JsonComponent drive = registry.get(relEntity, "DriveComponent");
            JsonComponent joint = registry.get(relEntity, "JointComponent");

            RdMatrix originMatrix{ RdIdentityInit };
            if (registry.has(entity, "OriginComponent")) {
                JsonComponent originComponent = registry.get(entity, "OriginComponent");
                originMatrix = originComponent.getMatrix("matrix");
            }
            else {
                // Otherwise it's safe to assume the rest matrix
                originMatrix = restComponent.getMatrix("matrix");
            }

            bool isKinematic = false;
            bool enableCCD = false;
            float contactStiffness = 0.0f;
            float contactDamping = 0.0f;
            float friction = 0.0f;
            float restitution = 0.0f;
            float linearDamping = 0.0f;
            float angularDamping = 0.0f;

            if (rigid.has("kinematic"))         isKinematic = rigid.getBoolean("kinematic");
            if (rigid.has("enableCCD"))         enableCCD = rigid.getBoolean("enableCCD");
            if (rigid.has("linearDamping"))     linearDamping = rigid.getFloat("linearDamping");
            if (rigid.has("angularDamping"))    angularDamping = rigid.getFloat("angularDamping");
            if (rigid.has("contactStiffness"))  contactStiffness = rigid.getFloat("contactStiffness");
            if (rigid.has("contactDamping"))    contactDamping = rigid.getFloat("contactDamping");
            if (rigid.has("friction"))          friction = rigid.getFloat("friction");
            if (rigid.has("restitution"))       restitution = rigid.getFloat("restitution");

            RdGeometryDescriptionComponent geometryDescriptionComponent;
            geometryDescriptionComponent.type = StringToRdGeometryType(geometryDescription.getString("type"));
            geometryDescriptionComponent.extents = geometryDescription.getVector("extents");
            geometryDescriptionComponent.offset = geometryDescription.getVector("offset");
            geometryDescriptionComponent.rotation = RdToEulerRotation(geometryDescription.getQuaternion("rotation"));
            geometryDescriptionComponent.radius = geometryDescription.getFloat("radius");
            geometryDescriptionComponent.length = geometryDescription.getFloat("length");

            if (geometryDescription.has("convexDecomposition")) {
                geometryDescriptionComponent.convexDecomposition = StringToRdConvexDecomposition(geometryDescription.getString("convexDecomposition"));
            }

            if (isKinematic) {
                // Skip the ground marker
                continue;
            }

            RagdollMarker& marker = ragdoll.m_markers.emplace_back();
            marker.name = nameComponent.getString("value");
            marker.inputMatrix = restComponent.getMatrix("matrix");
            marker.originMatrix = originMatrix;
            marker.parentFrame = joint.getMatrix("parentFrame");
            marker.childFrame = joint.getMatrix("childFrame");
            marker.limitRange = { limit.getFloat("twist"), limit.getFloat("swing1"), limit.getFloat("swing2") };
            marker.convexMeshVertices = convexMesh.getPoints("vertices");
            marker.convexMeshIndices = convexMesh.getUints("indices");
            marker.driveSlerp = drive.getBoolean("slerp");
            marker.driveSpringType = (int)drive.getBoolean("acceleration");
            marker.mass = markerUI.getFloat("mass");
            marker.linearStiffness = markerUI.getFloat("linearStiffness");
            marker.linearDampingRatio = markerUI.getFloat("linearDampingRatio");
            marker.angularStiffness = markerUI.getFloat("angularStiffness");
            marker.angularDampingRatio = markerUI.getFloat("angularDampingRatio");
            marker.inputType = toInputType(markerUI.getString("inputType"));
            marker.collisionGroup = markerUI.getInteger("collisionGroup");
            marker.useRootStiffness = markerUI.getBoolean("useRootStiffness", false);
            marker.useLinearAngularStiffness = markerUI.getBoolean("useLinearAngularStiffness");
            marker.color = color.getColor("value");
            marker.contactStiffness = contactStiffness;
            marker.contactDamping = contactDamping;
            marker.friction = friction;
            marker.restitution = restitution;
            marker.isKinematic = isKinematic;
            marker.enableCCD = enableCCD;
            marker.geometryDescriptionComponent = geometryDescriptionComponent;

            marker.scaleComponent.absolute = scaleComponent.getVector("absolute");
            marker.scaleComponent.value = scaleComponent.getVector("value");
            
            // Find corresponding bone name
            const std::vector<std::string> dst = markerUI.getStrings("destinationTransforms");
            for (const std::string& string : dst) {
                size_t pos = string.rfind("|");
                if (pos != std::string::npos) {
                    marker.boneName = string.substr(pos + 1);
                    break;
                }
            }
        }
    }

    void LoadJoints(RagdollInfo& ragdoll, rapidjson::Document& doc) {
        RdJsonRegistry registry{ doc };

        // Build a map of jsonEntity to marker name
        std::unordered_map<RdString, RdString> entityToMarkerName;
        for (const auto& m : doc["entities"].GetObject()) {
            RdString jentity = m.name.GetString();
            if (!registry.has(jentity, "MarkerUIComponent")) continue;
            auto Name = registry.get(jentity, "NameComponent");
            entityToMarkerName[jentity] = Name.getString("value");
        }

        // For each marker, pull its joint aka the "relative" subentity
        for (const auto& m : doc["entities"].GetObject()) {
            RdString childId = m.name.GetString();

            // Only markers participate
            if (!registry.has(childId, "MarkerUIComponent")) continue;

            // Find parent (new: ParentComponent, legacy: RigidComponent.parentRigid)
            RdString parentId = "-1";
            if (registry.has(childId, "ParentComponent")) {
                auto Parent = registry.get(childId, "ParentComponent");
                parentId = Parent.getEntity("entity");
            }
            else if (registry.has(childId, "RigidComponent")) {
                auto Rigid = registry.get(childId, "RigidComponent");
                parentId = Rigid.getEntity("parentRigid");
            }

            // Skip roots / invalid parents
            if (!doc["entities"].HasMember(parentId.c_str())) {
                continue;
            }

            // Joint sub entity that carries joint/limit/drive data
            if (!registry.has(childId, "SubEntitiesComponent")) {
                continue;
            }
            auto Subs = registry.get(childId, "SubEntitiesComponent");
            RdString jJoint = Subs.getEntity("relative");

            // Some very old scenes may be missing the joint sub entity
            if (!doc["entities"].HasMember(jJoint.c_str())) {
                continue;
            }

            // Components on the joint sub-entity
            auto Joint = registry.get(jJoint, "JointComponent");
            auto Limit = registry.get(jJoint, "LimitComponent");
            auto Drive = registry.get(jJoint, "DriveComponent");

            RdString childName = entityToMarkerName.count(childId) ? entityToMarkerName[childId] : childId;
            RdString parentName = entityToMarkerName.count(parentId) ? entityToMarkerName[parentId] : parentId;

            auto MarkerUI = registry.get(childId, "MarkerUIComponent");

            RagdollJoint& ragdollJoint = ragdoll.m_joints.emplace_back();
            ragdollJoint.name = childName + "_to_" + parentName;
            ragdollJoint.parentJsonId = parentId;
            ragdollJoint.childJsonId = childId;
            ragdollJoint.parentName = parentName;
            ragdollJoint.childName = childName;
            ragdollJoint.parentFrame = Joint.getMatrix("parentFrame");
            ragdollJoint.childFrame = Joint.getMatrix("childFrame");
            ragdollJoint.limitRange = { Limit.getFloat("twist"), Limit.getFloat("swing1"), Limit.getFloat("swing2") };
            ragdollJoint.driveSlerp = Drive.getBoolean("slerp");
            ragdollJoint.driveSpringType = static_cast<int>(Drive.getBoolean("acceleration"));
            ragdollJoint.driveAngularAmountTwist = Drive.getFloat("angularAmountTwist", 0.0f);
            ragdollJoint.driveAngularAmountSwing = Drive.getFloat("angularAmountSwing", 0.0f);
            ragdollJoint.linearMotion = StringToRdMotion(MarkerUI.getString("linearMotion"));
            ragdollJoint.relativeJsonId = jJoint;
            ragdollJoint.driveLinearAmount = Drive.getVector("linearAmount");

            // Limit tuning, prefer LimitComponent, fallback to MarkerUI
            if (Limit.has("stiffness")) {
                ragdollJoint.limitStiffness = Limit.getFloat("stiffness");
                ragdollJoint.limitDampingRatio = Limit.getFloat("dampingRatio");
                ragdollJoint.limitAutoOrient = Limit.getBoolean("autoOrient", false);
            }
            else {
                ragdollJoint.limitStiffness = MarkerUI.getFloat("limitStiffness", 0.0f);
                ragdollJoint.limitDampingRatio = MarkerUI.getFloat("limitDampingRatio", 0.0f);
                ragdollJoint.limitAutoOrient = MarkerUI.getBoolean("limitAutoOrient", false);
            }

            // Drove
            ragdollJoint.driveLinearStiffness = Drive.getFloat("linearStiffness", 0.0f);
            ragdollJoint.driveLinearDamping = Drive.getFloat("linearDamping", 0.0f);
            ragdollJoint.driveAngularStiffness = Drive.getFloat("angularStiffness", 0.0f);
            ragdollJoint.driveAngularDamping = Drive.getFloat("angularDamping", 0.0f);

            // Per axis linear drive amount
            ragdollJoint.driveLinearAmount = Drive.getVector("linearAmount");

            // Drive caps and space
            ragdollJoint.driveMaxLinearForce = Drive.getFloat("maxLinearForce", -1.0f);    // -1 = infinite
            ragdollJoint.driveMaxAngularForce = Drive.getFloat("maxAngularForce", -1.0f);  // -1 = infinite
            ragdollJoint.driveWorldspace = Drive.getBoolean("worldspace", false);
            if (Drive.has("target")) {
                ragdollJoint.driveTarget = Drive.getMatrix("target");
            }
            ragdollJoint.ignoreMass = MarkerUI.getBoolean("ignoreMass", false);

            // Joint flags
            if (Joint.has("disableCollision")) {
                ragdollJoint.disableCollision = Joint.getBoolean("disableCollision");
            }
            if (Joint.has("ignoreMass")) {
                ragdollJoint.ignoreMass = Joint.getBoolean("ignoreMass");
            }

            // Linear limits
            float limitLinearX = -1;
            float limitLinearY = -1;
            float limitLinearZ = -1;
            if (Limit.has("x")) limitLinearX = Limit.getFloat("x");
            if (Limit.has("y")) limitLinearY = Limit.getFloat("y");
            if (Limit.has("z")) limitLinearZ = Limit.getFloat("z");
            ragdollJoint.limitLinear = RdVectorF(limitLinearX, limitLinearY, limitLinearZ);
        }
    }

    uint64_t SpawnRagdoll(glm::vec3 position, glm::vec3 eulerRotation, const std::string& ragdollName) {
        Logging::Function() << "RagdollManager::SpawnRagdoll()";
        uint64_t ragdollId = UniqueID::GetNextObjectId(ObjectType::RAGDOLL_V2);

        RagdollV2& ragdoll = g_ragdolls[ragdollId] = RagdollV2();
        ragdoll.Init(position, eulerRotation, ragdollName, ragdollId);
        Logging::Debug() << "Created ragdoll '" << ragdollName << "' at " << position << " with id '" << ragdollId << "'";

        return ragdollId;
    }

    void AddForce(uint64_t physicsId, glm::vec3 force) {
        // diirity. fix mee
        for (auto it = g_ragdolls.begin(); it != g_ragdolls.end(); ) {
            RagdollV2& ragdoll = it->second;
            ragdoll.AddForce(physicsId, force);
            it++;
        }
    }

    RagdollInfo* GetRagdollInfoByName(const std::string& filename) {
        auto it = g_ragdollInfoSet.find(filename);
        return it != g_ragdollInfoSet.end() ? &it->second : nullptr;
    }

    RagdollV2* GetRagdollV2ById(uint64_t ragdollId) {
        for (auto it = g_ragdolls.begin(); it != g_ragdolls.end(); ) {
            RagdollV2& ragdoll = it->second;   
            if (ragdoll.GetRagdollId() == ragdollId) {
                return &ragdoll;
            }
            it++;
        }

        Logging::Error() << "RagdollManager::GetRagdollById() failed to get by id '" << ragdollId << "'";
        return nullptr;
    }

    std::unordered_map<uint64_t, RagdollV2>& GetRagdolls() {
        return g_ragdolls;
    }
}