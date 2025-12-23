#include "Bible/Bible.h"
#include "AssetManagement/AssetManager.h"

namespace Bible {
    void ConfigureMeshNodesPlantBlackBerries(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "BlackBerries", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Leaves", "Leaves_BlackBerry");
        meshNodes.SetBlendingModeByMeshName("Leaves", BlendingMode::ALPHA_DISCARD);
        meshNodes.SetMaterialByMeshName("Trunk", "TreeLarge_0");
        meshNodes.SetMaterialByMeshName("Trunk2", "TreeLarge_0");
        meshNodes.CastCSMShadows();
    }

    void ConfigureMeshNodesPlantTree(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "TreeLarge_0", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Tree", "TreeLarge_0");
        meshNodes.CastCSMShadows();
    }
}