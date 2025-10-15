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

    void ConfigureMeshNodesDrawersLarge(uint64_t id, MeshNodes& meshNodes);
}