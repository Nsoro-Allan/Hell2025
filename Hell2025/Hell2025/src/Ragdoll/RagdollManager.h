#pragma once
#include <string>
#include "RagdollInfo.h"
#include "RagdollV2.h"
#include <unordered_map>

namespace RagdollManager {
    void Init();
    uint64_t SpawnRagdoll(glm::vec3 position, glm::vec3 eulerRotation, const std::string& ragdollName);
    void AddForce(uint64_t physicsId, glm::vec3 force);

    RagdollInfo* GetRagdollInfoByName(const std::string& filename);
    RagdollV2* GetRagdollV2ById(uint64_t ragdollId);
    std::unordered_map<uint64_t, RagdollV2>& GetRagdolls();
}
