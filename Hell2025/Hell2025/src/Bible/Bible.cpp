#include "Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {
    std::unordered_map<std::string, AmmoInfo> g_ammoInfos;
    std::unordered_map<std::string, InventoryItemInfo> m_inventoryItemInfos;
    std::unordered_map<std::string, WeaponAttachmentInfo> g_weaponAttachmentInfos;
    std::unordered_map<std::string, WeaponInfo> g_weaponsInfos;

    std::vector<std::string> g_sortedWeaponNames;

    void InitAmmoInfo();
    void InitInventoryItemInfo();
    void InitWeaponInfo();
    void InitWeaponAttachmentInfo();

    void Init() {
        g_ammoInfos.clear();
        m_inventoryItemInfos.clear();
        g_weaponAttachmentInfos.clear();
        g_weaponsInfos.clear();
        g_sortedWeaponNames.clear();

        InitAmmoInfo();
        InitInventoryItemInfo();
        InitWeaponInfo();
        InitWeaponAttachmentInfo();

        CreateSortedWeaponNameList();
        PrintDebugInfo();

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

    void CreateSortedWeaponNameList() {
        struct TempWeaponName {
            std::string name;
            int damage;
        };

        std::vector<TempWeaponName> melees;
        std::vector<TempWeaponName> pistols;
        std::vector<TempWeaponName> shotguns;
        std::vector<TempWeaponName> automatics;

        for (const auto& [key, value] : g_weaponsInfos) {
            switch (value.type) {
                case WeaponType::MELEE:     melees.push_back(TempWeaponName(key, value.damage));        break;
                case WeaponType::PISTOL:    pistols.push_back(TempWeaponName(key, value.damage));       break;
                case WeaponType::SHOTGUN:   shotguns.push_back(TempWeaponName(key, value.damage));      break;
                case WeaponType::AUTOMATIC: automatics.push_back(TempWeaponName(key, value.damage));    break;
                default: break;
            }
        }

        auto less_than_damage = [](const TempWeaponName& a, const TempWeaponName& b) {
            return a.damage < b.damage;
        };

        std::sort(melees.begin(), melees.end(), less_than_damage);
        std::sort(pistols.begin(), pistols.end(), less_than_damage);
        std::sort(shotguns.begin(), shotguns.end(), less_than_damage);
        std::sort(automatics.begin(), automatics.end(), less_than_damage);

        g_sortedWeaponNames.clear();
        g_sortedWeaponNames.reserve(melees.size() + pistols.size() + shotguns.size() + automatics.size());

        for (const auto& weapon : melees)      g_sortedWeaponNames.push_back(weapon.name);
        for (const auto& weapon : pistols)     g_sortedWeaponNames.push_back(weapon.name);
        for (const auto& weapon : shotguns)    g_sortedWeaponNames.push_back(weapon.name);
        for (const auto& weapon : automatics)  g_sortedWeaponNames.push_back(weapon.name);
    }

    void PrintDebugInfo() {
        std::cout << "\n** BIBLE **\n";
        std::cout << "\n";

        // Sorted weapon names
        std::cout << "Sorted weapon names\n";
        for (size_t i = 0; i < g_sortedWeaponNames.size(); ++i) {
            std::cout << " " << i << ": " << g_sortedWeaponNames[i] << "\n";
        }

        std::cout << "\n";
    }

    AmmoInfo& CreateAmmoInfo(const std::string& name) {
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

    bool AmmoInfoExists(const std::string& name) {
        return g_ammoInfos.find(name) != g_ammoInfos.end();
    }

    bool InventoryItemInfoExists(const std::string& name) {
        return m_inventoryItemInfos.find(name) != m_inventoryItemInfos.end();
    }

    bool WeaponAttachmentInfoExists(const std::string& name) {
        return g_weaponAttachmentInfos.find(name) != g_weaponAttachmentInfos.end();
    }

    bool WeaponInfoExists(const std::string& name) {
        return g_weaponsInfos.find(name) != g_weaponsInfos.end();
    }

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name) {
        if (InventoryItemInfoExists(name)) 
            return &m_inventoryItemInfos[name];

        Logging::Warning() << "Bible::GetInventoryItemInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    int GetInventoryItemSizeByName(const std::string& name) {
        if (InventoryItemInfo* itemInfo = GetInventoryItemInfoByName(name))
            return itemInfo->m_cellSize;
        
        return 0;
    }
}