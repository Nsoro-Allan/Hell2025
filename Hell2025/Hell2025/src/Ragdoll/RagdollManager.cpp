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
    std::vector<RagdollV2> g_ragdolls;
    MeshBuffer g_meshBuffer;

    void LoadFile(const FileInfo& fileInfo);
    void LoadMarkers(RagdollInfo& ragdoll, rapidjson::Document& doc);
    void LoadJoints(RagdollInfo& ragdoll, rapidjson::Document& doc);
    void LoadSolver(RagdollInfo& ragdoll, rapidjson::Document& doc);

    void Init() {
        for (FileInfo& fileInfo : Util::IterateDirectory("res/", { "rag" })) {
            LoadFile(fileInfo);
        }
        SpawnRagdoll(glm::vec3(36, 31, 36), glm::vec3(0.0f, 0.2f, 0.0f), "manikin");
        SpawnRagdoll(glm::vec3(37, 31, 36), glm::vec3(0.0f, -0.4f, 0.0f), "dobermann");

        std::cout << g_ragdolls.size() << " ragdolls\n";
        for (RagdollV2& ragdoll : g_ragdolls) {
            std::cout << "'" << ragdoll.GetRagdollName() << "'\n";
        }

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

        //if (fileInfo.name == "dobermann") {
        //    for (RagdollMarker& marker : ragdollInfo.m_markers) {
        //        ragdollInfo.PrintMarkerInfo(marker);
        //    }
        //}
        //ragdollInfo.PrintSolverInfo();
        //Logging::Debug() << "Loaded " << fileInfo.path << "\n";
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

            JsonComponent NameComponent = registry.get(entity, "NameComponent");
            RdString name = NameComponent.getString("value");
            JsonComponent ScaleComponent = registry.get(entity, "ScaleComponent");
            JsonComponent RestComponent = registry.get(entity, "RestComponent");
            RdMatrix mtx = RestComponent.getMatrix("matrix");

            RdMatrix originMtx{ RdIdentityInit };
            if (registry.has(entity, "OriginComponent")) {
                auto OriginComponent = registry.get(entity, "OriginComponent");
                originMtx = OriginComponent.getMatrix("matrix");
            }
            else {
                // Otherwise it's safe to assume  the rest matrix
                originMtx = mtx;
            }

            JsonComponent geometryDescription = registry.get(entity, "GeometryDescriptionComponent");
            JsonComponent convexMesh = registry.get(entity, "ConvexMeshComponents");
            JsonComponent color = registry.get(entity, "ColorComponent");
            JsonComponent subs = registry.get(entity, "SubEntitiesComponent");
            RdString relEntity = subs.getEntity("relative");
            JsonComponent limit = registry.get(relEntity, "LimitComponent");
            JsonComponent rigid = registry.get(entity, "RigidComponent");
            JsonComponent drive = registry.get(relEntity, "DriveComponent");
            JsonComponent joint = registry.get(relEntity, "JointComponent");
            JsonComponent markerUI = registry.get(entity, "MarkerUIComponent");
            JsonComponent sceneComponent = registry.get(entity, "SceneComponent");

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

            if (geometryDescription.has("convexDecomposition")) Logging::ToDo() << "You haven't implemented convexDecomposition parsing, apparently you need it";
             
            if (isKinematic) {
                Logging::Debug() << "Skipping ground marker";
                continue;
            }

            RagdollMarker& ragdollMarker = ragdoll.m_markers.emplace_back();
            ragdollMarker.name = name;
            ragdollMarker.inputMatrix = mtx;
            ragdollMarker.originMatrix = originMtx;
            ragdollMarker.parentFrame = joint.getMatrix("parentFrame");
            ragdollMarker.childFrame = joint.getMatrix("childFrame");
            ragdollMarker.shapeType = geometryDescription.getString("type");
            ragdollMarker.limitRange = { limit.getFloat("twist"), limit.getFloat("swing1"), limit.getFloat("swing2") };
            ragdollMarker.extents = geometryDescription.getVector("extents");
            ragdollMarker.shapeOffset = geometryDescription.getVector("offset");
            ragdollMarker.shapeRadius = geometryDescription.getFloat("radius");
            ragdollMarker.shapeLength = geometryDescription.getFloat("length");
            ragdollMarker.shapeRotation = RdToEulerRotation(geometryDescription.getQuaternion("rotation"));
            ragdollMarker.convexMeshVertices = convexMesh.getPoints("vertices");
            ragdollMarker.convexMeshIndices = convexMesh.getUints("indices");
            ragdollMarker.driveSlerp = drive.getBoolean("slerp");
            ragdollMarker.driveSpringType = (int)drive.getBoolean("acceleration");
            ragdollMarker.mass = markerUI.getFloat("mass");
            ragdollMarker.linearStiffness = markerUI.getFloat("linearStiffness");
            ragdollMarker.linearDampingRatio = markerUI.getFloat("linearDampingRatio");
            ragdollMarker.angularStiffness = markerUI.getFloat("angularStiffness");
            ragdollMarker.angularDampingRatio = markerUI.getFloat("angularDampingRatio");
            ragdollMarker.inputType = toInputType(markerUI.getString("inputType"));
            ragdollMarker.collisionGroup = markerUI.getInteger("collisionGroup");
            ragdollMarker.useRootStiffness = markerUI.getBoolean("useRootStiffness", false);
            ragdollMarker.useLinearAngularStiffness = markerUI.getBoolean("useLinearAngularStiffness");
            ragdollMarker.color = color.getColor("value");
            ragdollMarker.contactStiffness = contactStiffness;
            ragdollMarker.contactDamping = contactDamping;
            ragdollMarker.friction = friction;
            ragdollMarker.restitution = restitution;
            ragdollMarker.isKinematic = isKinematic;
            ragdollMarker.enableCCD = enableCCD;

            const std::string src = markerUI.getString("sourceTransform");
            const std::vector<std::string> dst = markerUI.getStrings("destinationTransforms");

            std::string chosenPath;
            if (!src.empty()) {
                chosenPath = src;
            }
            else if (!dst.empty()) {
                // Pick the longest (and usually deepest) destination path
                chosenPath = *std::max_element(
                    dst.begin(), dst.end(),
                    [](const std::string& a, const std::string& b) { return a.size() < b.size(); });
            }

            sanitizePath(chosenPath);

            ragdollMarker.bonePath = chosenPath;
            ragdollMarker.boneName = lastSegment(chosenPath);
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

            // Joint ub-entity that carries Joint/Limit/Drive data
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
            RdMotion linearMotion = StringToMotion(MarkerUI.getString("linearMotion"));

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
            ragdollJoint.linearMotion = linearMotion;
            ragdollJoint.linearMotionString = MarkerUI.getString("linearMotion");
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

    void SpawnRagdoll(glm::vec3 position, glm::vec3 eulerRotation, const std::string& ragdollName) {
        RagdollV2& ragdoll = g_ragdolls.emplace_back();
        ragdoll.Init(position, eulerRotation, ragdollName);
    }

    void AddForce(uint64_t physicsId, glm::vec3 force) {
        for (RagdollV2& ragdoll : g_ragdolls) {
            ragdoll.AddForce(physicsId, force);
        }
    }

    RagdollInfo* RagdollManager::GetRagdollInfoByName(const std::string& filename) {
        auto it = g_ragdollInfoSet.find(filename);
        return it != g_ragdollInfoSet.end() ? &it->second : nullptr;
    }

    RagdollV2* GetRagdollById(uint64_t ragdollId) {
        //std::cout << "Looking for " << ragdollId << "\n";
        for (RagdollV2& ragdoll : g_ragdolls) {
            //std::cout << "-" << ragdoll.GetRagdollId() << "\n";
            if (ragdoll.GetRagdollId() == ragdollId) {
                return &ragdoll;
            }
        }
        return nullptr;
    }

    std::vector<RagdollV2>& GetRagdolls() { return g_ragdolls; }
}