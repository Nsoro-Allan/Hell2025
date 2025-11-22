#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Trim.h"
#include "WallSegment.h"
#include <glm/glm.hpp>
#include <vector>


struct BoardVertexData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct Wall {
    Wall() = default;
    Wall(uint64_t id, const WallCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    Wall(const Wall&) = delete;
    Wall& operator=(const Wall&) = delete;
    Wall(Wall&&) noexcept = default;
    Wall& operator=(Wall&&) noexcept = default;
    ~Wall() = default;

    void CleanUp();
    void UpdateSegmentsTrimsAndVertexData();
    void UpdateWorldSpaceCenter(glm::vec3 worldSpaceCenter);
    void SubmitRenderItems();
    void CreateTrims();
    void DrawSegmentVertices(glm::vec4 color);
    void DrawSegmentLines(glm::vec4 color);
    void FlipFaces();
    bool AddPointToEnd(glm::vec3 point, bool supressWarning = true);
    bool UpdatePointPosition(int pointIndex, glm::vec3 position, bool supressWarning = true);

    void SetCeilingTrimType(TrimType trimType);
    void SetFloorTrimType(TrimType trimType);
    void SetHeight(float value);
    void SetTextureScale(float value);
    void SetTextureOffsetU(float value);
    void SetTextureOffsetV(float value);
    void SetMaterial(const std::string& materialName);

    const glm::vec3& GetPointByIndex(int pointIndex); 

    const WallType GetWallType() const                                      { return m_createInfo.wallType; }
    const size_t GetPointCount() const                                      { return m_createInfo.points.size(); }
    const glm::vec3& GetWorldSpaceCenter() const                            { return m_worldSpaceCenter; }
    Material* GetMaterial()                                                 { return m_material; };
    const std::vector<RenderItem>& GetWeatherBoardstopRenderItems()         { return m_weatherBoardstopRenderItems; }
    std::vector<WallSegment>& GetWallSegments()                             { return m_wallSegments; }
    const uint64_t GetObjectId() const                                      { return m_objectId; }
    const WallCreateInfo& GetCreateInfo() const                             { return m_createInfo; }
    const std::string& GetEditorName() const                                { return m_createInfo.editorName; }


    std::vector<BoardVertexData> m_boardVertexDataSet;


private:
    uint64_t m_objectId = 0;
    Material* m_material = nullptr;
    TrimType m_ceilingTrimType = TrimType::NONE;
    TrimType m_floorTrimType = TrimType::NONE;
    glm::vec3 m_worldSpaceCenter = glm::vec3(0.0f);
    std::vector<RenderItem> m_weatherBoardstopRenderItems;
    std::vector<WallSegment> m_wallSegments;
    std::vector<Trim> m_trims;
    WallCreateInfo m_createInfo;
    SpawnOffset m_spawnOffset;

    void CreateCSGVertexData();
    void CreateWeatherBoards();
    BoardVertexData CreateBoardVertexData(glm::vec3 begin, glm::vec3 end, glm::vec3 boardDirection, int yUVOffsetIndex, float xUVOffset);

};