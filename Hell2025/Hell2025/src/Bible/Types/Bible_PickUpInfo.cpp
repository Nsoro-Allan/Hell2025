#include "Bible/Bible.h"
#include "HellLogging.h"
#include <unordered_map>
#include "Util.h">

namespace Bible {

    void InitPickUpInfo() {
        PickUpInfo& glock = CreatePickUpInfo("Glock");
        glock.mass = 3.3f;
        glock.type = PickUpType::WEAPON;

        PickUpInfo& goldenGlock = CreatePickUpInfo("GoldenGlock");
        goldenGlock.mass = 3.3f;
        goldenGlock.type = PickUpType::WEAPON;

        PickUpInfo& remington870 = CreatePickUpInfo("Remington870");
        remington870.mass = 3.2f;
        remington870.type = PickUpType::WEAPON;

        PickUpInfo& spas = CreatePickUpInfo("SPAS");
        spas.mass = 3.3f;
        spas.type = PickUpType::WEAPON;

        PickUpInfo& tokarev = CreatePickUpInfo("Tokarev");
        tokarev.mass = 0.7f;
        tokarev.type = PickUpType::WEAPON;

        PickUpInfo& shottyBuckShotBox = CreatePickUpInfo("ShotgunBuckShotBox");
        shottyBuckShotBox.mass = 0.45f;
        shottyBuckShotBox.type = PickUpType::AMMO;

        PickUpInfo& shottySlugBox = CreatePickUpInfo("ShotgunSlugBox");
        shottySlugBox.mass = 0.45f;
        shottySlugBox.type = PickUpType::AMMO;
    }

    float GetPickUpMass(const std::string& name) {
        if (PickUpInfo* pickUpInfo = GetPickUpInfoByName(name)) {
            return pickUpInfo->mass;
        }
        return 0;
    }
}