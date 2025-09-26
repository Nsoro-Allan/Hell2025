#include "SectorManager.h"
#include "Core/JSON.h"
#include "World/World.h"
#include "Util.h"

namespace SectorManager {
    std::vector<SectorCreateInfo> g_sectorCreateInfoSet;

    void CreateHeightMapEditorSector(const std::string& name) {
        SectorCreateInfo& heightMapEditorSectorCreateInfo = g_sectorCreateInfoSet.emplace_back();
        heightMapEditorSectorCreateInfo.heightMapName = "None";
        heightMapEditorSectorCreateInfo.sectorName = name;
    }

    void LoadSectorsFromDisk() {
        g_sectorCreateInfoSet.clear();

        // Load sectors from disk
        std::vector<FileInfo> files = Util::IterateDirectory("res/sectors");
        for (FileInfo& file : files) {
            SectorCreateInfo sectorCreateInfo = JSON::LoadSector(file.path);
            g_sectorCreateInfoSet.emplace_back(sectorCreateInfo);
            //std::cout << "Loaded sector: '" << file.path << "' with height map: '" << sectorCreateInfo.heightMapName << "'\n";
        }
    }

    void NewSector(const std::string& name) {
        if (name == "") return;
        SectorCreateInfo& sectorCreateInfo = g_sectorCreateInfoSet.emplace_back();
        sectorCreateInfo.sectorName = name;
        sectorCreateInfo.heightMapName = "None";
        SaveSector(name);
    }

    void UpdateSectorFromDisk(const std::string& filename) {
        std::vector<FileInfo> files = Util::IterateDirectory("res/sectors");
        for (FileInfo& file : files) {
            if (file.name == filename) {
                for (SectorCreateInfo& sectorCreateInfo : g_sectorCreateInfoSet) {
                    if (sectorCreateInfo.sectorName == filename) {
                        sectorCreateInfo = JSON::LoadSector(file.path);
                    }
                }
            }
        }
    }
    
    void SaveSector(const std::string& name) {        
        SectorCreateInfo* sectorCreateInfo = GetSectorCreateInfoByName(name);
        if (sectorCreateInfo) {
            JSON::SaveSector("res/sectors/" + name + ".json", *sectorCreateInfo);
        }
    }

    SectorCreateInfo* GetSectorCreateInfoByName(const std::string& name) {
        for (SectorCreateInfo& sectorCreateInfo : g_sectorCreateInfoSet) {
            if (sectorCreateInfo.sectorName == name) {
                return &sectorCreateInfo;
            }
        }
        return nullptr;
    }

    const std::string& GetSectorHeightMapName(const std::string& sectorName) {
        SectorCreateInfo* sectorCreateInfo = GetSectorCreateInfoByName(sectorName);
        if (sectorCreateInfo) {
            return sectorCreateInfo->heightMapName;
        } 
        else {
            static std::string empty = "None";
            return empty;
        }
    }
}