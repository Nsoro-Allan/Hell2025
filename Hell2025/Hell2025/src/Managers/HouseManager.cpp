#include "HouseManager.h"
#include <fstream>
#include "HellLogging.h"
#include "Core/JSON.h"
#include "World/World.h"
#include "Util.h"

namespace HouseManager {
    std::vector<House> g_houses;

    void Init() {
        //g_houses.clear();
        //for (FileInfo& fileInfo : Util::IterateDirectory("res/houses/", { "json" })) {
        //    g_houses[fileInfo.name] = JSON::LoadHouse(fileInfo.path);
        //}

        LoadHouse("TestHouse");
    }

    void LoadHouse(const std::string& filename) {
        const std::string path = "res/houses/" + filename + ".house";
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            Logging::Error() << "HouseManager::LoadHouse(): failed to open '" << path;
            return;
        }

        nlohmann::json json;
        if (!JSON::LoadJsonFromFile(json, path)) {
            std::cerr << "HouseManager::LoadHouse() failed to open file: " << path;
            return;
        }

        House& house = g_houses.emplace_back();
        house.SetFilename(filename);

        CreateInfoCollection& createInfoCollection = house.GetCreateInfoCollection();
        createInfoCollection.doors = json.value("Doors", std::vector<DoorCreateInfo>{});
        createInfoCollection.fireplaces = json.value("Fireplaces", std::vector<FireplaceCreateInfo>{});
        createInfoCollection.genericObjects = json.value("Drawers", std::vector<GenericObjectCreateInfo>{});
        createInfoCollection.lights = json.value("Lights", std::vector<LightCreateInfo>{});
        createInfoCollection.housePlanes = json.value("Planes", std::vector<HousePlaneCreateInfo>{}); // error here
        createInfoCollection.pianos = json.value("Pianos", std::vector<PianoCreateInfo>{});
        createInfoCollection.pictureFrames = json.value("PictureFrames", std::vector<PictureFrameCreateInfo>{});
        createInfoCollection.walls = json.value("Walls", std::vector<WallCreateInfo>{});
        createInfoCollection.windows = json.value("Windows", std::vector<WindowCreateInfo>{});

        for (size_t i = 0; i < createInfoCollection.genericObjects.size();) {
            if (createInfoCollection.genericObjects[i].type == GenericObjectType::UNDEFINED) {
                createInfoCollection.genericObjects.erase(createInfoCollection.genericObjects.begin() + i);
                Logging::Error() << "Found UNDEFINED GenericGameObject in " << filename << " and removed it";
            }
            else {
                ++i;
            }
        }

        Logging::Debug()
            << "Loaded: " << path
            << "\n" << Util::CreateInfoCollectionToJSON(createInfoCollection)
            //<< "- signature:     " << header.signature << "\n"
            //<< "- version:       " << header.version << "\n"
            //<< "- chunk count x: " << header.chunkCountX << "\n"
            //<< "- chunk count z: " << header.chunkCountZ << "\n"
            //<< createInfoJson << "\n"
            //<< additionalJson;
            << "";
    }
    
    void SaveHouse(const std::string& filename) {
        House* house = GetHouseByName(filename);
        if (!house) {
            Logging::Error() << "HouseManager::SaveHouse(): failed because '" << filename << "' was not found.";
            return;
        }

        // Construct the JSON string
        CreateInfoCollection createInfoCollection = World::GetCreateInfoCollection();
        house->SetCreateInfoCollection(createInfoCollection);

        std::string createInfoJson = Util::CreateInfoCollectionToJSON(createInfoCollection);

        // Create the file
        std::string outputPath = "res/houses/" + filename + ".house";
        std::ofstream file(outputPath, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            std::cout << "Failed to open file for writing: " << outputPath << "\n";
            return;
        }

        // Quick n dirty dump of the string to file
        file.write(createInfoJson.data(), static_cast<std::streamsize>(createInfoJson.size()));

        Logging::Debug() 
            << "Saved: " << outputPath
            << "\n" << createInfoJson
            << "";
    }

    void UpdateCreateInfoCollectionFromWorld(const std::string& houseName) {
        House* house = GetHouseByName(houseName);
        if (!house) {
            Logging::Error() << "HouseManager::UpdateCreateInfoCollectionFromWorld(): failed because '" << houseName << "' was not found.";
            return;
        }

        CreateInfoCollection createInfoCollection = World::GetCreateInfoCollection();
        house->SetCreateInfoCollection(createInfoCollection);
    }

    House* GetHouseByName(const std::string& filename) {
        for (House& house : g_houses) {
            if (house.GetFilename() == filename) {
                return &house;
            }
        }
        Logging::Error() << "HouseManager::GetHouseByName() failed coz '" << filename << "' was not found";
        return nullptr;
    }
}