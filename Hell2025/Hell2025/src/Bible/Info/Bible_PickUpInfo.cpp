#include "Bible/Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {

    void InitPickUpInfo() {
        PickUpInfo& aks74u = CreatePickUpInfo("AKS74U");
        aks74u.modelName = "Weapon_AKS74U";
        aks74u.convexMeshName = "CollisionMesh_AKS74U";
        aks74u.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        aks74u.type = PickUpType::WEAPON;
        aks74u.mass = 2.7f;

        PickUpInfo& blackSkull = CreatePickUpInfo("BlackSkull");
        blackSkull.modelName = "Item_BlackSkull";
        blackSkull.convexMeshName = "CollisionMesh_BlackSkull";
        blackSkull.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        blackSkull.type = PickUpType::USELESS;
        blackSkull.mass = 2.7f;

        PickUpInfo& glock = CreatePickUpInfo("Glock");
        glock.modelName = "Weapon_Glock";
        glock.convexMeshName = "CollisionMesh_Glock";
        glock.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        glock.type = PickUpType::WEAPON;
        glock.mass = 3.3f;

        PickUpInfo& goldenGlock = CreatePickUpInfo("GoldenGlock");
        goldenGlock.modelName = "Weapon_GoldenGlock";
        goldenGlock.convexMeshName = "CollisionMesh_GoldenGlock";
        goldenGlock.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        goldenGlock.type = PickUpType::WEAPON;
        goldenGlock.mass = 3.3f;

        PickUpInfo& knife = CreatePickUpInfo("Knife");
        knife.modelName = "Weapon_Knife";
        knife.convexMeshName = "CollisionMesh_Knife";
        knife.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        knife.type = PickUpType::WEAPON;
        knife.mass = 0.5f;

        PickUpInfo& remington870 = CreatePickUpInfo("Remington870");
        remington870.modelName = "Weapon_Remington870";
        remington870.convexMeshName = "CollisionMesh_Remington870";
        remington870.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        remington870.type = PickUpType::WEAPON;
        remington870.mass = 3.2f;

        PickUpInfo& spas = CreatePickUpInfo("SPAS");
        spas.modelName = "Weapon_SPAS";
        spas.convexMeshName = "CollisionMesh_SPAS";
        spas.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        spas.type = PickUpType::WEAPON;
        spas.mass = 3.3f;

        PickUpInfo& tokarev = CreatePickUpInfo("Tokarev");
        tokarev.modelName = "Weapon_Tokarev";
        tokarev.convexMeshName = "CollisionMesh_Tokarev";
        tokarev.physicsShapeType = PhysicsShapeType::CONVEX_MESH;
        tokarev.type = PickUpType::WEAPON;
        tokarev.mass = 0.7f;

        PickUpInfo& shottyBuckShotBox = CreatePickUpInfo("12GaugeBuckShot");
        shottyBuckShotBox.modelName = "Ammo_ShotgunBox";
        shottyBuckShotBox.physicsShapeType = PhysicsShapeType::BOX;
        shottyBuckShotBox.type = PickUpType::AMMO;
        shottyBuckShotBox.mass = 0.45f;

        PickUpInfo& shottySlugBox = CreatePickUpInfo("12GaugeSlug");
        shottySlugBox.modelName = "Ammo_ShotgunBox";
        shottySlugBox.physicsShapeType = PhysicsShapeType::BOX;
        shottySlugBox.type = PickUpType::AMMO;
        shottySlugBox.mass = 0.45f;
    }

    float GetPickUpMass(const std::string& name) {
        if (PickUpInfo* pickUpInfo = GetPickUpInfoByName(name)) {
            return pickUpInfo->mass;
        }
        return 0;
    }

    PickUpType GetPickUpTypeByName(const std::string& name) {
        if (PickUpInfo* pickUpInfo = GetPickUpInfoByName(name)) {
            return pickUpInfo->type;
        }
        return PickUpType::UNDEFINED;
    }
}