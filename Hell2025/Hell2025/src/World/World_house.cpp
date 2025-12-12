#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Core/JSON.h"
#include "Managers/HouseManager.h"
#include "Renderer/GlobalIllumination.h"
#include "Renderer/RenderDataManager.h"

namespace World {

    MeshBuffer g_houseMeshBuffer;
    MeshBuffer g_weatherBoardMeshBuffer;
    
    // Find out why this isn't required for windows and doors, yet still somehow updates all this shit
    void RecreateHouseMesh() {
        UpdateClippingCubes();
        UpdateAllWallCSG();
        UpdateHouseMeshBuffer();
        UpdateWeatherBoardMeshBuffer();
        UpdateAllHangingLightCords();
        UpdateTrims();
    }

    void UpdateTrims() {
        Hell::SlotMap<TrimSet>& trimSets = GetTrimSets();
        trimSets.clear();

		for (Wall& wall : GetWalls()) {
            if (wall.GetWallType() == WallType::WEATHER_BOARDS) continue;

			const WallCreateInfo& createInfo = wall.GetCreateInfo();

			// Ceiling trim
			TrimSetCreateInfo createInfoCeiling;
			for (const glm::vec3& point : createInfo.points) {
				glm::vec3 trimPoint = point + glm::vec3(0.0f, createInfo.height, 0.0f);
                trimPoint.y -= 0.01f; // safety threshold
				createInfoCeiling.points.push_back(trimPoint);
				createInfoCeiling.type = TrimSetType::CEILING_FANCY;
                createInfoCeiling.trimScale = 0.95f;
			}

			World::AddTrimSet(createInfoCeiling, SpawnOffset());
		}
    }

    void UpdateHouseMeshBuffer() {
        g_houseMeshBuffer.Reset();

        for (Wall& wall : GetWalls()) {
            for (WallSegment& wallSegment : wall.GetWallSegments()) {
                uint32_t meshIndex = g_houseMeshBuffer.AddMesh(wallSegment.GetVertices(), wallSegment.GetIndices());
                wallSegment.SetMeshIndex(meshIndex);
            }
        }
        for (HousePlane& housePlane : GetHousePlanes()) {
            uint32_t meshIndex = g_houseMeshBuffer.AddMesh(housePlane.GetVertices(), housePlane.GetIndices());
            housePlane.SetMeshIndex(meshIndex);
        }

        g_houseMeshBuffer.UpdateBuffers();
    }

    void UpdateWeatherBoardMeshBuffer() {
        g_weatherBoardMeshBuffer.Reset();

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (Wall& wall : GetWalls()) {

            for (BoardVertexData& boardVertexData : wall.m_boardVertexDataSet) {
                uint32_t baseVertex = vertices.size();
                vertices.insert(vertices.end(), boardVertexData.vertices.begin(), boardVertexData.vertices.end());

                for (uint32_t& index : boardVertexData.indices) {
                    indices.push_back(index + baseVertex);
                }
            }
        }

        g_weatherBoardMeshBuffer.GetGLMeshBuffer().ReleaseBuffers();
        g_weatherBoardMeshBuffer.GetGLMeshBuffer().UpdateBuffers(vertices, indices);
    }

    void ResetWeatherboardMeshBuffer() {
        g_weatherBoardMeshBuffer.Reset();
    }

    void UpdateAllHangingLightCords() {
        for (Light& light : GetLights()) {
            light.ConfigureMeshNodes();
        }
    }

    MeshBuffer& GetHouseMeshBuffer() {
        return g_houseMeshBuffer;
    }

    MeshBuffer& GetWeatherBoardMeshBuffer() {
        return g_weatherBoardMeshBuffer;
    }

    Mesh* GetHouseMeshByIndex(uint32_t meshIndex) {
        return g_houseMeshBuffer.GetMeshByIndex(meshIndex);
    }
}