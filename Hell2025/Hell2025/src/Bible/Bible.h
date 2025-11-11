#pragma once
#include "Inventory/Inventory.h"
#include "Types/Renderer/MeshNodes.h"
#include <string>
#include <vector>

namespace Bible {
    void Init();
    void ConfigureMeshNodes(uint64_t id, GenericObjectType type, MeshNodes& meshNodes);

    int GetInventoryItemSizeByName(const std::string& name);
    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name);

    void ConfigureTestModel(uint64_t, MeshNodes& meshNodes);
    void ConfigureTestModel2(uint64_t, MeshNodes& meshNodes);

    void ConfigureMeshNodesCouch(uint64_t, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersSmall(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersLarge(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesToilet(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomBasin(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomCabinet(uint64_t id, MeshNodes& meshNodes);
}