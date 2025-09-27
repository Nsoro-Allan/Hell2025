#include "Util.h"
#include "CreateInfo.h"
#include "Core/JSON.h"

namespace Util {
    CreateInfoCollection CreateInfoCollectionFromJSON(const std::string& jsonString) {
        CreateInfoCollection createInfoCollection;
        nlohmann::json json = nlohmann::json::parse(jsonString);

        // Load Game Objects
        //for (auto& jsonObject : json["GameObjects"]) {
        //    GameObjectCreateInfo& createInfo = createInfoCollection.gameObjects.emplace_back();
        //    createInfo.position = jsonObject["position"];
        //    createInfo.rotation = jsonObject["rotation"];
        //    createInfo.scale = jsonObject["scale"];
        //    createInfo.modelName = jsonObject["modelName"];
        //    createInfo.meshRenderingInfoSet = jsonObject["meshRenderingInfo"];
        //}

        // Load lights
        //for (auto& jsonObject : json["Lights"]) {
        //    LightCreateInfo& createInfo = sectorCreateInfo.lights.emplace_back();
        //    createInfo.position = jsonObject["position"];
        //    createInfo.color = jsonObject["color"];
        //    createInfo.radius = jsonObject["radius"];
        //    createInfo.strength = jsonObject["strength"];
        //    createInfo.type = Util::StringToLightType(jsonObject["type"]);
        //}

        // Pickups
        for (auto& jsonObject : json["PickUps"]) {
            PickUpCreateInfo& createInfo = createInfoCollection.pickUps.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.pickUpType = jsonObject["type"];
        }

        // Trees
        for (auto& jsonObject : json["Trees"]) {
            TreeCreateInfo& createInfo = createInfoCollection.trees.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.scale = jsonObject["scale"];
            createInfo.type = Util::StringToTreeType(jsonObject.value<std::string>("type", std::string("TREE_LARGE_0")));
        }

        return createInfoCollection;
    }

    std::string CreateInfoCollectionToJSON(CreateInfoCollection& createInfoCollection) {
        nlohmann::json json;
        json["PickUps"] = nlohmann::json::array();
        json["Trees"] = nlohmann::json::array();

        // Pick Ups
        for (const PickUpCreateInfo& createInfo : createInfoCollection.pickUps) {
            json["PickUps"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "type", createInfo.pickUpType }
            });
        }

        // Trees
        for (const TreeCreateInfo& createInfo : createInfoCollection.trees) {
            json["Trees"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "scale", createInfo.scale },
                { "type", Util::TreeTypeToString(createInfo.type) }
            });
        }

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