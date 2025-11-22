#include "Wall.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Modelling/Clipping.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"

Wall::Wall(uint64_t id, const WallCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;
    m_spawnOffset = spawnOffset;

    for (glm::vec3& point : m_createInfo.points) {
        point += spawnOffset.translation;
    }

    UpdateSegmentsTrimsAndVertexData();
}

void Wall::UpdateSegmentsTrimsAndVertexData() {
    CleanUp();

    for (WallSegment& wallSegment : m_wallSegments) {
        wallSegment.CleanUp();
    }
    m_wallSegments.clear();

    //m_points = m_createInfo.points;
    //m_height = m_createInfo.height;
    //m_textureOffsetU = m_createInfo.textureOffsetU;
    //m_textureOffsetV = m_createInfo.textureOffsetV;
    //m_textureScale = m_createInfo.textureScale;
    m_material = AssetManager::GetMaterialByName(m_createInfo.materialName);
    m_ceilingTrimType = m_createInfo.ceilingTrimType;
    m_floorTrimType = m_createInfo.floorTrimType;

    if (m_createInfo.useReversePointOrder) {
        std::reverse(m_createInfo.points.begin(), m_createInfo.points.end());
    }

    for (int i = 0; i < GetPointCount() - 1; i++) {
        const glm::vec3& start = m_createInfo.points[i];
        const glm::vec3& end = m_createInfo.points[i + 1];
        WallSegment& wallSegment = m_wallSegments.emplace_back();
        wallSegment.Init(start, end, m_createInfo.height, m_objectId, m_spawnOffset);
    }

    // Calculate worldspace center
    m_worldSpaceCenter = glm::vec3(0.0f);
    if (!m_createInfo.points.empty()) {
        for (glm::vec3& point : m_createInfo.points) {
            m_worldSpaceCenter += point;
        }
        m_worldSpaceCenter /= m_createInfo.points.size();
    }

    // Create weather boards
    if (m_createInfo.wallType == WallType::WEATHER_BOARDS) {
        CreateWeatherBoards();
        CreateCSGVertexData();
    }
    // Create CSG geometry and trims
    else {
        CreateCSGVertexData();
        CreateTrims();
    }
}

void Wall::FlipFaces() {
    m_createInfo.useReversePointOrder = !m_createInfo.useReversePointOrder;
    UpdateSegmentsTrimsAndVertexData();
}

void Wall::UpdateWorldSpaceCenter(glm::vec3 worldSpaceCenter) {
    glm::vec3 offset = worldSpaceCenter - m_worldSpaceCenter;
    for (glm::vec3& point : m_createInfo.points) {
        point += offset;
    }
    UpdateSegmentsTrimsAndVertexData();
}

bool Wall::AddPointToEnd(glm::vec3 point, bool supressWarning) {
    glm::vec3& previousPoint = m_createInfo.points.back();
    float threshold = 0.05f;
    if (glm::distance(point, previousPoint) < threshold) {
        std::cout << "Wall::AddPoint() failed: new point " << point << " is too close to previous point " << previousPoint << "\n";
        return false;
    }

    m_createInfo.points.push_back(point);
    UpdateSegmentsTrimsAndVertexData();
    return true;
}

bool Wall::UpdatePointPosition(int pointIndex, glm::vec3 position, bool supressWarning) {
    if (pointIndex < 0 || pointIndex >= m_createInfo.points.size()) {
        std::cout << "Wall::UpdatePointPosition() failed: point index " << pointIndex << " out of range of size " << m_createInfo.points.size() << "\n";
    }

    // Threshold check
    float threshold = 0.05f;
    if (pointIndex > 0) {
        glm::vec3& previousPoint = m_createInfo.points[pointIndex - 1];
        if (glm::distance(position, previousPoint) < threshold) {
            std::cout << "Wall::UpdatePointPosition() failed: new point " << position << " is too close to previous point " << previousPoint << "\n";
            return false;
        }
    }
    if (pointIndex < m_createInfo.points.size() - 1) {
        glm::vec3& nextPoint = m_createInfo.points[pointIndex + 1];
        if (glm::distance(position, nextPoint) < threshold) {
            std::cout << "Wall::UpdatePointPosition() failed: new point " << position << " is too close to next point " << nextPoint << "\n";
            return false;
        }
    }

    m_createInfo.points[pointIndex] = position;
    UpdateSegmentsTrimsAndVertexData();
    return true;
}

void Wall::SetMaterial(const std::string& materialName) {
    if (Material* material = AssetManager::GetMaterialByName(materialName)) {
        m_createInfo.materialName = materialName;
        m_material = AssetManager::GetMaterialByName(materialName);
        UpdateSegmentsTrimsAndVertexData();
    }
}

void Wall::SetHeight(float value) {
    m_createInfo.height = value;
    UpdateSegmentsTrimsAndVertexData();
}

void Wall::SetTextureScale(float value) {
    m_createInfo.textureScale = value;
    UpdateSegmentsTrimsAndVertexData();
}

void Wall::SetTextureOffsetU(float value) {
    m_createInfo.textureOffsetU = value;
    UpdateSegmentsTrimsAndVertexData();
}

void Wall::SetTextureOffsetV(float value) {
    m_createInfo.textureOffsetV = value;
    UpdateSegmentsTrimsAndVertexData();
}

void Wall::SetFloorTrimType(TrimType trimType) {
    m_createInfo.floorTrimType = trimType;
    UpdateSegmentsTrimsAndVertexData();
}
void Wall::SetCeilingTrimType(TrimType trimType) {
    m_createInfo.ceilingTrimType = trimType;
    UpdateSegmentsTrimsAndVertexData();
}

const glm::vec3& Wall::GetPointByIndex(int pointIndex) {
    static glm::vec3 invalid = glm::vec3(0.0f);

    if (pointIndex < 0 || pointIndex >= m_createInfo.points.size()) {
        std::cout << "Wall::GetPointByIndex() failed: point index " << pointIndex << " out of range of size " << m_createInfo.points.size() << "\n";
        return invalid;
    }
    return m_createInfo.points[pointIndex];
}

void Wall::CleanUp() {
    for (WallSegment& wallSegment : m_wallSegments) {
        wallSegment.CleanUp();
    }
}

void Wall::CreateTrims() {
    m_trims.clear();
	World::UpdateDoorAndWindowCubeTransforms();
	return;

    // Ceiling
    if (m_ceilingTrimType != TrimType::NONE) {
        for (int i = 0; i < (int)m_createInfo.points.size() - 1; i++) {
            const glm::vec3& start = m_createInfo.points[i];
            const glm::vec3& end = m_createInfo.points[i + 1];

            Transform t;
            t.position = start;
            t.position.y += m_createInfo.height;
            t.rotation.y = Util::EulerYRotationBetweenTwoPoints(start, end);
            t.scale.x = glm::distance(start, end);

            Trim& trim = m_trims.emplace_back();
            trim.Init(t, "TrimCeiling", "Trims");
        }
    }

    // Floor
    if (m_floorTrimType != TrimType::NONE) {
        for (int i = 0; i < (int)m_createInfo.points.size() - 1; i++) {
            const glm::vec3& start = m_createInfo.points[i];
            const glm::vec3& end = m_createInfo.points[i + 1];

            glm::vec3 rayOrigin = start;
            glm::vec3 rayDir = glm::normalize(end - start);
            const float segmentLength = glm::distance(start, end);
            float remaining = segmentLength;
            const float eps = 1e-3f;

            while (remaining > eps) {
                CubeRayResult r = Util::CastCubeRay(rayOrigin, rayDir, World::GetDoorAndWindowCubeTransforms(), remaining);
                if (!r.hitFound) break;

                // Only add a trim up to a NEAR face (entering the cube)
                if (glm::dot(r.hitNormal, rayDir) < 0.0f) {
                    Transform t;
                    t.position = rayOrigin;
                    t.rotation.y = Util::EulerYRotationBetweenTwoPoints(start, end);
                    t.scale.x = r.distanceToHit;
                    if (t.scale.x > eps) {
                        Trim& trim = m_trims.emplace_back();
                        trim.Init(t, "TrimFloor", "Trims");
                    }
                }

                float advance = r.distanceToHit + eps; // step through face
                rayOrigin += rayDir * advance;
                remaining -= advance;
            }

            if (remaining > eps) {
                Transform t;
                t.position = rayOrigin;
                t.rotation.y = Util::EulerYRotationBetweenTwoPoints(rayOrigin, end);
                t.scale.x = remaining;
                Trim& trim = m_trims.emplace_back();
                trim.Init(t, "TrimFloor", "Trims");
            }
        }
    }
}

void Wall::CreateCSGVertexData() {
    for (WallSegment& wallSegment : m_wallSegments) {
        wallSegment.CreateVertexData(World::GetClippingCubes(), m_createInfo.textureOffsetU, m_createInfo.textureOffsetV, m_createInfo.textureScale);
    }
}

void Wall::SubmitRenderItems() {
    // If this wall is exterior, then dont render the CSG geometry, or any trims if you accidentally set it to have trims
    if (m_createInfo.wallType == WallType::WEATHER_BOARDS) {
        return;
    }

    for (WallSegment& wallSegment : m_wallSegments) {
        Mesh* mesh = World::GetHouseMeshByIndex(wallSegment.GetMeshIndex());
        if (!mesh) continue;

        HouseRenderItem renderItem;
        renderItem.baseColorTextureIndex = m_material->m_basecolor;
        renderItem.normalMapTextureIndex = m_material->m_normal;
        renderItem.rmaTextureIndex = m_material->m_rma;
        renderItem.baseVertex = mesh->baseVertex;
        renderItem.baseIndex = mesh->baseIndex;
        renderItem.vertexCount = mesh->vertexCount;
        renderItem.indexCount = mesh->indexCount;
        renderItem.aabbMin = glm::vec4(mesh->aabbMin, 0.0f);
        renderItem.aabbMax = glm::vec4(mesh->aabbMax, 0.0f);
        renderItem.meshIndex = wallSegment.GetMeshIndex();
        RenderDataManager::SubmitRenderItem(renderItem);

        // Outline?
        //if (Editor::GetHoveredObjectId() == m_objectId) {
        //    RenderDataManager::SubmitOutlineRenderItem(renderItem);
        //    //std::cout << "hover found id: " << m_objectId << "\n";
        //}
    }
    //std::cout << "wall::submit() id: " << m_objectId << "\n";

    for (Trim& trim : m_trims) {
        trim.SubmitRenderItem();
    }
}

void Wall::DrawSegmentVertices(glm::vec4 color) {
    for (WallSegment& wallSegment : m_wallSegments) {
        const glm::vec3& p1 = wallSegment.GetStart();
        const glm::vec3& p2 = wallSegment.GetEnd();
        glm::vec3 p3 = wallSegment.GetStart() + glm::vec3(0.0f, wallSegment.GetHeight(), 0.0f);
        glm::vec3 p4 = wallSegment.GetEnd() + glm::vec3(0.0f, wallSegment.GetHeight(), 0.0f);
        Renderer::DrawPoint(p1, color);
        Renderer::DrawPoint(p2, color);
        Renderer::DrawPoint(p3, color);
        Renderer::DrawPoint(p4, color);
    }
}

void Wall::DrawSegmentLines(glm::vec4 color) {
    for (WallSegment& wallSegment : m_wallSegments) {
        const glm::vec3& p1 = wallSegment.GetStart();
        const glm::vec3& p2 = wallSegment.GetEnd();
        glm::vec3 p3 = wallSegment.GetStart() + glm::vec3(0.0f, wallSegment.GetHeight(), 0.0f);
        glm::vec3 p4 = wallSegment.GetEnd() + glm::vec3(0.0f, wallSegment.GetHeight(), 0.0f);
        Renderer::DrawLine(p1, p2, color);
        Renderer::DrawLine(p3, p4, color);
        Renderer::DrawLine(p1, p3, color);
        Renderer::DrawLine(p2, p4, color);

        glm::vec3 midPoint = Util::GetMidPoint(wallSegment.GetStart(), wallSegment.GetEnd()); 
        glm::vec3 normal = wallSegment.GetNormal();
        glm::vec3 projectedMidPoint = midPoint + (normal * 0.2f);
        Renderer::DrawLine(midPoint, projectedMidPoint, color);
    }
}

BoardVertexData Wall::CreateBoardVertexData(glm::vec3 begin, glm::vec3 end, glm::vec3 boardDirection, int yUVOffsetIndex, float xUVOffset) {
    BoardVertexData weatherBoardVertexData;

    Model* model = AssetManager::GetModelByName("WeatherBoard");
    uint32_t meshIndex = model->GetMeshIndices()[0];
    Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);

    std::span<Vertex> verticesSpan = AssetManager::GetMeshVerticesSpan(mesh);
    std::span<uint32_t> indicesSpan = AssetManager::GetMeshIndicesSpan(mesh);

    float boardWidth = glm::distance(begin, end);
    const float meshBoardWidth = 4.0f;

    glm::mat3 rot3 = glm::mat3(Util::GetRotationMat4FromForwardVector(glm::normalize(boardDirection)));

    for (Vertex vertex : verticesSpan) {
        bool isRightEdge = vertex.uv.x > 0.5f;
        if (isRightEdge) {
            vertex.position.x = boardWidth * boardDirection.x;
            vertex.position.z = boardWidth * boardDirection.z;
            vertex.uv.x = boardWidth / meshBoardWidth;
        }

        vertex.normal = glm::normalize(rot3 * vertex.normal);
        vertex.position += begin;

        float uvVerticalOffset = 1.0f / 16.0f;
        vertex.uv.y -= uvVerticalOffset * yUVOffsetIndex;
        vertex.uv.x += xUVOffset;

        weatherBoardVertexData.vertices.push_back(vertex);
    }

    for (uint32_t& index : indicesSpan) {
        weatherBoardVertexData.indices.push_back(index);
    }

    return weatherBoardVertexData;
}


#define WEATHERBOARD_STOP_MESH_HEGIHT 2.6f

void Wall::CreateWeatherBoards() {
    float individialBoardHeight = 0.13f;
    float desiredTotalWallHeight = 2.8f;
    int weatherBoardCount = (int)(desiredTotalWallHeight / individialBoardHeight);
    float actualFinalWallHeight = weatherBoardCount * individialBoardHeight;
    float weathboardStopScale = actualFinalWallHeight / WEATHERBOARD_STOP_MESH_HEGIHT;

    m_weatherBoardstopRenderItems.clear();

    for (WallSegment& wallSegemet : m_wallSegments) {
        glm::vec3 start = wallSegemet.GetStart();
        glm::vec3 end = wallSegemet.GetEnd();

        Transform transform;
        transform.position = start;
        transform.scale.y = weathboardStopScale;
        transform.rotation.y = Util::EulerYRotationBetweenTwoPoints(start, end);

        Material* material = AssetManager::GetMaterialByName("WeatherBoards0");
        Model* model = AssetManager::GetModelByName("WeatherboardStop");

        RenderItem& renderItem = m_weatherBoardstopRenderItems.emplace_back();
        renderItem.modelMatrix = transform.to_mat4();
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = model->GetMeshIndices()[0];
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }

    World::UpdateDoorAndWindowCubeTransforms();

    m_boardVertexDataSet.clear();

    for (WallSegment& wallSegemet : m_wallSegments) {
        int yUVOffsetIndex = -1;

        for (int i = 0; i < weatherBoardCount; i++) {
            yUVOffsetIndex++;
            float xUVOffset = 0.0f;

            if (i > 15) {
                yUVOffsetIndex = 12;
            }
            if (i > 12) {
                xUVOffset = Util::RandomFloat(0.0f, 100.0f);
            }

            glm::vec3 start = wallSegemet.GetStart();
            glm::vec3 end = wallSegemet.GetEnd();

            start.y += individialBoardHeight * i;
            end.y += individialBoardHeight * i;

            glm::vec3 rayOrigin = start;
            glm::vec3 rayDir = glm::normalize(end - start);
            const float segLen = glm::distance(start, end);
            float remaining = segLen;
            const float eps = 1e-3f;

            while (remaining > eps) {
                CubeRayResult r = Util::CastCubeRay(rayOrigin, rayDir, World::GetDoorAndWindowCubeTransforms(), remaining);
                if (!r.hitFound) break;

                if (glm::dot(r.hitNormal, rayDir) < 0.0f && r.distanceToHit > eps) {
                    glm::vec3 localStart = rayOrigin;
                    glm::vec3 localEnd = rayOrigin + (rayDir * r.distanceToHit);
                    m_boardVertexDataSet.emplace_back(CreateBoardVertexData(localStart, localEnd, rayDir, yUVOffsetIndex, xUVOffset));
                }

                float advance = r.distanceToHit + eps;
                rayOrigin += rayDir * advance;
                remaining -= advance;
            }

            if (remaining > eps) {
                m_boardVertexDataSet.emplace_back(CreateBoardVertexData(rayOrigin, end, rayDir, yUVOffsetIndex, xUVOffset));
            }
        }
    }
}


