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
        const std::string path = "res/houses/" + filename + ".json";
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
        createInfoCollection.lights = json.value("Lights", std::vector<LightCreateInfo>{});
        createInfoCollection.planes = json.value("Planes", std::vector<PlaneCreateInfo>{});
        createInfoCollection.pianos = json.value("Pianos", std::vector<PianoCreateInfo>{});
        createInfoCollection.pictureFrames = json.value("PictureFrames", std::vector<PictureFrameCreateInfo>{});
        createInfoCollection.walls = json.value("Walls", std::vector<WallCreateInfo>{});
        createInfoCollection.windows = json.value("Windows", std::vector<WindowCreateInfo>{});

        Logging::Debug()
            << "Loaded house: " << filename << ".json\n"
            //<< "- signature:     " << header.signature << "\n"
            //<< "- version:       " << header.version << "\n"
            //<< "- chunk count x: " << header.chunkCountX << "\n"
            //<< "- chunk count z: " << header.chunkCountZ << "\n"
            //<< createInfoJson << "\n"
            //<< additionalJson;
            << "";
    }
    
    void SaveHouse(const std::string& houseName) {

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