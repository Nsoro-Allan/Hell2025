#pragma once
#include "HellTypes.h"
#include "Types/Game/SpawnPoint.h"
#include "CreateInfo.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace nlohmann {
    void to_json(nlohmann::json& j, const DoorCreateInfo& info);
    void to_json(nlohmann::json& j, const DrawersCreateInfo& info);
    void to_json(nlohmann::json& j, const HouseLocation& houseLocation);
    void to_json(nlohmann::json& j, const LightCreateInfo& info);
    void to_json(nlohmann::json& j, const MeshRenderingInfo& info);
    void to_json(nlohmann::json& j, const PianoCreateInfo& info);
    void to_json(nlohmann::json& j, const PickUpCreateInfo& info);
    void to_json(nlohmann::json& j, const PictureFrameCreateInfo& info);
    void to_json(nlohmann::json& j, const PlaneCreateInfo& info);
    void to_json(nlohmann::json& j, const SpawnPoint& spawnPoint);
    void to_json(nlohmann::json& j, const TreeCreateInfo& info);
    void to_json(nlohmann::json& j, const WallCreateInfo& info);
    void to_json(nlohmann::json& j, const WindowCreateInfo& info);

    void from_json(const nlohmann::json& j, DoorCreateInfo& info);
    void from_json(const nlohmann::json& j, DrawersCreateInfo& info);
    void from_json(const nlohmann::json& j, HouseLocation& houseLocation);
    void from_json(const nlohmann::json& j, LightCreateInfo& info);
    void from_json(const nlohmann::json& j, MeshRenderingInfo& info);
    void from_json(const nlohmann::json& j, PianoCreateInfo& info);
    void from_json(const nlohmann::json& j, PickUpCreateInfo& info);
    void from_json(const nlohmann::json& j, PictureFrameCreateInfo& info);
    void from_json(const nlohmann::json& j, PlaneCreateInfo& info);
    void from_json(const nlohmann::json& j, SpawnPoint& info);
    void from_json(const nlohmann::json& j, TreeCreateInfo& info);
    void from_json(const nlohmann::json& j, WallCreateInfo& info);
    void from_json(const nlohmann::json& j, WindowCreateInfo& info);

    void to_json(nlohmann::json& j, const glm::vec3& v);
    void to_json(nlohmann::json& j, const std::map<ivecXZ, std::string>& map);

    void from_json(const json& j, glm::mat4& m);
    void from_json(const json& j, glm::quat& q);

    void from_json(const nlohmann::json& j, glm::vec3& v);
    void from_json(const nlohmann::json& j, MeshRenderingInfo& info);
}

namespace JSON {
    bool LoadJsonFromFile(nlohmann::json& json, const std::string filepath);
    void SaveToFile(nlohmann::json& json, const std::string& filepath);

    void SaveHouse(const std::string& filepath, HouseCreateInfo& houseCreateInfo);
    void SaveMap(const std::string& filepath, MapCreateInfo& mapCreateInfo);
    void SaveSector(const std::string& filepath, SectorCreateInfo& sectorCreateInfo);

    HouseCreateInfo LoadHouse(const std::string& filepath);
    MapCreateInfo LoadMap(const std::string& filepath);
    SectorCreateInfo LoadSector(const std::string& filepath);

}