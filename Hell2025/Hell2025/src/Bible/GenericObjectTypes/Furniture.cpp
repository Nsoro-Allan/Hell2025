#include "Bible/Bible.h"
#include "AssetManagement/AssetManager.h"

namespace Bible {
    void ConfigureMeshNodesCouch(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& body = meshNodeCreateInfoSet.emplace_back();
        body.meshName = "CouchBody";
        body.materialName = "Couch";

        PhysicsFilterData filterData;
        filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
        filterData.collisionGroup = CollisionGroup::GENERIC_BOUNCEABLE;
        filterData.collidesWith = CollisionGroup(ENVIROMENT_OBSTACLE | GENERIC_BOUNCEABLE);

        MeshNodeCreateInfo& cushion0 = meshNodeCreateInfoSet.emplace_back();
        cushion0.meshName = "Cushion0";
        cushion0.materialName = "Couch";
        //cushion0.type = MeshNodeType::RIGID_DYNAMIC;
        //cushion0.rigidDynamic.filterData = filterData;
        
        MeshNodeCreateInfo& cushion1 = meshNodeCreateInfoSet.emplace_back();
        cushion1.meshName = "Cushion1";
        cushion1.materialName = "Couch";

        MeshNodeCreateInfo& cushion2 = meshNodeCreateInfoSet.emplace_back();
        cushion2.meshName = "Cushion2";
        cushion2.materialName = "Couch";

        MeshNodeCreateInfo& cushion3 = meshNodeCreateInfoSet.emplace_back();
        cushion3.meshName = "Cushion3";
        cushion3.materialName = "Couch";

        MeshNodeCreateInfo& cushion4 = meshNodeCreateInfoSet.emplace_back();
        cushion4.meshName = "Cushion4";
        cushion4.materialName = "Couch";

        meshNodes.Init(id, "Couch", meshNodeCreateInfoSet);
    }

    void ConfigureTestModel(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "Fireplace", meshNodeCreateInfoSet);

        meshNodes.SetMeshMaterials("NumGrid");
        meshNodes.SetMeshMaterials("Ceiling2");
        meshNodes.SetMeshMaterials("WallPaper");
        meshNodes.SetMeshMaterials("PlasterRed");
        meshNodes.SetMeshMaterials("T_Walls");

        meshNodes.SetMaterialByMeshName("Fireplace_Wall_Damage", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Wall_Damage.001", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Bricks_low", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Cap_low", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Log_Tray_low", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Log_Try_Holder_low", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Floor_low", "Fireplace");

        meshNodes.SetMaterialByMeshName("Fireplace_Mantle_low", "Fireplace");
        meshNodes.SetMaterialByMeshName("Fireplace_Mantle_Trim_low", "Fireplace");

        meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log1_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log2_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log3_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log4_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log5_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Coal1_low", "T_Firewood");
        meshNodes.SetMaterialByMeshName("Fireplace_Coal2_low", "T_Firewood");

        meshNodes.SetMaterialByMeshName("Fireplace_Walls_Chipped", "T_WallsChippedEdges");
        meshNodes.SetBlendingModeByMeshName("Fireplace_Walls_Chipped", BlendingMode::ALPHA_DISCARD);


        meshNodes.SetMaterialByMeshName("TrimMiddle", "FloorBoards");
        meshNodes.SetMaterialByMeshName("TrimMiddleArch", "FloorBoards");
        meshNodes.SetMaterialByMeshName("TrimCeiling", "FloorBoards");
    }

    void ConfigureTestModel2(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "DeerTest", meshNodeCreateInfoSet);
        meshNodes.SetMeshMaterials("Deer");
        meshNodes.SetMaterialByMeshName("DeerIris", "DeerIris");
        meshNodes.SetMaterialByMeshName("DeerSclera", "Scelra");
        meshNodes.SetBlendingModeByMeshName("DeerSclera", BlendingMode::GLASS);
    }
}