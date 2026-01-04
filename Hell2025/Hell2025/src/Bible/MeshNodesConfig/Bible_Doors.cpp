#include "Bible/Bible.h"
#include "AssetManagement/AssetManager.h"

namespace Bible {

    void ConfigureDoorMeshNodes(uint64_t id, DoorCreateInfo& createInfo, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        // Deadlock stuff
        MeshNodeCreateInfo& deadLock = meshNodeCreateInfoSet.emplace_back();
        deadLock.meshName = "Door_Deadlock";
        deadLock.materialName = "DoorMetals";
        if (!createInfo.hasDeadLock) deadLock.blendingMode = BlendingMode::DO_NOT_RENDER;

        MeshNodeCreateInfo& deadLockSwitch = meshNodeCreateInfoSet.emplace_back();
        deadLockSwitch.meshName = "Door_DeadLockSwitch";
        deadLockSwitch.materialName = "DoorMetals";
        if (!createInfo.hasDeadLock) deadLockSwitch.blendingMode = BlendingMode::DO_NOT_RENDER;

        MeshNodeCreateInfo& frameDeadLock = meshNodeCreateInfoSet.emplace_back();
        frameDeadLock.meshName = "DoorFrame_Deadlock";
        frameDeadLock.materialName = "DoorMetals";
        if (!createInfo.hasDeadLock) frameDeadLock.blendingMode = BlendingMode::DO_NOT_RENDER;

        if (createInfo.type == DoorType::STANDARD_A) {

            // Collision mesh node
            MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
            door.meshName = "DoorOld_Sides";
            door.rigidDynamic.createObject = true;
            door.rigidDynamic.kinematic = true;
            door.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
            door.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
            door.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
            door.addtoNavMesh = true;

            // Openable mesh node
            MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
            hinges.meshName = "Door_Hinges";
            hinges.materialName = "Door_RE";
            hinges.openable.additionalTriggerMeshNames = {
                "DoorOld_Front",
                "DoorOld_Back",
                "DoorOld_Sides",
                "Door_Knob",
                "Door_Deadlock",
                "Door_DeadLockSwitch"
            };
            hinges.openable.isOpenable = true;
            hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
            hinges.openable.initialOpenState = OpenState::CLOSED;
            hinges.openable.minOpenValue = 0.0f;
            hinges.openable.maxOpenValue = createInfo.maxOpenValue;
            hinges.openable.openSpeed = 5.208f;
            hinges.openable.closeSpeed = 5.208f;
            hinges.openable.openingAudio = "Door_Open.wav";
            hinges.openable.closingAudio = "Door_Open.wav";
            hinges.openable.isDeadLock = true;

            meshNodes.Init(id, "Door_StandardA", meshNodeCreateInfoSet);

            // Front
            if (createInfo.materialTypeFront == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("DoorOld_Front", "Door_WP");
            }
            else if (createInfo.materialTypeFront == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("DoorOld_Front", "Door_RE");
            }

            // Back
            if (createInfo.materialTypeBack == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("DoorOld_Back", "Door_WP");
                meshNodes.SetMaterialByMeshName("DoorOld_Sides", "Door_WP");
            }
            else if (createInfo.materialTypeBack == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("DoorOld_Back", "Door_RE");
                meshNodes.SetMaterialByMeshName("DoorOld_Sides", "Door_RE");
            }
        }

        if (createInfo.type == DoorType::STANDARD_B) {

            // Collision mesh node
            MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
            door.meshName = "Door_Sides";
            door.rigidDynamic.createObject = true;
            door.rigidDynamic.kinematic = true;
            door.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
            door.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
            door.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
            door.addtoNavMesh = true;

            // Openable mesh node
            MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
            hinges.meshName = "Door_Hinges";
            hinges.openable.additionalTriggerMeshNames = {
                "Door_Front",
                "Door_Back",
                "Door_Sides",
                "Door_Knob",
                "Door_Deadlock",
            };
            hinges.openable.isOpenable = true;
            hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
            hinges.openable.initialOpenState = OpenState::CLOSED;
            hinges.openable.minOpenValue = 0.0f;
            hinges.openable.maxOpenValue = createInfo.maxOpenValue;
            hinges.openable.openSpeed = 5.208f;
            hinges.openable.closeSpeed = 5.208f;
            hinges.openable.openingAudio = "Door_Open.wav";
            hinges.openable.closingAudio = "Door_Open.wav";
            hinges.openable.isDeadLock = true;

            meshNodes.Init(id, "Door_StandardB", meshNodeCreateInfoSet);

            // Front
            if (createInfo.materialTypeFront == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("Door_Front", "Door_WP");
            }
            else if (createInfo.materialTypeFront == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("Door_Front", "Door_RE");
                meshNodes.SetMaterialByMeshName("Door_Sides", "Door_WP");
            }

            // Back
            if (createInfo.materialTypeBack == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("Door_Back", "Door_WP");
            }
            else if (createInfo.materialTypeBack == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("Door_Back", "Door_RE");
                meshNodes.SetMaterialByMeshName("Door_Sides", "Door_RE");
            }
        }

        if (createInfo.type == DoorType::STAINED_GLASS) {

            // Collision mesh node
            MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
            door.meshName = "DoorGlass_Sides";
            door.rigidDynamic.createObject = true;
            door.rigidDynamic.kinematic = true;
            door.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
            door.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
            door.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | ITEM_PICK_UP | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
            door.addtoNavMesh = true;

            // Openable mesh node
            MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
            hinges.meshName = "Door_Hinges";
            hinges.materialName = "Door_RE";
            hinges.openable.additionalTriggerMeshNames = {
                "DoorGlass_Front",
                "DoorGlass_Back",
                "DoorGlass_Sides",
                "Door_Knob",
                "Door_Deadlock",
                "DoorGlassArtFrame",
                "DoorGlass_Front",
                "DoorGlass_Back",
                "DoorGlass_Sides",
                "DoorGlass_Hinge_Sides",
                "Door_DeadLockSwitch",
                "DoorGlassArt_Clear",
                "DoorGlassArt_Green",
                "DoorGlassArt_Orange",
                "DoorGlassArt_Purple",
                "DoorGlassArt_Red",
                "DoorGlassArt_Yellow",
                "DoorGlassArt_CenterCircle"
            };
            hinges.openable.isOpenable = true;
            hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
            hinges.openable.initialOpenState = OpenState::CLOSED;
            hinges.openable.minOpenValue = 0.0f;
            hinges.openable.maxOpenValue = createInfo.maxOpenValue;
            hinges.openable.openSpeed = 5.208f;
            hinges.openable.closeSpeed = 5.208f;
            hinges.openable.openingAudio = "Door_Open.wav";
            hinges.openable.closingAudio = "Door_Open.wav";
            hinges.openable.isDeadLock = true;

            // Glass pieces
            MeshNodeCreateInfo& glassClear = meshNodeCreateInfoSet.emplace_back();
            glassClear.meshName = "DoorGlassArt_Clear";
            glassClear.materialName = "DoorGlass";
            glassClear.blendingMode = BlendingMode::STAINED_GLASS;
            glassClear.decalType = DecalType::GLASS;
            glassClear.tintColor = glm::vec3(1.0f, 1.0f, 1.0f);

            MeshNodeCreateInfo& glassGreen = meshNodeCreateInfoSet.emplace_back();
            glassGreen.meshName = "DoorGlassArt_Green";
            glassGreen.materialName = "DoorGlass";
            glassGreen.blendingMode = BlendingMode::STAINED_GLASS;
            glassGreen.decalType = DecalType::GLASS;
            glassGreen.tintColor = glm::vec3(0.0f, 0.95f, 0.5f);

            MeshNodeCreateInfo& glassOrange = meshNodeCreateInfoSet.emplace_back();
            glassOrange.meshName = "DoorGlassArt_Orange";
            glassOrange.materialName = "DoorGlass";
            glassOrange.blendingMode = BlendingMode::STAINED_GLASS;
            glassOrange.decalType = DecalType::GLASS;
            glassOrange.tintColor = glm::vec3(0.6f, 0.2f, 0.0f);

            MeshNodeCreateInfo& glassPurple = meshNodeCreateInfoSet.emplace_back();
            glassPurple.meshName = "DoorGlassArt_Purple";
            glassPurple.materialName = "DoorGlass";
            glassPurple.blendingMode = BlendingMode::STAINED_GLASS;
            glassPurple.decalType = DecalType::GLASS;
            glassPurple.tintColor = glm::vec3(0.15f, 0.0f, 0.5f);

            MeshNodeCreateInfo& glassRed = meshNodeCreateInfoSet.emplace_back();
            glassRed.meshName = "DoorGlassArt_Red";
            glassRed.materialName = "DoorGlass";
            glassRed.blendingMode = BlendingMode::STAINED_GLASS;
            glassRed.decalType = DecalType::GLASS;
            glassRed.tintColor = glm::vec3(0.95f, 0.0f, 0.0f);

            MeshNodeCreateInfo& glassYellow = meshNodeCreateInfoSet.emplace_back();
            glassYellow.meshName = "DoorGlassArt_Yellow";
            glassYellow.materialName = "DoorGlass";
            glassYellow.blendingMode = BlendingMode::STAINED_GLASS;
            glassYellow.decalType = DecalType::GLASS;
            glassYellow.tintColor = glm::vec3(1.00f, 0.8f, 0.25f);

            MeshNodeCreateInfo& glassCenterCircle = meshNodeCreateInfoSet.emplace_back();
            glassCenterCircle.meshName = "DoorGlassArt_CenterCircle";
            glassCenterCircle.materialName = "DoorGlass";
            glassCenterCircle.blendingMode = BlendingMode::STAINED_GLASS;
            glassCenterCircle.decalType = DecalType::GLASS;
            glassCenterCircle.tintColor = glm::vec3(0.5f, 0.35f, 0.0f);

            meshNodes.Init(id, "Door_StainedGlass", meshNodeCreateInfoSet);

            // Front
            if (createInfo.materialTypeFront == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("DoorGlass_Front", "Door_WP");
            }
            else if (createInfo.materialTypeFront == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("DoorGlass_Front", "Door_RE");
            }

            // Back
            if (createInfo.materialTypeBack == DoorMaterialType::WHITE_PAINT) {
                meshNodes.SetMaterialByMeshName("DoorGlass_Back", "Door_WP");
                meshNodes.SetMaterialByMeshName("DoorGlass_Sides", "Door_WP");
            }
            else if (createInfo.materialTypeBack == DoorMaterialType::RESIDENT_EVIL) {
                meshNodes.SetMaterialByMeshName("DoorGlass_Back", "Door_RE");
                meshNodes.SetMaterialByMeshName("DoorGlass_Sides", "Door_RE");
            }

            // Glass frame
            meshNodes.SetMaterialByMeshName("DoorGlassArtFrame", "DoorGlassFrame");
        }

        if (createInfo.type == DoorType::STAINED_GLASS2) {

            // Collision mesh node
            MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
            door.meshName = "BlackDoor_Front";
            door.rigidDynamic.createObject = true;
            door.rigidDynamic.kinematic = true;
            door.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
            door.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
            door.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | ITEM_PICK_UP | RAGDOLL_PLAYER | RAGDOLL_ENEMY);
            door.addtoNavMesh = true;

            // Openable mesh node
            MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
            hinges.meshName = "Door_Hinges";
            hinges.materialName = "T_BlackDoor_Door";
            hinges.openable.additionalTriggerMeshNames = {
                "Glass",
                "BlackDoor_Sides",
                "BlackDoor_Front",
                "BlackDoor_Back",
                "DoorBell"
            };
            hinges.openable.isOpenable = true;
            hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
            hinges.openable.initialOpenState = OpenState::CLOSED;
            hinges.openable.minOpenValue = 0.0f;
            hinges.openable.maxOpenValue = createInfo.maxOpenValue;
            hinges.openable.openSpeed = 5.208f;
            hinges.openable.closeSpeed = 5.208f;
            hinges.openable.openingAudio = "Door_Open.wav";
            hinges.openable.closingAudio = "Door_Open.wav";
            hinges.openable.isDeadLock = true;

            MeshNodeCreateInfo& glass = meshNodeCreateInfoSet.emplace_back();
            glass.meshName = "Glass";
            glass.materialName = "T_BlackDoor_Glass";
            glass.blendingMode = BlendingMode::STAINED_GLASS;
            glass.decalType = DecalType::GLASS;
            glass.tintColor = glm::vec3(1.0f);

            meshNodes.Init(id, "Door_StainedGlass2", meshNodeCreateInfoSet);

            std::string front = "T_BlackDoor_Door";
            std::string back = "T_BlackDoor_Door";
            //front = "T_BlackDoorWP_Door";
            //back = "T_BlackDoorWP_Door";

            meshNodes.SetMaterialByMeshName("ChainLocker", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink1", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink2", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink3", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink4", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink5", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink6", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink7", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink8", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink9", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLink10", "T_BlackDoor_Metals");

            meshNodes.SetMaterialByMeshName("DeadLockMain", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("BoltLockerHold", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("Plate", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("BoltLockerGuide", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("DeadLockVault", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("KeyHole", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("DeadLockSwitch", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLach", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("DeadLockCylinder", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("DoorBell", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("ChainLach", "T_BlackDoor_Metals");
            meshNodes.SetMaterialByMeshName("BoltLockerSlide", "T_BlackDoor_Metals");

            meshNodes.SetMaterialByMeshName("BlackDoor_Sides", front);
            meshNodes.SetMaterialByMeshName("BlackDoor_Front", front);
            meshNodes.SetMaterialByMeshName("BlackDoor_Back", back);

            meshNodes.SetMaterialByMeshName("DoorFrame_Front", "DoorFrame_WP"); // black missing?
            meshNodes.SetMaterialByMeshName("DoorFrame_Back", "DoorFrame_WP"); // black missing?
            meshNodes.SetMaterialByMeshName("DoorFrame_Inner", "DoorFrame_WP"); // black missing?

            meshNodes.SetMaterialByMeshName("Glass", "T_BlackDoor_Glass");
            
            meshNodes.SetMaterialByMeshName("Door_Hinges", "DoorMetals"); // set later anyway
            meshNodes.SetMaterialByMeshName("DoorFrame_Hinges", "DoorMetals"); // set later anyway
                
            meshNodes.PrintMeshNames();

        }

        // Frame material
        if (createInfo.materialTypeFrameFront == DoorMaterialType::RESIDENT_EVIL) {
            meshNodes.SetMaterialByMeshName("DoorFrame_Front", "DoorFrame_RE");
        }
        if (createInfo.materialTypeFrameFront == DoorMaterialType::WHITE_PAINT) {
            meshNodes.SetMaterialByMeshName("DoorFrame_Front", "DoorFrame_WP");
        }
        if (createInfo.materialTypeFrameBack == DoorMaterialType::RESIDENT_EVIL) {
            meshNodes.SetMaterialByMeshName("DoorFrame_Back", "DoorFrame_RE");
            meshNodes.SetMaterialByMeshName("DoorFrame_Inner", "DoorFrame_RE");
        }
        if (createInfo.materialTypeFrameBack == DoorMaterialType::WHITE_PAINT) {
            meshNodes.SetMaterialByMeshName("DoorFrame_Back", "DoorFrame_WP");
            meshNodes.SetMaterialByMeshName("DoorFrame_Inner", "DoorFrame_WP");
        }

        // Same for all combinations
        meshNodes.SetMaterialByMeshName("Door_Knob", "DoorOldKnob");
        meshNodes.SetMaterialByMeshName("Door_Hinges", "DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_Hinges", "DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_KnobLatch", "DoorOldKnob");
        meshNodes.CastCSMShadows();
    }
}
