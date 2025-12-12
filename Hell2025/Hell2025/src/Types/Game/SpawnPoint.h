#pragma once
#include "HellTypes.h"

struct SpawnPoint {
    SpawnPoint() = default;
    SpawnPoint(const glm::vec3& position, const glm::vec3& camEuler);
    void Init();
    void CleanUp();
    void DrawDebugCube();

    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetCamEuler() const { return m_camEuler; }

private:
    uint64_t m_objectId = 0;
    //uint64_t m_rigidStaticId = 0;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_camEuler = glm::vec3(0.0f);
};