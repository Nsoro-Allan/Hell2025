#include "MapManager.h"

#include "BackEnd/BackEnd.h"
#include "File/File.h"
#include "HellLogging.h"
#include "Renderer/Renderer.h"
#include "World/World.h"

#include <fstream>

namespace MapManager {
    std::vector<Map> g_maps;

    void Init() {
        //NewMap("Shit", 8, 16, 30.0f);
        LoadMap("Shit");
    }

    void NewMap(const std::string& name, int chunkWidth, int chunkDepth, float initialHeight) {
        Map& map = g_maps.emplace_back();
        map.CreateNew(name, chunkWidth, chunkDepth, initialHeight);
    }

    void SaveMap(const std::string& mapName) {
        Map* map = GetMapByName(mapName);
        if (!map) {
            Logging::Error() << "SaveMap(): failed because '" << mapName << "' was not found.";
            return;
        }

        // Get heightmap data
        if (BackEnd::GetAPI() == API::OPENGL) {
            Renderer::ReadBackHeightMapData(map);
        }
        if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan TODO: MapManager::SaveHeightMap()";
            return;
        }

        int32_t textureWidth = map->GetTextureWidth();
        int32_t textureHeight = map->GetTextureHeight();
        int32_t floatCount = textureWidth * textureHeight;
        int32_t dataSize = map->GetHeightMapData().size();

        // Validate height map data size
        if (dataSize != floatCount) {
            Logging::Error() << "File::SaveHeightMap() failed because map.m_heightMapData.size() is " << dataSize << " but width(" << textureWidth << ") * height(" << textureHeight << ") equals " << floatCount;
            return;
        }

        // Construct the JSON string
        CreateInfoCollection createInfoCollection = World::GetCreateInfoCollection();
        map->SetCreateInfoCollection(createInfoCollection);                             // This can probably go into the close editor function, or something better
        
        std::string createInfoJson = Util::CreateInfoCollectionToJSON(createInfoCollection);
        std::string additionalJson = Util::AdditionalMapDataToJSON(map->GetAdditionalMapData());

        // Create the file
        std::string outputPath = "res/maps/" + mapName + ".map";
        std::ofstream file(outputPath, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Failed to open file for writing: " << outputPath << "\n";
            return;
        }

        // Write the header
        MapHeader header{};
        header.version = 1;
        header.chunkCountX = map->GetChunkCountX();
        header.chunkCountZ = map->GetChunkCountZ();
        header.createInfoJsonLength = createInfoJson.size();
        header.additionalJsonLength = additionalJson.size();
        File::MemCopyFileSignature(header.signature, HELL_MAP_SIGNATURE);
        file.write(reinterpret_cast<const char*>(&header), sizeof(MapHeader));

        // Write the height map pixel data
        file.write(reinterpret_cast<const char*>(map->GetHeightMapData().data()), floatCount * sizeof(float));

        // Write JSON blobs immediately after
        file.write(createInfoJson.data(), static_cast<std::streamsize>(createInfoJson.size()));
        file.write(additionalJson.data(), static_cast<std::streamsize>(additionalJson.size()));

        // Close file
        file.close();

        Logging::Debug()
            << "Saved map '" << mapName << "'\n"
            << createInfoJson << "'\n"
            << additionalJson;
    }

    void LoadMap(const std::string& mapName) {
        const std::string path = "res/maps/" + mapName + ".map";
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            Logging::Error() << "LoadMap(): failed to open '" << path << "'";
            return;
        }

        MapHeader header{};
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (!file) {
            Logging::Error() << "LoadMap(): failed reading header";
            return;
        }

        // Validate header signature
        if (std::memcmp(header.signature, HELL_MAP_SIGNATURE, sizeof(HELL_MAP_SIGNATURE)) != 0) {
            Logging::Error() << "LoadMap(): bad file signature";
            return;
        }

        uint32_t textureWidth = header.chunkCountX * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
        uint32_t textureHeight = header.chunkCountZ * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
        uint32_t floatCount = textureWidth * textureHeight;
        std::vector<float> heightMapData(floatCount);

        // Read height map data
        file.read(reinterpret_cast<char*>(heightMapData.data()), static_cast<std::streamsize>(floatCount * sizeof(float)));
        if (!file) {
            Logging::Error() << "LoadMap(): failed reading height data";
            return;
        }

        Map& map = g_maps.emplace_back();
        map.SetFilename(mapName);
        map.SetHeightMapData(header.chunkCountX, header.chunkCountZ, heightMapData);

        std::string createInfoJson;
        std::string additionalJson;

        createInfoJson.resize(header.createInfoJsonLength);
        additionalJson.resize(header.additionalJsonLength);

        if (header.createInfoJsonLength > 0) {
            file.read(createInfoJson.data(), static_cast<std::streamsize>(header.createInfoJsonLength));
            if (!file) {
                Logging::Error() << "LoadMap(): failed reading create info json";
                return;
            }
        }

        if (header.additionalJsonLength > 0) {
            file.read(additionalJson.data(), static_cast<std::streamsize>(header.additionalJsonLength));
            if (!file) {
                Logging::Error() << "LoadMap(): failed reading additional json";
                return;
            }
        }

        // Load Create Info Collection from JSON string
        CreateInfoCollection createInfoCollection = Util::CreateInfoCollectionFromJSON(createInfoJson);
        map.SetCreateInfoCollection(createInfoCollection);

        Logging::Debug()
            << "Loaded map: " << mapName << "\n"
            << "- signature:     " << header.signature << "\n"
            << "- version:       " << header.version << "\n"
            << "- chunk count x: " << header.chunkCountX << "\n"
            << "- chunk count z: " << header.chunkCountZ << "\n"
            << createInfoJson << "\n"
            << additionalJson;

        return;
    }

    Map* GetTestMap() {
        return GetMapByName("Shit");
    }

    Map* GetMapByIndex(int32_t index) {
        if (index > 0 || index >= g_maps.size()) {
            Logging::Error() << "MapManager::GetMapByIndex() failed coz '" << index << "' is out of range of size " << g_maps.size();
            return nullptr;
        }
        return &g_maps[index];
    }

    Map* GetMapByName(const std::string& name) {
        for (Map& map : g_maps) {
            if (map.GetFilename() == name) {
                return &map;
            }
        }
        Logging::Error() << "MapManager::GetMapByName() failed coz '" << name << "' was not found";
        return nullptr;
    }

    int32_t GetMapIndexByName(const std::string& name) {
        for (int i = 0; i < g_maps.size(); i++) {
            if (g_maps[i].GetFilename() == name) {
                return (int32_t)i;
            }
        }
        Logging::Error() << "MapManager::GetMapIndexByName() failed coz '" << name << "' was not found";
        return -1;
    }
}