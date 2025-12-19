#include "Bible/Bible.h"
#include "HellLogging.h"

namespace Bible {

    void ConfigureMeshNodesByPickUpName(uint64_t id, const std::string& pickUpName, MeshNodes& meshNodes, bool createPhysicsObjects) {

        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        PhysicsFilterData pickUpFilterData;
        pickUpFilterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
        pickUpFilterData.collisionGroup = CollisionGroup::ITEM_PICK_UP;
        pickUpFilterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

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
        if (pickUpName == "ShotgunBuckShotBox") {
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
        if (pickUpName == "ShotgunSlugBox") {
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

        Logging::Error() << "Bible::ConfigureMeshNodesByPickUpName(..) failed: '" << pickUpName << "' not implemented\n";
    }
}