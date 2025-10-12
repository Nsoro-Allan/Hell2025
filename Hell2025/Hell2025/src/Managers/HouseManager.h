#pragma once
//#include "CreateInfo.h"
#include "Types/House/House.h"

namespace HouseManager {
    void Init();
    void LoadHouse(const std::string& filename);
    void SaveHouse(const std::string& filename);
    void UpdateCreateInfoCollectionFromWorld(const std::string& houseName);

    House* GetHouseByName(const std::string& filename);
}