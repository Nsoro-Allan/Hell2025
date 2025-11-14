#include "Util.h"
#include "CreateInfo.h"
#include "Core/JSON.h"

namespace Util {
    CreateInfoCollection CreateInfoCollectionFromJSON(const std::string& jsonString) {
        CreateInfoCollection createInfoCollection;
        nlohmann::json json = nlohmann::json::parse(jsonString);

        createInfoCollection.doors = json.value("Doors", std::vector<DoorCreateInfo>{});
        createInfoCollection.genericObjects = json.value("Drawers", std::vector<GenericObjectCreateInfo>{});
        createInfoCollection.fireplaces = json.value("Fireplace", std::vector<FireplaceCreateInfo>{});
        createInfoCollection.lights = json.value("Lights", std::vector<LightCreateInfo>{});
        createInfoCollection.pianos = json.value("Pianos", std::vector<PianoCreateInfo>{});
        createInfoCollection.pickUps = json.value("PickUps", std::vector<PickUpCreateInfo>{});
        createInfoCollection.pictureFrames = json.value("PictureFrames", std::vector<PictureFrameCreateInfo>{});
        createInfoCollection.housePlanes = json.value("Planes", std::vector<HousePlaneCreateInfo>{});
        createInfoCollection.trees = json.value("Trees", std::vector<TreeCreateInfo>{});
        createInfoCollection.walls = json.value("Walls", std::vector<WallCreateInfo>{});
        createInfoCollection.windows = json.value("Windows", std::vector<WindowCreateInfo>{});

        return createInfoCollection;
    }

    std::string CreateInfoCollectionToJSON(CreateInfoCollection& createInfoCollection) {
        nlohmann::json json;
        json["Doors"] = createInfoCollection.doors;
        json["Drawers"] = createInfoCollection.genericObjects;
        json["Fireplaces"] = createInfoCollection.fireplaces;
        json["Lights"] = createInfoCollection.lights;
        json["Pianos"] = createInfoCollection.pianos;
        json["PickUps"] = createInfoCollection.pickUps;
        json["PictureFrames"] = createInfoCollection.pictureFrames;
        json["Planes"] = createInfoCollection.housePlanes;
        json["Trees"] = createInfoCollection.trees;
        json["Walls"] = createInfoCollection.walls;
        json["Windows"] = createInfoCollection.windows;

        return json.dump(2);
    }

    AdditionalMapData AdditionalMapDataFromJSON(const std::string& jsonString) {
        nlohmann::json json = nlohmann::json::parse(jsonString);

        AdditionalMapData additionalMapData;
        additionalMapData.houseLocations = json["HouseLocations"];
        additionalMapData.playerCampaignSpawns = json["CampaignSpawns"];
        additionalMapData.playerDeathmatchSpawns = json["DeathmatchSpawns"];

        return additionalMapData;
    }

    std::string AdditionalMapDataToJSON(AdditionalMapData& additionalMapData) {
        nlohmann::json json;

        json["HouseLocations"] = additionalMapData.houseLocations;
        json["CampaignSpawns"] = additionalMapData.playerCampaignSpawns;
        json["DeathmatchSpawns"] = additionalMapData.playerDeathmatchSpawns;

        return json.dump(2);
    }
}