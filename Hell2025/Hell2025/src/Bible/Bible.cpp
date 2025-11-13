#include "Bible.h"
#include "HellLogging.h"
#include <iostream>
#include <unordered_map>
#include "Util.h">

namespace Bible {
    std::unordered_map<std::string, InventoryItemInfo> m_inventoryItems;

    void InitInventoryInfo();
    void InitWeaponInfo();

    void Init() {
        InitInventoryInfo();
        InitWeaponInfo();
        Logging::Init() << "The Bible has been read";
    }

    void InitInventoryInfo() {
        m_inventoryItems.clear();

        InventoryItemInfo& glock = m_inventoryItems["Glock"];
        glock.m_name = "Glock"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
        glock.m_examineModelName = "World_Glock";
        glock.m_meshMaterialNames["Glock"] = "Glock";
        glock.m_cellSize = 1;
        glock.m_combineable = true;
        glock.m_discardable = true;
        glock.m_equipable = true;
        glock.m_itemHeading = "GLOCK 22";
        glock.m_description = R"(Australian police issue. Matte and boxy, a cold
little companion. It does the paperwork duty 
without drama. Dependable at short range, 
underwhelming at a distance. A proper piece 
of shit.)";

        InventoryItemInfo& goldenGlock = m_inventoryItems["GoldenGlock"];
        goldenGlock.m_name = "GoldenGlock"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        goldenGlock.m_examineModelName = "Glock2";
        goldenGlock.m_meshMaterialNames["GlockNew"] = "GlockGold";
        goldenGlock.m_meshMaterialNames["GlockRedDotGlassNew"] = "TX_LeupoldGlass_01a";
        goldenGlock.m_meshMaterialNames["GlockRedDotNew"] = "TX_LeupoldRedDot_01a";
        goldenGlock.m_meshMaterialNames["GlockSuppressorNew"] = "TX_Supressor_01a";
        goldenGlock.m_isGold = false;
        goldenGlock.m_cellSize = 1;
        goldenGlock.m_combineable = true;
        goldenGlock.m_discardable = true;
        goldenGlock.m_equipable = true;
        goldenGlock.m_itemHeading = "GOLDEN GLOCK 22";
        goldenGlock.m_description = R"(Shaken naked, not stirred. Pierce Brosnan's 
wet dream, wedding gift dipped in drip and 
glitter. Natalia gonna be in the good books 
for this one.)";

        InventoryItemInfo& tokarev = m_inventoryItems["Tokarev"];
        tokarev.m_name = "Tokarev"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
        tokarev.m_examineModelName = "World_Tokarev";
        tokarev.m_meshMaterialNames["TokarevBody"] = "Tokarev";
        tokarev.m_meshMaterialNames["TokarevGripPolymer"] = "TokarevGrip";
        tokarev.m_cellSize = 1;
        tokarev.m_combineable = false;
        tokarev.m_discardable = true;
        tokarev.m_equipable = true;
        tokarev.m_itemHeading = "TOKAREV";
        tokarev.m_description = R"(Soviet semi-automatic pistol developed in the
1920s. This baby runs hot and she got no frills, 
just prints pretty little holes. Straight out 
of Ourumov's briefcase.)";

        InventoryItemInfo& knife = m_inventoryItems["Knife"];
        knife.m_name = "Knife"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        knife.m_examineModelName = "World_Knife";
        knife.m_initialExamineItemTransform.rotation.y = -0.2f;
        knife.m_initialExamineItemTransform.rotation.z = -0.6f;
        knife.m_meshMaterialNames["Knife"] = "Knife";
        knife.m_cellSize = 1;
        knife.m_combineable = true;
        knife.m_discardable = false;
        knife.m_equipable = true;
        knife.m_itemHeading = "KNIFE";
        knife.m_description = R"(From fish to pharynx, this rusty little dagger 
takes the same short and messy path. Might
wanna bring a cloth.)";

        InventoryItemInfo& remington870 = m_inventoryItems["Remington870"];
        remington870.m_name = "Remington870"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        remington870.m_examineModelName = "World_Remington870";
        remington870.m_meshMaterialNames["Remington870"] = "Shotgun";
        remington870.m_cellSize = 3;
        remington870.m_combineable = false;
        remington870.m_discardable = true;
        remington870.m_equipable = true;
        remington870.m_itemHeading = "REMINGTON 870";
        remington870.m_description = R"(American pump-action built like a fence post. 
This twelve-gauge thunder will clear every 
damn room and barn in time for Judgment 
Day supper.)";

        InventoryItemInfo& spas = m_inventoryItems["SPAS"];
        spas.m_name = "SPAS"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        spas.m_examineModelName = "World_SPAS";
        spas.m_cellSize = 3;
        spas.m_meshMaterialNames["SPAS12_Main"] = "SPAS2_Main";
        spas.m_meshMaterialNames["SPAS12_Moving"] = "SPAS2_Moving";
        spas.m_meshMaterialNames["SPAS12_Stamped"] = "SPAS2_Stamped";
        spas.m_combineable = false;
        spas.m_discardable = true;
        spas.m_equipable = true;
        spas.m_itemHeading = "SPAS";
        spas.m_description = R"(Dual modal, Italian menace. If killing is a sport, 
then Franchi's Special Purpose Automatic 
Shotgun will put you on the podium with the 
cadence of a fucking riot.)";








        // Below you need to swap out the models for the original World_ versions and add an offset to get the angle you want
        // rather than baking into a second export of the model.


        //. !!!!!

        InventoryItemInfo& blackSkull = m_inventoryItems["BlackSkull"];
        blackSkull.m_name = "BlackSkull"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
        blackSkull.m_cellSize = 1;
        blackSkull.m_examineModelName = "BlackSkull2";
        blackSkull.m_meshMaterialNames["BlackSkullExamine"] = "BlackSkull";
        blackSkull.m_combineable = false;
        blackSkull.m_discardable = true;
        blackSkull.m_equipable = false;
        blackSkull.m_itemHeading = "BLACK SKULL";
        blackSkull.m_description = R"(Worth little to you, worth everything to them.)";

        InventoryItemInfo& smallKey = m_inventoryItems["SmallKey"];
        smallKey.m_name = "SmallKey"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
        smallKey.m_cellSize = 1;
        smallKey.m_examineModelName = "SmallKey";
        smallKey.m_meshMaterialNames["SmallKeyExamine"] = "SmallKey";
        smallKey.m_combineable = false;
        smallKey.m_discardable = true;
        smallKey.m_equipable = false;
        smallKey.m_itemHeading = "SMALL KEY";
        smallKey.m_description = R"(Smells of dust and secrets. One turn, one less 
mystery.)";


    }

    void InitWeaponInfo() {
        // TODO: Move everything from WeaponManager in here
    }

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name) {
        if (m_inventoryItems.find(name) != m_inventoryItems.end()) {
            return &m_inventoryItems[name];
        }
        else {
            std::cout << "GetInventoryItemInfoByName::(name) failed because '" << name << "' was not found in m_inventoryItems\n";
            return nullptr;
        }
    }

    int GetInventoryItemSizeByName(const std::string& name) {
        InventoryItemInfo* itemInfo = GetInventoryItemInfoByName(name);
        if (!itemInfo) return 0;
        else return itemInfo->m_cellSize;
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
                

            default: Logging::Error() << "Bible::ConfigureMeshNodes(...) failed from non-implemented GenericObjectType, type was: " << Util::GenericObjectTypeToString(type); return;

                // write an error message for when you fail to init the MeshNodes, that will hapnen a lot when you are rapidly implementing shit
                // so you need a warning!

        }
    }
}