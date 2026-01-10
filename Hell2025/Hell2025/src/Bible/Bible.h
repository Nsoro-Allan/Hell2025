#pragma once
#include <string>
#include <vector>

#include "Inventory/Inventory.h"
#include "Types/Core/ItemInfo.h"
#include "Types/Renderer/MeshNodes.h"
#include "Weapon/WeaponCommon.h"        // put me and my contents somewhere better!

namespace Bible {
    void Init();
    void ConfigureMeshNodes(uint64_t id, GenericObjectType type, MeshNodes& meshNodes);

    AmmoInfo& CreateAmmoInfo(const std::string& name);
    ItemInfo& CreateInventoryItemInfo(const std::string& name);
    WeaponAttachmentInfo& CreateWeaponAttachmentInfo(const std::string& name);
    WeaponInfo& CreateWeaponInfo(const std::string& name);

    bool AmmoInfoExists(const std::string& name);
    bool ItemInfoExists(const std::string& name);
    bool WeaponAttachmentInfoExists(const std::string& name);
    bool WeaponInfoExists(const std::string& name);

    // Text
    const std::string& MermaidShopGreeting();
    const std::string& MermaidShopWeaponPurchaseConfirmationText();
    const std::string& MermaidShopFailedPurchaseText();

    // Misc
    void PrintDebugInfo();

    void ConfigureMeshNodesByItemName(uint64_t id, const std::string& itemName, MeshNodes& meshNodes, bool createPhysicsObjects);
    void ConfigureDoorMeshNodes(uint64_t id, DoorCreateInfo& createInfo, MeshNodes& meshNodes);

    void ConfigureTestModel(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel2(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel3(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel4(uint64_t id, MeshNodes& meshNodes);

    void ConfigureMeshNodesChristmasPresentSmall(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesChristmasPresentLarge(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesChristmasTree(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesCouch(uint64_t, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersSmall(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersLarge(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesMermaidRock(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesPlantBlackBerries(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesPlantTree(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesToilet(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomBasin(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomCabinet(uint64_t id, MeshNodes& meshNodes);

    const std::vector<std::string>& GetAmmoNameList();
    const std::vector<std::string>& GetWeaponNameList();
    
    AmmoInfo* GetAmmoInfoByName(const std::string& name);
    ItemInfo* GetItemInfoByName(const std::string& name);
    WeaponInfo* GetWeaponInfoByName(const std::string& name); 
    WeaponAttachmentInfo* GetWeaponAttachmentInfoByName(const std::string& name);
    
    int GetInventoryItemSizeByName(const std::string& name);
    int32_t GetWeaponIndexFromWeaponName(const std::string& weaponName);
    int32_t GetWeaponMagSize(const std::string& name);
    int32_t GetAmmoPickUpAmount(const std::string& name);
    float GetItemMass(const std::string& name);
    ItemType GetItemType(const std::string& name);

    int GetPlayerKillCashReward();
    int GetPlayerHeadShotCashReward();

    int GetItemCost(const std::string& name);
}