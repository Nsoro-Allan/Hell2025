#include "Bible.h"
#include "HellLogging.h"
#include <iostream>
#include <unordered_map>
#include "Util.h">

namespace Bible {
    std::unordered_map<std::string, AmmoInfo> g_ammoInfos;
    std::unordered_map<std::string, InventoryItemInfo> m_inventoryItemInfos;
    std::unordered_map<std::string, WeaponAttachmentInfo> g_weaponAttachmentInfos;
    std::unordered_map<std::string, WeaponInfo> g_weaponsInfos;

    void InitInventoryInfo();
    void InitWeaponInfo();

    void Init() {
        g_ammoInfos.clear();
        m_inventoryItemInfos.clear();
        g_weaponAttachmentInfos.clear();
        g_weaponsInfos.clear();

        InitInventoryInfo();
        InitWeaponInfo();

        Logging::Init() << "The Bible has been read";
    }

    void ConfigureMeshNodes(uint64_t id, GenericObjectType type, MeshNodes& meshNodes) {
        switch (type) {
            case GenericObjectType::BATHROOM_BASIN:     return ConfigureMeshNodesBathroomBasin(id, meshNodes);
            case GenericObjectType::BATHROOM_CABINET:   return ConfigureMeshNodesBathroomCabinet(id, meshNodes);
            case GenericObjectType::DRAWERS_SMALL:      return ConfigureMeshNodesDrawersSmall(id, meshNodes);
            case GenericObjectType::DRAWERS_LARGE:      return ConfigureMeshNodesDrawersLarge(id, meshNodes);
            case GenericObjectType::TOILET:             return ConfigureMeshNodesToilet(id, meshNodes);
            case GenericObjectType::COUCH:              return ConfigureMeshNodesCouch(id, meshNodes);
            case GenericObjectType::TEST_MODEL:         return ConfigureTestModel(id, meshNodes);
            case GenericObjectType::TEST_MODEL2:        return ConfigureTestModel2(id, meshNodes);
            case GenericObjectType::TEST_MODEL3:        return ConfigureTestModel3(id, meshNodes);
            case GenericObjectType::TEST_MODEL4:        return ConfigureTestModel4(id, meshNodes);

            default: Logging::Error() << "Bible::ConfigureMeshNodes(...) failed: non-implemented GenericObjectType: '" << Util::GenericObjectTypeToString(type) << "'";
        }
    }

    AmmoInfo& CreateAmmoItemInfo(const std::string& name) {
        AmmoInfo& info = g_ammoInfos[name];
        info.name = name.c_str();
        return info;
    }

    InventoryItemInfo& CreateInventoryItemInfo(const std::string& name) {
        InventoryItemInfo& info = m_inventoryItemInfos[name];
        info.m_name = name;
        return info;
    }

    WeaponAttachmentInfo& CreateWeaponAttachmentInfo(const std::string& name) {
        WeaponAttachmentInfo& info = g_weaponAttachmentInfos[name];
        info.name = name;
        return info;
    }

    WeaponInfo& CreateWeaponInfo(const std::string& name) {
        WeaponInfo& info = g_weaponsInfos[name];
        info.name = name;
        return info;
    }

    bool InventoryItemExists(const std::string& name) {
        return m_inventoryItemInfos.find(name) != m_inventoryItemInfos.end();
    }

    void InitWeaponInfo() {
        // TODO: Move everything from WeaponManager in here
    }

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name) {
        if (InventoryItemExists(name)) {
            return &m_inventoryItemInfos[name];
        }
        Logging::Warning() << "Bible::GetInventoryItemInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    int GetInventoryItemSizeByName(const std::string& name) {
        InventoryItemInfo* itemInfo = GetInventoryItemInfoByName(name);
        if (!itemInfo) return 0;
        else return itemInfo->m_cellSize;
    }
}