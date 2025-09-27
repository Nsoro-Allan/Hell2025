#pragma once
#include "JSON.h"
#include "AssetManagement/AssetManager.h"
#include "Util.h"
#include <fstream>

std::vector<WallCreateInfo> walls;

namespace nlohmann {
    void to_json(nlohmann::json& j, const glm::vec3& v) {
        j = json::array({ v.x, v.y, v.z });
    }

    void to_json(nlohmann::json& j, const DoorCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Position", createInfo.position},
            {"Rotation", createInfo.rotation}
        };
    }

    void to_json(nlohmann::json& j, const HouseLocation& houseLocation) {
        j = nlohmann::json{
            {"Position", houseLocation.position},
            {"Rotation", houseLocation.rotation},
            {"Type", Util::HouseTypeToString(houseLocation.type)}
        };
    }

    void to_json(nlohmann::json& j, const LightCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Color", createInfo.color},
            {"Position", createInfo.position},
            {"Radius", createInfo.radius},
            {"Strength", createInfo.strength},
            {"Type", Util::LightTypeToString(createInfo.type)},
        };
    }

    void to_json(nlohmann::json& j, const PianoCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Position", createInfo.position},
            {"Rotation", createInfo.rotation}
        };
    }

    void to_json(nlohmann::json& j, const PictureFrameCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Position", createInfo.position},
            {"Rotation", createInfo.rotation},
            {"Scale", createInfo.scale},
            {"Type", Util::PictureFrameTypeToString(createInfo.type)},
        };
    }

    void to_json(nlohmann::json& j, const PlaneCreateInfo& createInfo) {
        j = nlohmann::json{
            {"P0", createInfo.p0},
            {"P1", createInfo.p1},
            {"P2", createInfo.p2},
            {"P3", createInfo.p3},
            {"TextureScale", createInfo.textureScale},
            {"TextureOffsetU", createInfo.textureOffsetU},
            {"TextureOffsetV", createInfo.textureOffsetV},
            {"TextureRotation", createInfo.textureRotation},
            {"Material", createInfo.materialName},
        };
    }

    void to_json(nlohmann::json& j, const SpawnPoint& spawnPoint) {
        j = nlohmann::json{
            {"Position", spawnPoint.GetPosition()},
            {"CamEuler", spawnPoint.GetCamEuler()},
        };
    }

    void to_json(nlohmann::json& j, const WallCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Height", createInfo.height},
            {"Material", createInfo.materialName},
            {"Points", createInfo.points},
            {"TextureScale", createInfo.textureScale},
            {"TextureOffsetU", createInfo.textureOffsetU},
            {"TextureOffsetV", createInfo.textureOffsetV},
            {"TextureRotation", createInfo.textureRotation},
            {"TrimTypeCeiling", Util::TrimTypeToString(createInfo.ceilingTrimType)},
            {"TrimTypeFloor",  Util::TrimTypeToString(createInfo.floorTrimType)},
            {"UseReversePointOrder", createInfo.useReversePointOrder},
            {"WallType",  Util::WallTypeToString(createInfo.wallType)}
        };
    }

    void to_json(nlohmann::json& j, const WindowCreateInfo& createInfo) {
        j = nlohmann::json{
            {"Position", createInfo.position},
            {"Rotation", createInfo.rotation}
        };
    }

    void to_json(nlohmann::json& j, const MeshRenderingInfo& info) {
        Material* material = AssetManager::GetMaterialByIndex(info.materialIndex);
        Mesh* mesh = AssetManager::GetMeshByIndex(info.meshIndex);

        if (!material) return;
        if (!mesh) return;

        j = nlohmann::json{
            {"meshName", mesh->GetName()},
            {"materialName", material ? material->m_name : DEFAULT_MATERIAL_NAME },
            {"blendingMode", Util::BlendingModeToString(info.blendingMode)}
        };
    }

    void to_json(nlohmann::json& j, const std::map<ivecXZ, std::string>& mapData) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& m : mapData) {
            nlohmann::json item;
            item["name"] = m.second;   // sector name
            item["x"] = m.first.x;     // x coordinate
            item["z"] = m.first.z;     // z coordinate
            arr.push_back(item);
        }
        j = arr;
    }

    void from_json(const nlohmann::json& j, DoorCreateInfo& info) {
        info.position = j.value("Position", glm::vec3(0.0f));
        info.rotation = j.value("Rotation", glm::vec3(0.0f));
    }

    void from_json(const nlohmann::json& j, HouseLocation& houseLocation) {
        houseLocation.position = j.value("Position", glm::vec3(0.0f));
        houseLocation.rotation = j.value("Rotation", 0.0f);
        houseLocation.type = Util::StringToHouseType(j.value("Type", "UNDEFINED"));
    }

    void from_json(const nlohmann::json& j, LightCreateInfo& info) {
        info.color = j.value("Color", glm::vec3(1.0f));
        info.position = j.value("Position", glm::vec3(0.0f));
        info.radius = j.value("Radius", 1.0f);
        info.strength = j.value("Strength", 1.0f);
        info.type = Util::StringToLightType(j.value("Type", "HANGING_LIGHT"));
    }

    void from_json(const nlohmann::json& j, PianoCreateInfo& info) {
        info.position = j.value("Position", glm::vec3(0.0f));
        info.rotation = j.value("Rotation", glm::vec3(0.0f));
    }

    void from_json(const nlohmann::json& j, PictureFrameCreateInfo& info) {
        info.position = j.value("Position", glm::vec3(0.0f));
        info.rotation = j.value("Rotation", glm::vec3(0.0f));
        info.scale = j.value("Scale", glm::vec3(0.0f));
        info.type = Util::StringToPictureFrameType(j.value("Type", "UNDEFINED"));
    }

    void from_json(const nlohmann::json& j, PlaneCreateInfo& info) {
        info.p0 = j.value("P0", glm::vec3(0.0f));
        info.p1 = j.value("P1", glm::vec3(0.0f));
        info.p2 = j.value("P2", glm::vec3(0.0f));
        info.p3 = j.value("P3", glm::vec3(0.0f));
        info.textureScale = j.value("TextureScale", 1.0f);
        info.textureOffsetU = j.value("TextureOffsetU", 0.0f);
        info.textureOffsetV = j.value("TextureOffsetV", 0.0f);
        info.textureRotation = j.value("TextureRotation", 0.0f);
        info.materialName = j.value("Material", "CheckerBoard");
    }

    void from_json(const nlohmann::json& j, WallCreateInfo& info) {
        info.height = j.value("Height", 2.4f);
        info.materialName = j.value("Material", "CheckerBoard");
        info.points = j.value("Points", std::vector<glm::vec3>{});
        info.textureScale = j.value("TextureScale", 1.0f);
        info.textureOffsetU = j.value("TextureOffsetU", 0.0f);
        info.textureOffsetV = j.value("TextureOffsetV", 0.0f);
        info.textureRotation = j.value("TextureRotation", 0.0f);
        info.ceilingTrimType = Util::StringToTrimType(j.value("TrimTypeCeiling", "NONE"));
        info.floorTrimType = Util::StringToTrimType(j.value("TrimTypeFloor", "NONE"));
        info.wallType = Util::StringToWallType(j.value("WallType", "NONE"));
        info.useReversePointOrder = j.value("UseReversePointOrder", false);
    }

    void from_json(const nlohmann::json& j, WindowCreateInfo& info) {
        info.position = j.value("Position", glm::vec3(0.0f));
        info.rotation = j.value("Rotation", glm::vec3(0.0f));
    }

    void from_json(const nlohmann::json& j, SpawnPoint& spawnPoint) {
        glm::vec3 position = j.value("Position", glm::vec3(0.0f));
        glm::vec3 camEuler = j.value("CamEuler", glm::vec3(0.0f));
        spawnPoint = SpawnPoint(position, camEuler);
    }

    void from_json(const nlohmann::json& j, glm::vec3& v) {
        try {
            std::array<float, 3> arr = j.get<std::array<float, 3>>();
            v = glm::vec3(arr[0], arr[1], arr[2]);
        }
        catch (const nlohmann::json::exception& e) {
            v = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    void from_json(const nlohmann::json& j, MeshRenderingInfo& info) {
        std::string meshName;
        std::string materialName;
        std::string blendingModeString;

        j.at("meshName").get_to(meshName);
        j.at("materialName").get_to(materialName);
        j.at("blendingMode").get_to(blendingModeString);

        info.meshIndex = AssetManager::GetMeshIndexByName(meshName);
        info.materialIndex = AssetManager::GetMaterialIndexByName(materialName);
        info.blendingMode = Util::StringToBlendingMode(blendingModeString);
    }

    void from_json(const nlohmann::json& j, std::map<ivecXZ, std::string>& mapData) {
        mapData.clear();
        for (const auto& item : j) {
            int x = item.at("x").get<int>();
            int z = item.at("z").get<int>();
            std::string sectorName = item.at("name").get<std::string>();
            ivecXZ key(x, z);
            mapData[key] = sectorName;
        }
    }

    void from_json(const json& j, glm::mat4& m) {
        std::array<float, 16> a = j.get<std::array<float, 16>>();
        m = glm::make_mat4(a.data());
    }

    void from_json(const json& j, glm::quat& q) {
        if (j.is_array() && j.size() == 4) {
            auto a = j.get<std::array<float, 4>>();
            q = glm::quat{ a[3], a[0], a[1], a[2] };
        }
        else {
            q = glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f };
        }
    }
}

namespace JSON {

    bool LoadJsonFromFile(nlohmann::json& json, const std::string filepath) {
        // Open the file
        std::ifstream file(filepath);
        if (!file) {
            std::cout << "JSON::LoadJsonFromFile() failed to open file: " << filepath << "\n";
            return false;
        }

        // Read the entire file into a string stream
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        // Try to parse the JSON
        try {
            json = nlohmann::json::parse(buffer.str());
            return true;
        }
        catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON::LoadJsonFromFile() failed to parse the file " << filepath << ": " << e.what() << "\n";
            return false;
        }

    }

    SectorCreateInfo LoadSector(const std::string& filepath) {
        SectorCreateInfo info;

        // Try to parse the JSON
        nlohmann::json json;
        if (!LoadJsonFromFile(json, filepath)) {
            std::cerr << "JSON::LoadSector() failed to open file: " << filepath << "\n";
            return info;
        }

        FileInfo fileInfo = Util::GetFileInfoFromPath(filepath);

        SectorCreateInfo sectorCreateInfo;
        sectorCreateInfo.sectorName = fileInfo.name;

        sectorCreateInfo.heightMapName = json.value("HeightMapName", "");
        if (sectorCreateInfo.heightMapName == "None") {
            sectorCreateInfo.heightMapName = "";
        }

        // Load Game Objects
        for (auto& jsonObject : json["GameObjects"]) {
            GameObjectCreateInfo& createInfo = sectorCreateInfo.gameObjects.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.scale = jsonObject["scale"];
            createInfo.modelName = jsonObject["modelName"];
            createInfo.meshRenderingInfoSet = jsonObject["meshRenderingInfo"];
        }

        // Load lights
        for (auto& jsonObject : json["Lights"]) {
            LightCreateInfo& createInfo = sectorCreateInfo.lights.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.color = jsonObject["color"];
            createInfo.radius = jsonObject["radius"];
            createInfo.strength = jsonObject["strength"];
            createInfo.type = Util::StringToLightType(jsonObject["type"]);
        }

        // Load Pickups
        for (auto& jsonObject : json["PickUps"]) {
            PickUpCreateInfo& createInfo = sectorCreateInfo.pickUps.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.pickUpType = jsonObject["type"];
        }

        // Load Trees
        for (auto& jsonObject : json["Trees"]) {
            TreeCreateInfo& createInfo = sectorCreateInfo.trees.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.scale = jsonObject["scale"];
            createInfo.type = Util::StringToTreeType(jsonObject.value<std::string>("type", std::string("TREE_LARGE_0")));
        }

        return sectorCreateInfo;
    }

    void SaveSector(const std::string& filepath, SectorCreateInfo& sectorCreateInfo) {

        nlohmann::json json;

        json["HeightMapName"] = sectorCreateInfo.heightMapName;
        json["Lights"] = nlohmann::json::array();
        json["GameObjects"] = nlohmann::json::array();

        // Save Game Objects
        for (const GameObjectCreateInfo& createInfo : sectorCreateInfo.gameObjects) {
            json["GameObjects"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "scale", createInfo.scale },
                { "modelName", createInfo.modelName },
                { "meshRenderingInfo", createInfo.meshRenderingInfoSet },
            });
        }

        // Save lights
        for (const LightCreateInfo& createInfo : sectorCreateInfo.lights) {
            json["Lights"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "color", createInfo.color },
                { "type", createInfo.type },
                { "radius", createInfo.radius },
                { "strength", createInfo.strength }
            });
        }

        // Save PickUps
        for (const PickUpCreateInfo& createInfo : sectorCreateInfo.pickUps) {
            json["PickUps"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "type", createInfo.pickUpType }
            });
        }

        // Save Trees
        for (const TreeCreateInfo& createInfo : sectorCreateInfo.trees) {
            json["Trees"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "scale", createInfo.scale },
                { "type", Util::TreeTypeToString(createInfo.type) }
            });
        }

        JSON::SaveToFile(json, filepath);
    }

    MapCreateInfo LoadMap(const std::string& filepath) {
        MapCreateInfo mapCreateInfo;
        // TODO: 
        std::cout << "TODO: JSON::LoadMap()\n";
        return mapCreateInfo;
    }

    HouseCreateInfo LoadHouse(const std::string& filepath) {
        nlohmann::json json;
        if (!LoadJsonFromFile(json, filepath)) {
            std::cerr << "JSON::LoadHouse() failed to open file: " << filepath << "\n";
            return HouseCreateInfo();
        }

        HouseCreateInfo houseCreateInfo;
        houseCreateInfo.doors = json.value("Doors", std::vector<DoorCreateInfo>{});
        houseCreateInfo.lights = json.value("Lights", std::vector<LightCreateInfo>{});
        houseCreateInfo.planes = json.value("Planes", std::vector<PlaneCreateInfo>{});
        houseCreateInfo.pianos = json.value("Pianos", std::vector<PianoCreateInfo>{});
        houseCreateInfo.pictureFrames = json.value("PictureFrames", std::vector<PictureFrameCreateInfo>{});
        houseCreateInfo.walls = json.value("Walls", std::vector<WallCreateInfo>{});
        houseCreateInfo.windows = json.value("Windows", std::vector<WindowCreateInfo>{});
        return houseCreateInfo;
    }
    
    void SaveHouse(const std::string& filepath, HouseCreateInfo& houseCreateInfo) {
        nlohmann::json json;
        json["Doors"] = houseCreateInfo.doors;
        json["Lights"] = houseCreateInfo.lights;
        json["Planes"] = houseCreateInfo.planes;
        json["Pianos"] = houseCreateInfo.pianos;
        json["PictureFrames"] = houseCreateInfo.pictureFrames;
        json["Walls"] = houseCreateInfo.walls;
        json["Windows"] = houseCreateInfo.windows;
        JSON::SaveToFile(json, filepath);

        //nlohmann::json json;
        //
        //for (const DoorCreateInfo& createInfo : houseCreateInfo.doors) {
        //    json["Doors"].push_back(nlohmann::json(createInfo));
        //}
        //for (const LightCreateInfo& createInfo : houseCreateInfo.lights) {
        //    json["Lights"].push_back(nlohmann::json(createInfo));
        //}
        //for (const PlaneCreateInfo& createInfo : houseCreateInfo.planes) {
        //    json["Planes"].push_back(nlohmann::json(createInfo));
        //}
        //for (const PianoCreateInfo& createInfo : houseCreateInfo.pianos) {
        //    json["Pianos"].push_back(nlohmann::json(createInfo));
        //}
        //for (const WallCreateInfo& createInfo : houseCreateInfo.walls) {
        //    json["Walls"].push_back(nlohmann::json(createInfo));
        //}
        //for (const WindowCreateInfo& createInfo : houseCreateInfo.windows) {
        //    json["Windows"].push_back(nlohmann::json(createInfo));
        //}
        //
        //JSON::SaveToFile(json, filepath);
    }

    void SaveMap(const std::string& filepath, MapCreateInfo& mapCreateInfo) {
        nlohmann::json json;
        json["Name"] = mapCreateInfo.name;
        json["Width"] = mapCreateInfo.width;
        json["Depth"] = mapCreateInfo.depth;
        //json["SectorLocations"] = mapCreateInfo.sectorLocations;
        JSON::SaveToFile(json, filepath);

        std::cout << "TODO: JSON::SaveMap()\n";
    }

    void SaveToFile(nlohmann::json& json, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << json.dump(4);
            file.close();
            std::cout << "Saved " << filepath << "\n";
        }
    }
}