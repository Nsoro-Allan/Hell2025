#include "Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {
    std::unordered_map<std::string, AmmoInfo> g_ammoInfos;
    std::unordered_map<std::string, InventoryItemInfo> g_inventoryItemInfos;
    std::unordered_map<std::string, PickUpInfo> g_pickUpInfos;
    std::unordered_map<std::string, WeaponAttachmentInfo> g_weaponAttachmentInfos;
    std::unordered_map<std::string, WeaponInfo> g_weaponsInfos;

    std::vector<std::string> g_sortedWeaponNames;  // Sorted by weapon type, then damage
    std::vector<std::string> g_sortedAmmoNames;    // Sorted alphabetically

    void InitAmmoInfo();
    void InitInventoryItemInfo();
    void InitPickUpInfo();
    void InitWeaponInfo();
    void InitWeaponAttachmentInfo();
    void CreateSortedAmmoNameList();
    void CreateSortedWeaponNameList();
    void Validate();

    void Init() {
        g_ammoInfos.clear();
        g_inventoryItemInfos.clear();
        g_pickUpInfos.clear();
        g_weaponAttachmentInfos.clear();
        g_weaponsInfos.clear();
        g_sortedWeaponNames.clear();
        g_sortedAmmoNames.clear();

        InitAmmoInfo();
        InitInventoryItemInfo();
        InitPickUpInfo();
        InitWeaponInfo();
        InitWeaponAttachmentInfo();

        Validate();

        CreateSortedAmmoNameList();
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
            case GenericObjectType::PLANT_BLACKBERRIES: return ConfigureMeshNodesPlantBlackBerries(id, meshNodes);
            case GenericObjectType::PLANT_TREE:         return ConfigureMeshNodesPlantTree(id, meshNodes);
            case GenericObjectType::TEST_MODEL:         return ConfigureTestModel(id, meshNodes);
            case GenericObjectType::TEST_MODEL2:        return ConfigureTestModel2(id, meshNodes);
            case GenericObjectType::TEST_MODEL3:        return ConfigureTestModel3(id, meshNodes);
            case GenericObjectType::TEST_MODEL4:        return ConfigureTestModel4(id, meshNodes);

            default: Logging::Error() << "Bible::ConfigureMeshNodes(...) failed: non-implemented GenericObjectType: '" << Util::GenericObjectTypeToString(type) << "'";
        }
    }

    void CreateSortedAmmoNameList() {
        g_sortedAmmoNames.clear();
        g_sortedAmmoNames.reserve(g_ammoInfos.size());

        for (const auto& [key, value] : g_ammoInfos) {
            g_sortedAmmoNames.push_back(key);
        }

        std::sort(g_sortedAmmoNames.begin(), g_sortedAmmoNames.end());
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

    void Validate() {
        // Weapon Info
        for (const auto& [name, value] : g_weaponsInfos) {
            const WeaponInfo& weaponInfo = value;

            // Check ammo name
            if (!AmmoInfoExists(weaponInfo.ammoType) && name != "Knife") {
                Logging::Warning() << "WeaponInfo '" << name << "' has a not found ammoType '" << weaponInfo.ammoType << "'";
            }
            // Check pickup name
            if (!PickUpInfoExists(weaponInfo.pickupName) && name != "Knife") {
                Logging::Warning() << "WeaponInfo '" << name << "' has a not found pickupName '" << weaponInfo.pickupName << "'";
            }
        }
    }

    void PrintDebugInfo() {
        std::cout << "\n** BIBLE **\n";

        std::cout << "\nAmmo\n";
        for (size_t i = 0; i < g_sortedAmmoNames.size(); ++i) {
            std::cout << " " << i << ": " << g_sortedAmmoNames[i] << "\n";
        }

        std::cout << "\nPickups\n";
        for (const auto& [name, value] : g_pickUpInfos) {
            std::cout << " - " << name << "\n";
        }

        std::cout << "\nWeapons\n";
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
        InventoryItemInfo& info = g_inventoryItemInfos[name];
        info.m_name = name;
        return info;
    }

    PickUpInfo& CreatePickUpInfo(const std::string& name) {
        PickUpInfo& info = g_pickUpInfos[name];
        info.name = name;
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
        return g_inventoryItemInfos.find(name) != g_inventoryItemInfos.end();
    }

    bool PickUpInfoExists(const std::string& name) {
        return g_pickUpInfos.find(name) != g_pickUpInfos.end();
    }

    bool WeaponAttachmentInfoExists(const std::string& name) {
        return g_weaponAttachmentInfos.find(name) != g_weaponAttachmentInfos.end();
    }

    bool WeaponInfoExists(const std::string& name) {
        return g_weaponsInfos.find(name) != g_weaponsInfos.end();
    }


    AmmoInfo* GetAmmoInfoByName(const std::string& name) {
        if (AmmoInfoExists(name))
            return &g_ammoInfos[name];

        Logging::Warning() << "Bible::GetAmmoInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name) {
        if (InventoryItemInfoExists(name)) 
            return &g_inventoryItemInfos[name];

        Logging::Warning() << "Bible::GetInventoryItemInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    PickUpInfo* GetPickUpInfoByName(const std::string& name) {
        if (PickUpInfoExists(name))
            return &g_pickUpInfos[name];

        Logging::Warning() << "Bible::GetPickUpInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    WeaponInfo* GetWeaponInfoByName(const std::string& name) {
        if (WeaponInfoExists(name))
            return &g_weaponsInfos[name];

        Logging::Warning() << "Bible::GetWeaponInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }


    WeaponAttachmentInfo* GetWeaponAttachmentInfoByName(const std::string& name) {
        if (WeaponAttachmentInfoExists(name))
            return &g_weaponAttachmentInfos[name];

        Logging::Warning() << "Bible::GetWeaponAttachmentInfoByName::(...) failed: '" << name << "' not found\n";
        return nullptr;
    }

    int32_t GetWeaponIndexFromWeaponName(const std::string& weaponName) {
        std::vector<WeaponInfo> g_sortedWeaponNames;
        for (int i = 0; i < g_sortedWeaponNames.size(); i++) {
            if (g_sortedWeaponNames[i].name == weaponName) {
                return i;
            }
        }
        return -1;
    }

    int GetInventoryItemSizeByName(const std::string& name) {
        if (InventoryItemInfo* itemInfo = GetInventoryItemInfoByName(name))
            return itemInfo->m_cellSize;
        
        return 0;
    }

    const std::vector<std::string>& GetAmmoNameList() {
        return g_sortedAmmoNames;
    }

    const std::vector<std::string>& GetWeaponNameList() {
        return g_sortedWeaponNames;
    }
}