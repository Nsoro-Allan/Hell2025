#include "Physics.h"
#include "HellConstants.h"
#include "HellEnums.h"
#include "HellLogging.h"
#include <iostream>

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    if (filterData0.word2 == CollisionGroup::NO_COLLISION) {
        return PxFilterFlag::eKILL;
    }
    else if ((filterData0.word2 & filterData1.word1) && (filterData1.word2 & filterData0.word1)) {
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        return PxFilterFlag::eDEFAULT;
    }

    return PxFilterFlag::eKILL;
}

class UserErrorCallback : public physx::PxErrorCallback {
public:
    virtual void reportError(physx::PxErrorCode::Enum /*code*/, const char* message, const char* file, int line) {
        std::cout << file << " line " << line << ": " << message << "\n";
        std::cout << "\n";
    }
} gErrorCallback;

namespace Physics {

    PxPhysics* g_physics = NULL;
    PxScene* g_scene = NULL;
    PxFoundation* g_foundation;
    PxDefaultAllocator g_allocator;
    PxDefaultCpuDispatcher* g_dispatcher = NULL;
    PxPvd* g_pvd = NULL;
    PxMaterial* g_defaultMaterial = NULL;
    PxMaterial* g_grassMaterial = NULL;
    PxControllerManager* g_characterControllerManager;
    std::vector<CollisionReport> g_collisionReports;
    std::vector<CharacterCollisionReport> g_characterCollisionReports;
    ContactReportCallback g_contactReportCallback;
    CCTHitCallback g_cctHitCallback;

    #define PVD_HOST "127.0.0.1"

    void Init() {
        Logging::Init() << "Physics::Init()";

        g_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator, gErrorCallback);
        g_pvd = physx::PxCreatePvd(*g_foundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        g_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        g_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_foundation, physx::PxTolerancesScale(), true, g_pvd);
        physx::PxSceneDesc sceneDesc(g_physics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        g_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = g_dispatcher;
        sceneDesc.filterShader = contactReportFilterShader;
        sceneDesc.simulationEventCallback = &g_contactReportCallback;

        g_scene = g_physics->createScene(sceneDesc);
        g_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
        g_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);

        physx::PxPvdSceneClient* pvdClient = g_scene->getScenePvdClient();
        if (pvdClient) {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
        g_defaultMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.6f);
        g_grassMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.6f);
        //g_grassMaterial = g_physics->createMaterial(0.8f, 0.7f, 0.05f);

        // Character controller shit
        g_characterControllerManager = PxCreateControllerManager(*g_scene);

        // temporary ground plane
        PxRigidStatic* groundPlane = NULL;
        PxShape* groundShape = NULL;

        groundPlane = PxCreatePlane(*g_physics, PxPlane(0, 1, 0, 0.01f), *g_defaultMaterial);
        g_scene->addActor(*groundPlane);
        groundPlane->getShapes(&groundShape, 1);
        PxFilterData filterData;
        filterData.word0 = RaycastGroup::RAYCAST_ENABLED; // must be disabled or it causes crash in scene::update when it tries to retrieve rigid body flags from this actor
        filterData.word1 = CollisionGroup::ENVIROMENT_OBSTACLE;
        filterData.word2 = CollisionGroup::BULLET_CASING | CollisionGroup::GENERIC_BOUNCEABLE | CollisionGroup::CHARACTER_CONTROLLER | CollisionGroup::RAGDOLL_ENEMY;
        groundShape->setQueryFilterData(filterData);
        groundShape->setSimulationFilterData(filterData); // sim is for ragz

        PhysicsUserData userData;
        userData.physicsId = 0;
        userData.objectId = 0;
        userData.physicsType = PhysicsType::GROUND_PLANE;
        userData.objectType = ObjectType::NO_TYPE;
        groundPlane->userData = new PhysicsUserData(userData);

        // This might not work!
        PxScene* pxScene = GetPxScene();
        pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
        pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
        pxScene->setFlag(PxSceneFlag::eENABLE_STABILIZATION, true);
        // This may not work!

        LoadRagdollsFromDisk();
    }

    void BeginFrame() {
        RemoveAnyHeightFieldMarkedForRemoval();
        RemoveAnyRagdollsMarkedForRemoval();
        RemoveAnyD6JointMarkedForRemoval();
        RemoveAnyRigidDynamicMarkedForRemoval();
        RemoveAnyRigidStaticMarkedForRemoval();
    }

    void StepPhysics(float deltaTime) {
        ClearCollisionReports();
        g_scene->simulate(deltaTime);
        g_scene->fetchResults(true);
    }

    void ForceZeroStepUpdate() {
        g_scene->simulate(0.0f, nullptr);
        //pxScene->flushQueryUpdates();
        g_scene->fetchResults(true);
    }


    void AddCollisionReport(CollisionReport& collisionReport) {
        g_collisionReports.push_back(collisionReport);
    }

    void ClearCollisionReports() {
        g_collisionReports.clear();
    }

    void ClearCharacterControllerCollsionReports() {
        g_characterCollisionReports.clear();
    }

    std::vector<CollisionReport>& GetCollisionReports() {
        return g_collisionReports;
    }

    std::vector<CharacterCollisionReport>& GetCharacterCollisionReports() {
        return g_characterCollisionReports;
    }

    PxMaterial* GetDefaultMaterial() {
        return g_defaultMaterial;
    }

    PxMaterial* GetGrassMaterial() {
        return g_grassMaterial;
    }

    PxPhysics* GetPxPhysics() {
        return g_physics;
    }

    PxScene* GetPxScene() {
        return g_scene;
    }

    CCTHitCallback& GetCharacterControllerHitCallback() {
        return g_cctHitCallback;
    }

    PxControllerManager* GetCharacterControllerManager() {
        return g_characterControllerManager;
    }

    void PrintSceneInfo() {
        std::cout << "\n";
        std::cout << " **************************\n";
        std::cout << " *                        *\n";
        std::cout << " *    PhysX Scene Info    *\n";
        std::cout << " *                        *\n";
        std::cout << " **************************\n";
        std::cout << "\n";

        PrintSceneRigidInfo();
        PrintSceneD6JointInfo();
        PrintSceneRagdollInfo();
    }
}

void CCTHitCallback::onShapeHit(const PxControllerShapeHit& hit) {
    CharacterCollisionReport report;
    report.hitNormal = Physics::PxVec3toGlmVec3(hit.worldNormal);
    report.worldPosition = Physics::PxVec3toGlmVec3(hit.worldPos);
    report.characterController = hit.controller;
    report.hitShape = hit.shape;
    report.hitActor = hit.actor;
    Physics::GetCharacterCollisionReports().push_back(report);
}

void CCTHitCallback::onControllerHit(const PxControllersHit& hit) {
}

void CCTHitCallback::onObstacleHit(const PxControllerObstacleHit& hit) {
}


