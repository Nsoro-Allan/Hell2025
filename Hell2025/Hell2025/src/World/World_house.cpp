#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Core/JSON.h"
#include "Managers/HouseManager.h"
#include "Renderer/GlobalIllumination.h"
#include "Renderer/RenderDataManager.h"

namespace World {

    MeshBuffer g_houseMeshBuffer;
    MeshBuffer g_weatherBoardMeshBuffer;
    
    void AddHouse(HouseCreateInfo houseCreateInfo, SpawnOffset spawnOffset) {
//     for (DoorCreateInfo& createInfo : houseCreateInfo.doors) {
//         AddDoor(createInfo, spawnOffset);
//     }
//     for (PlaneCreateInfo& createInfo : houseCreateInfo.planes) {
//         AddHousePlane(createInfo, spawnOffset);
//     }
//     for (LightCreateInfo& createInfo : houseCreateInfo.lights) {
//         AddLight(createInfo, spawnOffset);
//     }
//     for (PianoCreateInfo& createInfo : houseCreateInfo.pianos) {
//         AddPiano(createInfo, spawnOffset);
//     }
//     for (PictureFrameCreateInfo& createInfo : houseCreateInfo.pictureFrames) {
//         AddPictureFrame(createInfo, spawnOffset);
//     }
//     for (WindowCreateInfo& createInfo : houseCreateInfo.windows) {
//         AddWindow(createInfo, spawnOffset);
//     }
//
//     // Update the clipping cubes, so the walls have correct cut outs for doors/windows
//     UpdateClippingCubes();
//     
//     for (WallCreateInfo& createInfo : houseCreateInfo.walls) {
//         AddWall(createInfo, spawnOffset);
//     }
//
//     UpdateHouseMeshBuffer();
//     UpdateWeatherBoardMeshBuffer();
//
//     //GlobalIllumination::CreatePointCloud();
//
//     // Hack in a Christmas tree
//     ChristmasTreeCreateInfo christmasTreeCreateInfo;
//     christmasTreeCreateInfo.position = glm::vec3(8.13f, 0.15f, 1.2f);
//     christmasTreeCreateInfo.rotation.y = Util::RandomFloat(0, HELL_PI);
//     AddChristmasTree(christmasTreeCreateInfo, spawnOffset);
//
//     christmasTreeCreateInfo.position = glm::vec3(0.78f, 0.15f, 2.25f);
//     christmasTreeCreateInfo.rotation.y = Util::RandomFloat(0, HELL_PI);
//     AddChristmasTree(christmasTreeCreateInfo, spawnOffset);
//
//  
//     ChristmasLightsCreateInfo christmasLightsCreateInfo;
//     christmasLightsCreateInfo.start = glm::vec3(4.4, 2.13241, 0.861675);
//     christmasLightsCreateInfo.end = glm::vec3(2.99485, 2.16198, 2.9);
//     christmasLightsCreateInfo.spiral = false;
//     christmasLightsCreateInfo.sag = 0.6f;
//     World::AddChristmasLights(christmasLightsCreateInfo, spawnOffset);
//
//
//     glm::vec3 couchSpawnPosition = glm::vec3(0.02f, 0.0f, -1.5f);
//     glm::vec3 couchSpawnRotation = glm::vec3(0.0f, HELL_PI * 0.5f, 0.0f);
//
//     GenericStaticCreateInfo genericStaticCreateInfo;
//     genericStaticCreateInfo.position = couchSpawnPosition;
//     genericStaticCreateInfo.rotation = couchSpawnRotation;
//     genericStaticCreateInfo.type = GenericStaticType::COUCH;
//     World::AddGenericStatic(genericStaticCreateInfo, spawnOffset);
//
//
//     GenericBouncableCreateInfo genericBouncableCreateInfo;
//     genericBouncableCreateInfo.position = couchSpawnPosition;
//     genericBouncableCreateInfo.rotation = couchSpawnRotation;
//     genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
//     genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_0;
//     World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
//
//     genericBouncableCreateInfo.position = couchSpawnPosition;
//     genericBouncableCreateInfo.rotation = couchSpawnRotation;
//     genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
//     genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_0;
//     World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
//
//     genericBouncableCreateInfo.position = couchSpawnPosition;
//     genericBouncableCreateInfo.rotation = couchSpawnRotation;
//     genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
//     genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_1;
//     World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
//
//     genericBouncableCreateInfo.position = couchSpawnPosition;
//     genericBouncableCreateInfo.rotation = couchSpawnRotation;
//     genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
//     genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_2;
//     World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
//
//     genericBouncableCreateInfo.position = couchSpawnPosition;
//     genericBouncableCreateInfo.rotation = couchSpawnRotation;
//     genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
//     genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_3;
//     World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
//
//
//     ToiletCreateInfo toiletCreateInfo;
//     toiletCreateInfo.position = glm::vec3(4.40f, 0.0f, -0.7f);
//     toiletCreateInfo.rotation.y = HELL_PI * 0.5f;
//     World::AddToilet(toiletCreateInfo, spawnOffset);
//
//     DrawersCreateInfo drawersCreateInfo;
//     drawersCreateInfo.position = glm::vec3(8.65f, 0.0f, -0.6f);
//     drawersCreateInfo.rotation.y = HELL_PI * 1.5f;
//     drawersCreateInfo.type = DrawersType::LARGE;
//     World::AddDrawers(drawersCreateInfo, spawnOffset);
//
//     drawersCreateInfo.position = glm::vec3(4.50f, 0.0f, -0.85f);
//     drawersCreateInfo.rotation.y = HELL_PI * 0.5f;
//     drawersCreateInfo.type = DrawersType::SMALL;
//     World::AddDrawers(drawersCreateInfo, spawnOffset);
//

   //  ToiletCreateInfo toiletCreateInfo2;
   //  toiletCreateInfo2.position = glm::vec3(0.1f, 0.0f, 4.5f);
   //  toiletCreateInfo2.rotation.y = HELL_PI * 1.5f;
   //  World::AddToilet(toiletCreateInfo2, spawnOffset);

    }

    void SaveHouse() {

        std::string filename = "TestHouse.json";

        HouseCreateInfo houseCreateInfo;

        for (Door& door : GetDoors()) {
            houseCreateInfo.doors.emplace_back(door.GetCreateInfo());
        }
        for (Light& light : GetLights()) {
            houseCreateInfo.lights.emplace_back(light.GetCreateInfo());
        }
        for (Piano& piano : GetPianos()) {
            houseCreateInfo.pianos.emplace_back(piano.GetCreateInfo());
        }
        for (Plane& plane : GetPlanes()) {
            houseCreateInfo.planes.emplace_back(plane.GetCreateInfo());
        }
        for (PictureFrame& pictureFrame : GetPictureFrames()) {
            houseCreateInfo.pictureFrames.emplace_back(pictureFrame.GetCreateInfo());
        }
        for (Window& window : GetWindows()) {
            houseCreateInfo.windows.emplace_back(window.GetCreateInfo());
        }
        for (Wall& wall: GetWalls()) {
            houseCreateInfo.walls.emplace_back(wall.GetCreateInfo());
        }

        JSON::SaveHouse("res/houses/" + filename, houseCreateInfo);
    }

    // Find out why this isn't required for windows and doors, yet still somehow updates all this shit
    void RecreateHouseMesh() {
        UpdateClippingCubes();
        UpdateAllWallCSG();
        UpdateHouseMeshBuffer();
        UpdateWeatherBoardMeshBuffer();
    }

    void UpdateHouseMeshBuffer() {
        g_houseMeshBuffer.Reset();

        for (Wall& wall : GetWalls()) {
            for (WallSegment& wallSegment : wall.GetWallSegments()) {
                uint32_t meshIndex = g_houseMeshBuffer.AddMesh(wallSegment.GetVertices(), wallSegment.GetIndices());
                wallSegment.SetMeshIndex(meshIndex);
            }
        }
        for (Plane& housePlane : GetPlanes()) {
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