#pragma once
#include "Inventory/Inventory.h"
#include "Types/Renderer/MeshNodes.h"
#include "Weapon/WeaponCommon.h"        // put me and my contents somewhere better!
#include <string>
#include <vector>

namespace Bible {
    void Init();
    void ConfigureMeshNodes(uint64_t id, GenericObjectType type, MeshNodes& meshNodes);

    AmmoInfo& CreateAmmoInfo(const std::string& name);
    InventoryItemInfo& CreateInventoryItemInfo(const std::string& name);
    PickUpInfo& CreatePickUpInfo(const std::string& name);
    WeaponAttachmentInfo& CreateWeaponAttachmentInfo(const std::string& name);
    WeaponInfo& CreateWeaponInfo(const std::string& name);

    bool AmmoInfoExists(const std::string& name);
    bool InventoryItemInfoExists(const std::string& name);
    bool PickUpInfoExists(const std::string& name);
    bool WeaponAttachmentInfoExists(const std::string& name);
    bool WeaponInfoExists(const std::string& name);

    // Misc
    void PrintDebugInfo();

    void ConfigureMeshNodesByPickUpName(uint64_t id, const std::string& pickUpName, MeshNodes& meshNodes, bool createPhysicsObjects);
    void ConfigureDoorMeshNodes(uint64_t id, DoorCreateInfo& createInfo, MeshNodes& meshNodes);

    void ConfigureTestModel(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel2(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel3(uint64_t id, MeshNodes& meshNodes);
    void ConfigureTestModel4(uint64_t id, MeshNodes& meshNodes);

    void ConfigureMeshNodesCouch(uint64_t, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersSmall(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesDrawersLarge(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesToilet(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomBasin(uint64_t id, MeshNodes& meshNodes);
    void ConfigureMeshNodesBathroomCabinet(uint64_t id, MeshNodes& meshNodes);

    const std::vector<std::string>& GetAmmoNameList();
    const std::vector<std::string>& GetWeaponNameList();

    AmmoInfo* GetAmmoInfoByName(const std::string& name);
    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name);
    PickUpInfo* GetPickUpInfoByName(const std::string& name);
    WeaponInfo* GetWeaponInfoByName(const std::string& name); 
    WeaponAttachmentInfo* GetWeaponAttachmentInfoByName(const std::string& name);
    
    int GetInventoryItemSizeByName(const std::string& name);
    int32_t GetWeaponIndexFromWeaponName(const std::string& weaponName);
    int32_t GetWeaponMagSize(const std::string& name);
    float GetPickUpMass(const std::string& name);

}