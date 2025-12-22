#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshBuffer.h"
#include "Types/Generics/Wire.h"

struct ChristmasLights {
    ChristmasLights() = default;
    ChristmasLights(uint64_t id, ChristmasLightsCreateInfo& createInfo, SpawnOffset& spawnOffset);
    ChristmasLights(const ChristmasLights&) = delete;
    ChristmasLights& operator=(const ChristmasLights&) = delete;
    ChristmasLights(ChristmasLights&&) noexcept = default;
    ChristmasLights& operator=(ChristmasLights&&) noexcept = default;
    ~ChristmasLights() = default;

    void AddSegementFromLastPoint(const glm::vec3& nextPoint, float sag);
    void Update(float deltaTime);
    void RecreateLightRenderItems();
    void CleanUp();

    //glm::vec3 m_start;
    //glm::vec3 m_end;
    //float m_sag = 1.0f;
    bool m_spiral = false;
    glm::vec3 sprialTopCenter;
    float spiralRadius;
    float spiralHeight;
    float m_time = 0;

    MeshBuffer m_meshBuffer;
    std::vector<glm::vec3> m_wireSegmentPoints;
    std::vector<glm::vec3> m_lightSpawnPoints;

    std::vector<Wire>& GetWires()                           { return m_wires; }

    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }
    const ChristmasLightsCreateInfo& GetCreateInfo() const  { return m_createInfo; }
    const uint64_t GetObjectId() const                      { return m_objectId; }
    const glm::vec3& GetPosition() const                    { return m_position; }
    const glm::vec3& GetRotation() const                    { return m_rotation; }

private:
    std::vector<RenderItem> m_renderItems;
    ChristmasLightsCreateInfo m_createInfo;
    uint64_t m_objectId = 0;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f); 
    std::vector<Wire> m_wires;
};