#include "Bible/Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {

    void InitInventoryItemInfo() {

        InventoryItemInfo& glock = CreateInventoryItemInfo("Glock");
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

        InventoryItemInfo& goldenGlock = CreateInventoryItemInfo("GoldenGlock");
        goldenGlock.m_examineModelName = "Glock2";
        goldenGlock.m_meshMaterialNames["GlockNew"] = "GlockGold";
        goldenGlock.m_meshMaterialNames["GlockRedDotGlassNew"] = "TX_LeupoldGlass_01a";
        goldenGlock.m_meshMaterialNames["GlockRedDotNew"] = "TX_LeupoldRedDot_01a";
        goldenGlock.m_meshMaterialNames["GlockSuppressorNew"] = "TX_Supressor_01a";
        goldenGlock.m_cellSize = 1;
        goldenGlock.m_combineable = true;
        goldenGlock.m_discardable = true;
        goldenGlock.m_equipable = true;
        goldenGlock.m_itemHeading = "GOLDEN GLOCK 22";
        goldenGlock.m_description = R"(Shaken naked, not stirred. Pierce Brosnan's 
wet dream, wedding gift dipped in drip and 
glitter. Natalia gonna be in the good books 
for this one.)";

        InventoryItemInfo& tokarev = CreateInventoryItemInfo("Tokarev");
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

        InventoryItemInfo& knife = CreateInventoryItemInfo("Knife");
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

        InventoryItemInfo& remington870 = CreateInventoryItemInfo("Remington870");
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

        InventoryItemInfo& spas = CreateInventoryItemInfo("SPAS");
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

        InventoryItemInfo& blackSkull = CreateInventoryItemInfo("BlackSkull");
        blackSkull.m_cellSize = 1;
        blackSkull.m_examineModelName = "BlackSkull2";
        blackSkull.m_meshMaterialNames["BlackSkullExamine"] = "BlackSkull";
        blackSkull.m_combineable = false;
        blackSkull.m_discardable = true;
        blackSkull.m_equipable = false;
        blackSkull.m_itemHeading = "BLACK SKULL";
        blackSkull.m_description = R"(Worth little to you, worth everything to them.)";

        InventoryItemInfo& smallKey = CreateInventoryItemInfo("SmallKey");
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
}