#include "Bible/Bible.h"
#include "HellLogging.h"

namespace Bible {

    void ConfigureMeshNodesByPickUpName(uint64_t id, const std::string& pickUpName, MeshNodes& meshNodes, bool createPhysicsObjects) {
        PickUpInfo* pickUpInfo = Bible::GetPickUpInfoByName(pickUpName);
        if (!pickUpInfo) {
            Logging::Error() << "Bible::ConfigureMeshNodesByPickUpName(..) failed: '" << pickUpName << "' PickUpInfo not found in Bible\n";
            return;
        }

        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        PhysicsFilterData pickUpFilterData;
        pickUpFilterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
        pickUpFilterData.collisionGroup = CollisionGroup::ITEM_PICK_UP;
        pickUpFilterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

        // AKS74U
        if (pickUpInfo->name == "AKS74U") {
            MeshNodeCreateInfo& receiver = meshNodeCreateInfoSet.emplace_back();
            receiver.meshName = "AKS74UReceiver";
            receiver.materialName = "AKS74U_1";
            if (createPhysicsObjects) {
                receiver.rigidDynamic.createObject = true;
                receiver.rigidDynamic.kinematic = false;
                receiver.rigidDynamic.offsetTransform = Transform();
                receiver.rigidDynamic.filterData = pickUpFilterData;
                receiver.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                receiver.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                receiver.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

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

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Black Skull
        if (pickUpInfo->name == "BlackSkull") {
            MeshNodeCreateInfo& blackSkull = meshNodeCreateInfoSet.emplace_back();
            blackSkull.meshName = "BlackSkull";
            blackSkull.materialName = "BlackSkull";
            if (createPhysicsObjects) {
                blackSkull.rigidDynamic.createObject = true;
                blackSkull.rigidDynamic.kinematic = false;
                blackSkull.rigidDynamic.offsetTransform = Transform();
                blackSkull.rigidDynamic.filterData = pickUpFilterData;
                blackSkull.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                blackSkull.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                blackSkull.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }


        // Glock
        if (pickUpInfo->name == "Glock") {
            MeshNodeCreateInfo& glock = meshNodeCreateInfoSet.emplace_back();
            glock.meshName = "Glock";
            glock.materialName = "Glock";
            if (createPhysicsObjects) {
                glock.rigidDynamic.createObject = true;
                glock.rigidDynamic.kinematic = false;
                glock.rigidDynamic.offsetTransform = Transform();
                glock.rigidDynamic.filterData = pickUpFilterData;
                glock.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                glock.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                glock.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Knife
        if (pickUpInfo->name == "Knife") {
            MeshNodeCreateInfo& knife = meshNodeCreateInfoSet.emplace_back();
            knife.meshName = "Knife";
            knife.materialName = "Knife";
            if (createPhysicsObjects) {
                knife.rigidDynamic.createObject = true;
                knife.rigidDynamic.kinematic = false;
                knife.rigidDynamic.offsetTransform = Transform();
                knife.rigidDynamic.filterData = pickUpFilterData;
                knife.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                knife.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                knife.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Golden Glock
        if (pickUpInfo->name == "GoldenGlock") {
            MeshNodeCreateInfo& glock = meshNodeCreateInfoSet.emplace_back();
            glock.meshName = "GoldenGlock";
            glock.materialName = "GlockGold";
            if (createPhysicsObjects) {
                glock.rigidDynamic.createObject = true;
                glock.rigidDynamic.kinematic = false;
                glock.rigidDynamic.offsetTransform = Transform();
                glock.rigidDynamic.filterData = pickUpFilterData;
                glock.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                glock.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                glock.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Remington 870
        if (pickUpInfo->name == "Remington870") {
            MeshNodeCreateInfo& shotgun = meshNodeCreateInfoSet.emplace_back();
            shotgun.meshName = "Remington870";
            shotgun.materialName = "Shotgun";
            if (createPhysicsObjects) {
                shotgun.rigidDynamic.createObject = true;
                shotgun.rigidDynamic.kinematic = false;
                shotgun.rigidDynamic.offsetTransform = Transform();
                shotgun.rigidDynamic.filterData = pickUpFilterData;
                shotgun.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                shotgun.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                shotgun.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // SPAS
        if (pickUpInfo->name == "SPAS") {
            MeshNodeCreateInfo& main = meshNodeCreateInfoSet.emplace_back();
            main.meshName = "SPAS12_Main";
            main.materialName = "SPAS2_Main";
            if (createPhysicsObjects) {
                main.rigidDynamic.createObject = true;
                main.rigidDynamic.kinematic = false;
                main.rigidDynamic.offsetTransform = Transform();
                main.rigidDynamic.filterData = pickUpFilterData;
                main.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                main.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                main.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            MeshNodeCreateInfo& moving = meshNodeCreateInfoSet.emplace_back();
            moving.meshName = "SPAS12_Moving";
            moving.materialName = "SPAS2_Moving";

            MeshNodeCreateInfo& stamped = meshNodeCreateInfoSet.emplace_back();
            stamped.meshName = "SPAS12_Stamped";
            stamped.materialName = "SPAS2_Stamped";

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Shotty Buckshot Box
        if (pickUpInfo->name == "12GaugeBuckShot") {
            MeshNodeCreateInfo& ammo = meshNodeCreateInfoSet.emplace_back();
            ammo.meshName = "Ammo_ShotgunBox";
            ammo.materialName = "Shotgun_AmmoBox";
            if (createPhysicsObjects) {
                ammo.rigidDynamic.createObject = true;
                ammo.rigidDynamic.kinematic = false;
                ammo.rigidDynamic.offsetTransform = Transform();
                ammo.rigidDynamic.filterData = pickUpFilterData;
                ammo.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                ammo.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Shotty Slug Box
        if (pickUpInfo->name == "12GaugeSlug") {
            MeshNodeCreateInfo& ammo = meshNodeCreateInfoSet.emplace_back();
            ammo.meshName = "Ammo_ShotgunBox";
            ammo.materialName = "Shotgun_AmmoBoxSlug";
            if (createPhysicsObjects) {
                ammo.rigidDynamic.createObject = true;
                ammo.rigidDynamic.kinematic = false;
                ammo.rigidDynamic.offsetTransform = Transform();
                ammo.rigidDynamic.filterData = pickUpFilterData;
                ammo.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                ammo.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
            }

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        // Tokarev
        if (pickUpInfo->name == "Tokarev") {
            MeshNodeCreateInfo& body = meshNodeCreateInfoSet.emplace_back();
            body.meshName = "TokarevBody";
            body.materialName = "Tokarev";
            if (createPhysicsObjects) {
                body.rigidDynamic.createObject = true;
                body.rigidDynamic.kinematic = false;
                body.rigidDynamic.offsetTransform = Transform();
                body.rigidDynamic.filterData = pickUpFilterData;
                body.rigidDynamic.mass = Bible::GetPickUpMass(pickUpName);
                body.rigidDynamic.shapeType = pickUpInfo->physicsShapeType;
                body.rigidDynamic.convexMeshModelName = pickUpInfo->convexMeshName;
            }

            MeshNodeCreateInfo& grip = meshNodeCreateInfoSet.emplace_back();
            grip.meshName = "TokarevGripPolymer";
            grip.materialName = "TokarevGrip";

            meshNodes.Init(id, pickUpInfo->modelName, meshNodeCreateInfoSet);
            return;
        }

        Logging::Error() << "Bible::ConfigureMeshNodesByPickUpName(..) failed: '" << pickUpName << "' not implemented\n";
    }
}