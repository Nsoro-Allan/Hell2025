#include "Bible/Bible.h"
#include "HellLogging.h"

namespace Bible {

    void ConfigureMeshNodesByPickUpName(uint64_t id, const std::string& pickUpName, MeshNodes& meshNodes, bool createPhysicsObjects) {

        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        PhysicsFilterData pickUpFilterData;
        pickUpFilterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
        pickUpFilterData.collisionGroup = CollisionGroup::ITEM_PICK_UP;
        pickUpFilterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

        // AKS74U
        if (pickUpName == "AKS74U") {
            MeshNodeCreateInfo& receiver = meshNodeCreateInfoSet.emplace_back();
            receiver.meshName = "AKS74UReceiver";
            receiver.materialName = "AKS74U_1";
            receiver.rigidDynamic.createObject = true;
            receiver.rigidDynamic.kinematic = false;
            receiver.rigidDynamic.offsetTransform = Transform();
            receiver.rigidDynamic.filterData = pickUpFilterData;
            receiver.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            receiver.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            receiver.rigidDynamic.convexMeshModelName = "CollisionMesh_AKS74U";

            MeshNodeCreateInfo& barrel = meshNodeCreateInfoSet.emplace_back();
            barrel.meshName = "AKS74UBarrel";
            barrel.materialName = "AKS74U_4";

            MeshNodeCreateInfo& bolt = meshNodeCreateInfoSet.emplace_back();
            bolt.meshName = "AKS74UBolt";
            bolt.materialName = "AKS74U_1";

            MeshNodeCreateInfo& handGuard = meshNodeCreateInfoSet.emplace_back();
            handGuard.meshName = "AKS74UHandGuard";
            handGuard.materialName = "AKS74U_0";

            MeshNodeCreateInfo& mag = meshNodeCreateInfoSet.emplace_back();
            mag.meshName = "AKS74UMag";
            mag.materialName = "AKS74U_3";
            
            MeshNodeCreateInfo& pistolGrip = meshNodeCreateInfoSet.emplace_back();
            pistolGrip.meshName = "AKS74UPistolGrip";
            pistolGrip.materialName = "AKS74U_2";

            meshNodes.Init(id, "Weapon_AKS74U", meshNodeCreateInfoSet);
            return;
        }

        // Glock
        if (pickUpName == "Glock") {
            MeshNodeCreateInfo& glock = meshNodeCreateInfoSet.emplace_back();
            glock.meshName = "Glock";
            glock.materialName = "Glock";
            glock.rigidDynamic.createObject = true;
            glock.rigidDynamic.kinematic = false;
            glock.rigidDynamic.offsetTransform = Transform();
            glock.rigidDynamic.filterData = pickUpFilterData;
            glock.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            glock.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            glock.rigidDynamic.convexMeshModelName = "CollisionMesh_Glock";

            meshNodes.Init(id, "Weapon_Glock", meshNodeCreateInfoSet);
            return;
        }

        // Golden Glock
        if (pickUpName == "GoldenGlock") {
            MeshNodeCreateInfo& glock = meshNodeCreateInfoSet.emplace_back();
            glock.meshName = "GoldenGlock";
            glock.materialName = "GlockGold";
            glock.rigidDynamic.createObject = true;
            glock.rigidDynamic.kinematic = false;
            glock.rigidDynamic.offsetTransform = Transform();
            glock.rigidDynamic.filterData = pickUpFilterData;
            glock.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            glock.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            glock.rigidDynamic.convexMeshModelName = "CollisionMesh_GoldenGlock";

            meshNodes.Init(id, "Weapon_GoldenGlock", meshNodeCreateInfoSet);
            return;
        }

        // Remington 870
        if (pickUpName == "Remington870") {
            MeshNodeCreateInfo& shotgun = meshNodeCreateInfoSet.emplace_back();
            shotgun.meshName = "Remington870";
            shotgun.materialName = "Shotgun";
            shotgun.rigidDynamic.createObject = true;
            shotgun.rigidDynamic.kinematic = false;
            shotgun.rigidDynamic.offsetTransform = Transform();
            shotgun.rigidDynamic.filterData = pickUpFilterData;
            shotgun.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            shotgun.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            shotgun.rigidDynamic.convexMeshModelName = "CollisionMesh_Remington870";

            meshNodes.Init(id, "Weapon_Remington870", meshNodeCreateInfoSet);
            return;
        }

        // SPAS
        if (pickUpName == "SPAS") {
            MeshNodeCreateInfo& main = meshNodeCreateInfoSet.emplace_back();
            main.meshName = "SPAS12_Main";
            main.materialName = "SPAS2_Main";
            main.rigidDynamic.createObject = true;
            main.rigidDynamic.kinematic = false;
            main.rigidDynamic.offsetTransform = Transform();
            main.rigidDynamic.filterData = pickUpFilterData;
            main.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            main.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            main.rigidDynamic.convexMeshModelName = "CollisionMesh_SPAS";

            MeshNodeCreateInfo& moving = meshNodeCreateInfoSet.emplace_back();
            moving.meshName = "SPAS12_Moving";
            moving.materialName = "SPAS2_Moving";

            MeshNodeCreateInfo& stamped = meshNodeCreateInfoSet.emplace_back();
            stamped.meshName = "SPAS12_Stamped";
            stamped.materialName = "SPAS2_Stamped";

            meshNodes.Init(id, "Weapon_SPAS", meshNodeCreateInfoSet);
            return;
        }

        // Shotty Buckshot Box
        if (pickUpName == "12GaugeBuckShot") {
            MeshNodeCreateInfo& ammo = meshNodeCreateInfoSet.emplace_back();
            ammo.meshName = "Ammo_ShotgunBox";
            ammo.materialName = "Shotgun_AmmoBox";
            ammo.rigidDynamic.createObject = true;
            ammo.rigidDynamic.kinematic = false;
            ammo.rigidDynamic.offsetTransform = Transform();
            ammo.rigidDynamic.filterData = pickUpFilterData;
            ammo.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            ammo.rigidDynamic.shapeType = PhysicsShapeType::BOX;

            meshNodes.Init(id, "Ammo_ShotgunBox", meshNodeCreateInfoSet);
            return;
        }

        // Shotty Slug Box
        if (pickUpName == "12GaugeSlug") {
            MeshNodeCreateInfo& ammo = meshNodeCreateInfoSet.emplace_back();
            ammo.meshName = "Ammo_ShotgunBox";
            ammo.materialName = "Shotgun_AmmoBoxSlug";
            ammo.rigidDynamic.createObject = true;
            ammo.rigidDynamic.kinematic = false;
            ammo.rigidDynamic.offsetTransform = Transform();
            ammo.rigidDynamic.filterData = pickUpFilterData;
            ammo.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            ammo.rigidDynamic.shapeType = PhysicsShapeType::BOX;

            meshNodes.Init(id, "Ammo_ShotgunBox", meshNodeCreateInfoSet);
            return;
        }

        // Tokarev
        if (pickUpName == "Tokarev") {
            MeshNodeCreateInfo& body = meshNodeCreateInfoSet.emplace_back();
            body.meshName = "Tokarev_Body";
            body.materialName = "Tokarev";
            body.rigidDynamic.createObject = true;
            body.rigidDynamic.kinematic = false;
            body.rigidDynamic.offsetTransform = Transform();
            body.rigidDynamic.filterData = pickUpFilterData;
            body.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
            body.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
            body.rigidDynamic.convexMeshModelName = "CollisionMesh_Tokarev";

            MeshNodeCreateInfo& grip = meshNodeCreateInfoSet.emplace_back();
            grip.meshName = "TokarevGripPolymer";
            grip.materialName = "TokarevGrip";

            meshNodes.Init(id, "Weapon_Tokarev", meshNodeCreateInfoSet);
            return;
        }

        Logging::Error() << "Bible::ConfigureMeshNodesByPickUpName(..) failed: '" << pickUpName << "' not implemented\n";
    }
}