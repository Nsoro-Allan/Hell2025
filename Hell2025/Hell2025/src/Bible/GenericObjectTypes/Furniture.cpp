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

        meshNodes.SetMeshMaterials("Ceiling2");
        meshNodes.SetMaterialByMeshName("Body_Upper", "WallPaper");
        meshNodes.SetMaterialByMeshName("Mantle", "FloorBoards");
    }
}