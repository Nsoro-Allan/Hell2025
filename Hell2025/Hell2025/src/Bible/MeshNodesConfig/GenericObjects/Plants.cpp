#include "Bible/Bible.h"
#include "AssetManagement/AssetManager.h"

namespace Bible {
    void ConfigureMeshNodesPlantBlackBerries(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        glm::vec3 scale = glm::vec3(1.25f);

        MeshNodeCreateInfo& leaves = meshNodeCreateInfoSet.emplace_back();
        leaves.meshName = "Leaves";
        leaves.materialName = "Leaves_BlackBerry";
        leaves.blendingMode = BlendingMode::ALPHA_DISCARD;
        leaves.scale = scale;

        MeshNodeCreateInfo& trunk0 = meshNodeCreateInfoSet.emplace_back();
        trunk0.meshName = "Trunk";
        trunk0.materialName = "TreeLarge_0";
        trunk0.scale = scale;

        MeshNodeCreateInfo& trunk1 = meshNodeCreateInfoSet.emplace_back();
        trunk1.meshName = "Trunk2";
        trunk1.materialName = "TreeLarge_0";
        trunk1.scale = scale;

        meshNodes.Init(id, "BlackBerries", meshNodeCreateInfoSet);
        meshNodes.CastCSMShadows();
    }

    void ConfigureMeshNodesPlantTree(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        meshNodes.Init(id, "TreeLarge_0", meshNodeCreateInfoSet);

        meshNodes.SetMaterialByMeshName("Tree", "TreeLarge_0");
        meshNodes.CastCSMShadows();
    }
}