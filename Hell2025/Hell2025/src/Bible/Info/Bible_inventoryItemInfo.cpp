#include "Bible/Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {

    void InitInventoryItemInfo() {

        InventoryItemInfo& aks74u = CreateInventoryItemInfo("AKS74U");
        aks74u.m_itemType = InventoryItemType::WEAPON;
        aks74u.m_cellSize = 2;
        aks74u.m_combineable = false;
        aks74u.m_discardable = true;
        aks74u.m_equipable = true;
        aks74u.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        aks74u.m_examineModelScale = glm::vec3(2.75f);
        aks74u.m_maxExamineZoom = 1.75f;
        aks74u.m_itemHeading = "AKS-74U";
        aks74u.m_description = R"(Pew pew pew.)";

        InventoryItemInfo& glock = CreateInventoryItemInfo("Glock");
        glock.m_itemType = InventoryItemType::WEAPON;
        glock.m_cellSize = 1;
        glock.m_combineable = true;
        glock.m_discardable = true;
        glock.m_equipable = true;
        glock.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glock.m_examineModelScale = glm::vec3(4.5f);
        glock.m_maxExamineZoom = 1.85f;
        glock.m_itemHeading = "GLOCK 22";
        glock.m_description = R"(Australian police issue. Matte and boxy, a cold
little companion. It does the paperwork duty 
without drama. Dependable at short range, 
underwhelming at a distance. A proper piece 
of shit.)";

        InventoryItemInfo& goldenGlock = CreateInventoryItemInfo("GoldenGlock");
        goldenGlock.m_itemType = InventoryItemType::WEAPON;
        goldenGlock.m_cellSize = 1;
        goldenGlock.m_combineable = true;
        goldenGlock.m_discardable = true;
        goldenGlock.m_equipable = true;
        goldenGlock.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        goldenGlock.m_examineModelScale = glm::vec3(4.5f);
        goldenGlock.m_maxExamineZoom = 1.85f;
        goldenGlock.m_itemHeading = "GOLDEN GLOCK 22";
        goldenGlock.m_description = R"(Shaken naked, not stirred. Pierce Brosnan's 
wet dream, wedding gift dipped in drip and 
glitter. Natalia gonna be in the good books 
for this one.)";

        InventoryItemInfo& tokarev = CreateInventoryItemInfo("Tokarev");
        tokarev.m_itemType = InventoryItemType::WEAPON;
        tokarev.m_cellSize = 1;
        tokarev.m_combineable = false;
        tokarev.m_discardable = true;
        tokarev.m_equipable = true;
        tokarev.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        tokarev.m_examineModelScale = glm::vec3(4.5f);
        tokarev.m_maxExamineZoom = 1.85f;
        tokarev.m_itemHeading = "TOKAREV";
        tokarev.m_description = R"(Soviet semi-automatic pistol developed in the
1920s. This baby runs hot and she got no frills, 
just prints pretty little holes. Straight out 
of Ourumov's briefcase.)";

        InventoryItemInfo& knife = CreateInventoryItemInfo("Knife");
        knife.m_itemType = InventoryItemType::WEAPON;
        knife.m_cellSize = 1;
        knife.m_combineable = true;
        knife.m_discardable = false;
        knife.m_equipable = true;
        knife.m_examineModelRotation = glm::vec3(0.0f, -0.2f, -0.6f);
        knife.m_examineModelScale = glm::vec3(3.5f);
        knife.m_maxExamineZoom = 1.75f;
        knife.m_itemHeading = "KNIFE";
        knife.m_description = R"(From fish to pharynx, this rusty little dagger 
takes the same short and messy path. Might
wanna bring a cloth.)";

        InventoryItemInfo& remington870 = CreateInventoryItemInfo("Remington870");
        remington870.m_itemType = InventoryItemType::WEAPON;
        remington870.m_cellSize = 3;
        remington870.m_combineable = false;
        remington870.m_discardable = true;
        remington870.m_equipable = true;
        remington870.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        remington870.m_examineModelScale = glm::vec3(2.125f);
        remington870.m_maxExamineZoom = 1.75f;
        remington870.m_itemHeading = "REMINGTON 870";
        remington870.m_description = R"(American pump-action built like a fence post. 
This twelve-gauge thunder will clear every 
damn room and barn in time for Judgment 
Day supper.)";

        InventoryItemInfo& spas = CreateInventoryItemInfo("SPAS");
        spas.m_itemType = InventoryItemType::WEAPON;
        spas.m_cellSize = 3;
        spas.m_combineable = false;
        spas.m_discardable = true;
        spas.m_equipable = true;
        spas.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        spas.m_examineModelScale = glm::vec3(2.95f);
        spas.m_maxExamineZoom = 1.75f;
        spas.m_itemHeading = "SPAS";
        spas.m_description = R"(Dual modal, Italian menace. If killing is a sport, 
then Franchi's Special Purpose Automatic 
Shotgun will put you on the podium with the 
cadence of a fucking riot.)";

        InventoryItemInfo& shotgunSlugBox = CreateInventoryItemInfo("12GaugeBuckShot");
        shotgunSlugBox.m_itemType = InventoryItemType::WEAPON;
        shotgunSlugBox.m_cellSize = 3;
        shotgunSlugBox.m_discardable = true;
        shotgunSlugBox.m_equipable = false;
        shotgunSlugBox.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        shotgunSlugBox.m_examineModelScale = glm::vec3(2.5f);
        shotgunSlugBox.m_maxExamineZoom = 1.75f;
        shotgunSlugBox.m_itemHeading = "12 Gauge Ammo";
        shotgunSlugBox.m_description = R"(Neque porro quisquam est qui dolorem 
ipsum quia dolor sit amet, consectetur, 
adipisci velit...)";



        InventoryItemInfo& blackSkull = CreateInventoryItemInfo("BlackSkull");
        blackSkull.m_itemType = InventoryItemType::UESLESS;
        blackSkull.m_cellSize = 1;
        blackSkull.m_combineable = false;
        blackSkull.m_discardable = true;
        blackSkull.m_equipable = false;
        blackSkull.m_examineModelTranslation = glm::vec3(0.0f, 0.2f, 0.0f);
        blackSkull.m_examineModelRotation = glm::vec3(1.1f, 0.0f, 0.0f);
        blackSkull.m_examineModelScale = glm::vec3(4.5f);
        blackSkull.m_maxExamineZoom = 1.75f;
        blackSkull.m_itemHeading = "BLACK SKULL";
        blackSkull.m_description = R"(Worth little to you, worth everything to them.)";

        InventoryItemInfo& smallKey = CreateInventoryItemInfo("SmallKey");
        smallKey.m_itemType = InventoryItemType::KEY;
        smallKey.m_cellSize = 1;
        smallKey.m_combineable = false;
        smallKey.m_discardable = true;
        smallKey.m_equipable = false;
        smallKey.m_examineModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        smallKey.m_examineModelScale = glm::vec3(2.5f);
        smallKey.m_maxExamineZoom = 1.75f;
        smallKey.m_itemHeading = "SMALL KEY";
        smallKey.m_description = R"(Smells of dust and secrets. One turn, one less 
mystery.)";


    }
}