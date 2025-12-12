#include "Bible/Bible.h"
#include "AssetManagement/AssetManager.h"

namespace Bible {
    void ConfigureTestModel(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "StainGlassTest", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Glass", "BathroomCabinetMirror");
        meshNodes.SetBlendingModeByMeshName("Glass", BlendingMode::GLASS);
    }

    void ConfigureTestModel2(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "DeerTest", meshNodeCreateInfoSet);
        meshNodes.SetMeshMaterials("Deer");
        meshNodes.SetMaterialByMeshName("DeerIris", "DeerIris");
        meshNodes.SetMaterialByMeshName("DeerSclera", "Scelra");
        meshNodes.SetBlendingModeByMeshName("DeerSclera", BlendingMode::GLASS);
    }

    void ConfigureTestModel3(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
        door.meshName = "DoorOld_Sides";
        door.rigidDynamicAABB.createObject = true;
        door.rigidDynamicAABB.kinematic = true;
        door.rigidDynamicAABB.filterData.raycastGroup = RAYCAST_DISABLED;
        door.rigidDynamicAABB.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
        door.rigidDynamicAABB.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);

        MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
        hinges.meshName = "Door_Hinges";
        hinges.materialName = "T_Door_RE";
        hinges.openable.additionalTriggerMeshNames = {
            "Door_Front",
            "Door_Back",
            "Door_Sides",

            "DoorGlass_Front",
            "DoorGlass_Back",
            "DoorGlass_Sides",

            "DoorOld_Front",
            "DoorOld_Back",
            "DoorOld_Sides",

            "Door_Knob",
            "Door_Deadlock",

            "DoorGlassArtFrame",
            "DoorGlassArt_Clear",
            "DoorGlassArt_Green",
            "DoorGlassArt_Orange",
            "DoorGlassArt_Purple",
            "DoorGlassArt_Red",
            "DoorGlassArt_Yellow",

            "DoorGlass_Front",
            "DoorGlass_Back",
            "DoorGlass_Sides",
            "DoorGlass_Hinge_Sides",
        };
        hinges.openable.isOpenable = true;
        hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
        hinges.openable.initialOpenState = OpenState::CLOSED;
        hinges.openable.minOpenValue = 0.0f;
        hinges.openable.maxOpenValue = 2.1f;
        hinges.openable.openSpeed = 5.208f;
        hinges.openable.closeSpeed = 5.208f;
        hinges.openable.openingAudio = "Door_Open.wav";
        hinges.openable.closingAudio = "Door_Open.wav";

        meshNodes.Init(id, "DoorV5", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Door_Back", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("Door_Front", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("Door_Sides", "T_Door_RE");

        meshNodes.SetMaterialByMeshName("DoorGlass_Front", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("DoorGlass_Back", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("DoorGlass_Sides", "T_Door_RE");

        meshNodes.SetMaterialByMeshName("DoorOld_Back", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("DoorOld_Front", "T_Door_RE");
        meshNodes.SetMaterialByMeshName("DoorOld_Sides", "T_Door_RE");

        meshNodes.SetMaterialByMeshName("DoorGlassArtFrame", "T_DoorGlassFrame");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Clear", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Green", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Orange", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Purple", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Red", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Yellow", "T_DoorGlass");

        meshNodes.SetMaterialByMeshName("Door_Knob", "T_DoorOldKnob");
        meshNodes.SetMaterialByMeshName("Door_Hinges", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("Door_Deadlock", "T_DoorMetals");

        meshNodes.SetMaterialByMeshName("DoorFrame_Front", "T_DoorFrame_RE");
        meshNodes.SetMaterialByMeshName("DoorFrame_Back", "T_DoorFrame_RE");
        meshNodes.SetMaterialByMeshName("DoorFrame_Inner", "T_DoorFrame_RE");

        meshNodes.SetMaterialByMeshName("DoorFrame_Deadlock", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_Hinges", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_KnobLatch", "T_DoorOldKnob");
    }

    void ConfigureTestModel4(uint64_t id, MeshNodes& meshNodes) {

        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
        door.meshName = "DoorGlass_Sides";
        door.rigidDynamicAABB.createObject = true;
        door.rigidDynamicAABB.kinematic = true;
        door.rigidDynamicAABB.filterData.raycastGroup = RAYCAST_DISABLED;
        door.rigidDynamicAABB.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
        door.rigidDynamicAABB.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);

        MeshNodeCreateInfo& hinges = meshNodeCreateInfoSet.emplace_back();
        hinges.meshName = "Door_Hinges";
        hinges.materialName = "T_Door_RE";
        hinges.openable.additionalTriggerMeshNames = {
            "Door_Front",
            "Door_Back",
            "Door_Sides",

            "DoorGlass_Front",
            "DoorGlass_Back",
            "DoorGlass_Sides",

            "DoorOld_Front",
            "DoorOld_Back",
            "DoorOld_Sides",

            "Door_Knob",
            "Door_Deadlock",

            "DoorGlassArtFrame",
            "DoorGlassArt_Clear",
            "DoorGlassArt_Green",
            "DoorGlassArt_Orange",
            "DoorGlassArt_Purple",
            "DoorGlassArt_Red",
            "DoorGlassArt_Yellow",

            "DoorGlass_Front",
            "DoorGlass_Back",
            "DoorGlass_Sides",
            "DoorGlass_Hinge_Sides",
        };
        hinges.openable.isOpenable = true;
        hinges.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
        hinges.openable.initialOpenState = OpenState::CLOSED;
        hinges.openable.minOpenValue = 0.0f;
        hinges.openable.maxOpenValue = 2.1f;
        hinges.openable.openSpeed = 5.208f;
        hinges.openable.closeSpeed = 5.208f;
        hinges.openable.openingAudio = "Door_Open.wav";
        hinges.openable.closingAudio = "Door_Open.wav";

        meshNodes.Init(id, "DoorV4", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Door_Back", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("Door_Front", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("Door_Sides", "T_Door_WP");

        meshNodes.SetMaterialByMeshName("DoorGlass_Front", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("DoorGlass_Back", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("DoorGlass_Sides", "T_Door_WP");

        meshNodes.SetMaterialByMeshName("DoorOld_Back", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("DoorOld_Front", "T_Door_WP");
        meshNodes.SetMaterialByMeshName("DoorOld_Sides", "T_Door_WP");

        meshNodes.SetMaterialByMeshName("DoorGlassArtFrame", "T_DoorGlassFrame");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Clear", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Green", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Orange", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Purple", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Red", "T_DoorGlass");
        meshNodes.SetMaterialByMeshName("DoorGlassArt_Yellow", "T_DoorGlass");

        meshNodes.SetMaterialByMeshName("Door_Knob", "T_DoorOldKnob");
        meshNodes.SetMaterialByMeshName("Door_Hinges", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("Door_Deadlock", "T_DoorMetals");

        meshNodes.SetMaterialByMeshName("DoorFrame_Front", "T_DoorFrame_WP");
        meshNodes.SetMaterialByMeshName("DoorFrame_Back", "T_DoorFrame_WP");
        meshNodes.SetMaterialByMeshName("DoorFrame_Inner", "T_DoorFrame_WP");

        meshNodes.SetMaterialByMeshName("DoorFrame_Deadlock", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_Hinges", "T_DoorMetals");
        meshNodes.SetMaterialByMeshName("DoorFrame_KnobLatch", "T_DoorOldKnob");
    }
}