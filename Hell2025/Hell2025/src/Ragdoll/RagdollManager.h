#pragma once
#include <string>
#include "RagdollInfo.h"
#include "RagdollV2.h"

namespace RagdollManager {
    void Init();
    void SpawnRagdoll(glm::vec3 position, glm::vec3 eulerRotation, const std::string& ragdollName);
    void AddForce(uint64_t physicsId, glm::vec3 force);

    RagdollInfo* GetRagdollInfoByName(const std::string& filename);
    RagdollV2* GetRagdollById(uint64_t ragdollId);
    std::vector<RagdollV2>& GetRagdolls();
}
