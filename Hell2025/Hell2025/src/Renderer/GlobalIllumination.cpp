#include "GlobalIllumination.h"
#include "AssetManagement/AssetManager.h"
#include "Bvh/Gpu/Bvh.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Physics/Physics.h"
#include "World/World.h"

#include "Core/Game.h" 

namespace GlobalIllumination {

    float g_probeSpacing = 0.75f;

    struct Triangle {
        glm::vec3 v0;
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 normal;
    };

    void CreatePointCloud();
    void CreateDoorBvh();
    void CreateHouseBvh();
    void UpdateSceneBVh();

    void InitPointGrid(); // Rename me and put me somewhere better

    inline float RoundUp(float value, float spacing)    { return std::ceil(value / spacing) * spacing; }
    inline float RoundDown(float value, float spacing)  { return std::floor(value / spacing) * spacing; }

    uint64_t g_houseBvhId = 0;
    uint64_t g_doorBvhId = 0;
    uint64_t g_sceneBvhId = 0;

    glm::vec3 g_houseMinBounds = glm::vec3(0.0f);
    glm::vec3 g_houseMaxBounds = glm::vec3(0.0f);

    std::vector<LightVolume> g_lightVolumes;
    std::vector<Triangle> g_triangles;
    std::vector<CloudPoint> g_pointCloud;
    bool g_globalIlluminationStructuresDirty = false;
    bool g_pointCloudNeedsGpuUpdate = false;

    std::vector<PointCloudOctrant> g_pointCloudOctrants;
    std::vector<unsigned int> g_pointIndices;
    constexpr float g_pointCloudOctrantSpacing = 5.0f;
    glm::uvec3 g_pointCloudGridDimensions;
    glm::vec3 g_pointGridWorldMin;
    glm::vec3 g_pointGridWorldMax;
    glm::vec3 g_pointCloudOctrantSize;

    void Update() {
        if (g_globalIlluminationStructuresDirty) {
            GlobalIllumination::CreatePointCloud();
            GlobalIllumination::CreateHouseBvh();
            if (g_doorBvhId == 0) {
                CreateDoorBvh();
            }
            if (g_sceneBvhId == 0) {
                g_sceneBvhId = Bvh::Gpu::CreateNewSceneBvh();
            }
            Bvh::Gpu::FlatternMeshBvhNodes();
            g_globalIlluminationStructuresDirty = false;
        }

        UpdateSceneBVh();
    }

    void CreatePointCloud() {
        g_triangles.clear();

        // Store floor and ceilings triangles
        for (HousePlane& plane : World::GetHousePlanes()) {
            for (uint32_t i = 0; i < plane.GetIndices().size(); i+=3) {
                Triangle& triangle = g_triangles.emplace_back();

                int idx0 = plane.GetIndices()[i + 0];
                int idx1 = plane.GetIndices()[i + 1];
                int idx2 = plane.GetIndices()[i + 2];

                triangle.v0 = plane.GetVertices()[idx0].position;
                triangle.v1 = plane.GetVertices()[idx1].position;
                triangle.v2 = plane.GetVertices()[idx2].position;

                triangle.normal = plane.GetVertices()[i].normal;
            }
        }

        // Store wall triangles
        for (Wall& wall : World::GetWalls()) {
            for (WallSegment& wallSegment : wall.GetWallSegments()) {
                for (uint32_t i = 0; i < wallSegment.GetIndices().size(); i += 3) {
                    Triangle& triangle = g_triangles.emplace_back();
                    
                    int idx0 = wallSegment.GetIndices()[i + 0];
                    int idx1 = wallSegment.GetIndices()[i + 1];
                    int idx2 = wallSegment.GetIndices()[i + 2];
                    
                    triangle.v0 = wallSegment.GetVertices()[idx0].position;
                    triangle.v1 = wallSegment.GetVertices()[idx1].position;
                    triangle.v2 = wallSegment.GetVertices()[idx2].position;
                    
                    triangle.normal = normalize(
                        wallSegment.GetVertices()[idx0].normal +
                        wallSegment.GetVertices()[idx1].normal +
                        wallSegment.GetVertices()[idx2].normal
                    );
                }
            }
        }

        g_pointCloud.clear();

        for (Triangle& triangle : g_triangles) {

            // Make sure normal is valid
            glm::vec3 edge1 = triangle.v1 - triangle.v0;
            glm::vec3 edge2 = triangle.v2 - triangle.v0;
            triangle.normal = glm::cross(edge1, edge2);
            triangle.normal = glm::normalize(triangle.normal);

            // Calculate the normal of the triangle
            const glm::vec3& normal = triangle.normal;

            // Choose the up vector based on the normal
            glm::vec3 up = (std::abs(normal.z) > 0.999f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);

            // Calculate right and up vectors
            glm::vec3 right = glm::normalize(glm::cross(up, normal));
            up = glm::cross(normal, right);  // No need to normalize again as the cross product of two unit vectors is already normalized

            glm::mat3 transform(right.x, right.y, right.z,up.x, up.y, up.z,normal.x, normal.y, normal.z);

            glm::vec2 v0_2d(glm::dot(right, triangle.v0), glm::dot(up, triangle.v0));
            glm::vec2 v1_2d(glm::dot(right, triangle.v1), glm::dot(up, triangle.v1));
            glm::vec2 v2_2d(glm::dot(right, triangle.v2), glm::dot(up, triangle.v2));

            // Determine the bounding box of the 2D triangle
            glm::vec2 min = glm::min(glm::min(v0_2d, v1_2d), v2_2d);
            glm::vec2 max = glm::max(glm::max(v0_2d, v1_2d), v2_2d);

            // Round min and max values
            min.x = RoundDown(min.x, POINT_CLOUD_SPACING) - POINT_CLOUD_SPACING * 0.5f;
            min.y = RoundDown(min.y, POINT_CLOUD_SPACING) - POINT_CLOUD_SPACING * 0.5f;
            max.x = RoundUp(max.x, POINT_CLOUD_SPACING) + POINT_CLOUD_SPACING * 0.5f;
            max.y = RoundUp(max.y, POINT_CLOUD_SPACING) + POINT_CLOUD_SPACING * 0.5f;

            float theshold = 0.05f;
            min.x += theshold;
            min.y += theshold;
            max.x -= theshold;
            max.y -= theshold;

            // Generate points within the bounding box
            for (float x = min.x; x <= max.x; x += POINT_CLOUD_SPACING) {
                for (float y = min.y; y <= max.y; y += POINT_CLOUD_SPACING) {
                    glm::vec2 pt(x, y);
                    if (Util::IsPointInTriangle2D(pt, v0_2d, v1_2d, v2_2d)) {
                        glm::vec3 pt3d = triangle.v0 + right * (pt.x - v0_2d.x) + up * (pt.y - v0_2d.y);

                        CloudPoint& cloudPoint = g_pointCloud.emplace_back();
                        cloudPoint.position = glm::vec4(pt3d, 0.0f);
                        cloudPoint.normal = glm::vec4(triangle.normal, 0.0f);
                    }
                }
            }
        }

        g_pointCloudNeedsGpuUpdate = true;

        std::cout << "Recreated point cloud: " << g_pointCloud.size() << " points \n";
    }

    void CreateHouseBvh() {     
        // Destroy any previous house bvh
        if (g_houseBvhId != 0) {
            Bvh::Gpu::DestroyMeshBvh(g_houseBvhId);
        }

        g_houseMinBounds = glm::vec3(std::numeric_limits<float>::max());
        g_houseMaxBounds = glm::vec3(-std::numeric_limits<float>::max());

        // Create house vertices
        std::vector<Vertex> vertices;
        for (Triangle& triangle : g_triangles) {
            Vertex v0, v1, v2;
            v0.position = triangle.v0;
            v1.position = triangle.v1;
            v2.position = triangle.v2;
            v0.normal = triangle.normal;
            v1.normal = triangle.normal;
            v2.normal = triangle.normal;
            vertices.push_back(triangle.v0);
            vertices.push_back(triangle.v1);
            vertices.push_back(triangle.v2);

            g_houseMinBounds.x = std::min(g_houseMinBounds.x, v0.position.x);
            g_houseMinBounds.y = std::min(g_houseMinBounds.y, v0.position.y);
            g_houseMinBounds.z = std::min(g_houseMinBounds.z, v0.position.z);
            g_houseMaxBounds.x = std::max(g_houseMaxBounds.x, v0.position.x);
            g_houseMaxBounds.y = std::max(g_houseMaxBounds.y, v0.position.y);
            g_houseMaxBounds.z = std::max(g_houseMaxBounds.z, v0.position.z);

            g_houseMinBounds.x = std::min(g_houseMinBounds.x, v1.position.x);
            g_houseMinBounds.y = std::min(g_houseMinBounds.y, v1.position.y);
            g_houseMinBounds.z = std::min(g_houseMinBounds.z, v1.position.z);
            g_houseMaxBounds.x = std::max(g_houseMaxBounds.x, v1.position.x);
            g_houseMaxBounds.y = std::max(g_houseMaxBounds.y, v1.position.y);
            g_houseMaxBounds.z = std::max(g_houseMaxBounds.z, v1.position.z);

            g_houseMinBounds.x = std::min(g_houseMinBounds.x, v2.position.x);
            g_houseMinBounds.y = std::min(g_houseMinBounds.y, v2.position.y);
            g_houseMinBounds.z = std::min(g_houseMinBounds.z, v2.position.z);
            g_houseMaxBounds.x = std::max(g_houseMaxBounds.x, v2.position.x);
            g_houseMaxBounds.y = std::max(g_houseMaxBounds.y, v2.position.y);
            g_houseMaxBounds.z = std::max(g_houseMaxBounds.z, v2.position.z);
        }

        // Create house indices
        std::vector<uint32_t> indices(vertices.size());
        for (int i = 0; i < vertices.size(); i++) {
            indices[i] = i;
        }

        g_houseBvhId = Bvh::Gpu::CreateMeshBvhFromVertexData(vertices, indices);

        // For now you only have one light volume, for whatever house you just made.
        for (LightVolume& lightVolume : g_lightVolumes) {
            lightVolume.CleanUp();
        }
        g_lightVolumes.clear();
        LightVolume& lightVolume = g_lightVolumes.emplace_back();
        lightVolume.Init(vertices, g_houseMinBounds, g_houseMaxBounds);

        InitPointGrid();
    }

    void CreateDoorBvh() {
        Mesh* cubeMesh = AssetManager::GetCubeMesh();
        std::vector<Vertex> vertices = AssetManager::GetMeshVertices(cubeMesh);

        for (Vertex& vertex : vertices) {
            vertex.position *= glm::vec3(DOOR_DEPTH, DOOR_HEIGHT, DOOR_WIDTH);
            vertex.position.x += -0.005f;
            vertex.position.z -= DOOR_WIDTH / 2;
            vertex.position.y += DOOR_HEIGHT / 2;
            vertex.position.x -= DOOR_DEPTH / 2;
        }

        std::vector<uint32_t> indices(vertices.size());
        for (int i = 0; i < vertices.size(); i++) {
            indices[i] = i;
        }

        g_doorBvhId = Bvh::Gpu::CreateMeshBvhFromVertexData(vertices, indices);
    }
    
    void UpdateSceneBVh() {
        std::vector<PrimitiveInstance> instances;

        // Add the house
        PrimitiveInstance& instance = instances.emplace_back();
        instance.worldAabbBoundsMin.x = g_houseMinBounds.x;
        instance.worldAabbBoundsMin.y = g_houseMinBounds.y;
        instance.worldAabbBoundsMin.z = g_houseMinBounds.z;
        instance.worldAabbBoundsMax.x = g_houseMaxBounds.x;
        instance.worldAabbBoundsMax.y = g_houseMaxBounds.y;
        instance.worldAabbBoundsMax.z = g_houseMaxBounds.z;
        instance.objectId = 0;
        instance.worldTransform = glm::mat4(1.0f);
        instance.meshBvhId = g_houseBvhId;
        instance.worldAabbCenter = (instance.worldAabbBoundsMin + instance.worldAabbBoundsMax) * 0.5f;

        // Add all the doors
        int objectId = 1;
        for (Door& door : World::GetDoors()) {
            uint64_t rigidStaticId = door.GetPhysicsId();
            RigidStatic* rigidStatic = Physics::GetRigidStaitcById(rigidStaticId);
            PxRigidStatic* pxRigidStatic = rigidStatic->GetPxRigidStatic();
            PxRigidActor* pxRigidActor = static_cast<PxRigidActor*>(pxRigidStatic);
            PxBounds3 bounds = pxRigidActor->getWorldBounds();
            PxVec3 minBounds = bounds.minimum;
            PxVec3 maxBounds = bounds.maximum;

            PrimitiveInstance& instance = instances.emplace_back();
            instance.worldAabbBoundsMin.x = minBounds.x;
            instance.worldAabbBoundsMin.y = minBounds.y;
            instance.worldAabbBoundsMin.z = minBounds.z;
            instance.worldAabbBoundsMax.x = maxBounds.x;
            instance.worldAabbBoundsMax.y = maxBounds.y;
            instance.worldAabbBoundsMax.z = maxBounds.z;
            instance.objectId = objectId;// door.GetObjectId();
            instance.worldTransform = door.GetDoorModelMatrix();
            instance.meshBvhId = g_doorBvhId;
            instance.worldAabbCenter = (instance.worldAabbBoundsMin + instance.worldAabbBoundsMax) * 0.5f;
            objectId++;
        }

        Bvh::Gpu::UpdateSceneBvh(g_sceneBvhId, instances);
    }

    void InitPointGrid() {

        for (LightVolume& lightVolume : g_lightVolumes) {

            g_pointGridWorldMin = lightVolume.m_offset;
            g_pointGridWorldMax = lightVolume.m_offset + glm::vec3(lightVolume.m_worldSpaceWidth, lightVolume.m_worldSpaceHeight, lightVolume.m_worldSpaceDepth);

            glm::vec3 worldSize = g_pointGridWorldMax - g_pointGridWorldMin;
            g_pointCloudGridDimensions.x = static_cast<unsigned int>(glm::ceil(worldSize.x / g_pointCloudOctrantSpacing));
            g_pointCloudGridDimensions.y = static_cast<unsigned int>(glm::ceil(worldSize.y / g_pointCloudOctrantSpacing));
            g_pointCloudGridDimensions.z = static_cast<unsigned int>(glm::ceil(worldSize.z / g_pointCloudOctrantSpacing));

            g_pointCloudOctrantSize = worldSize / glm::vec3(g_pointCloudGridDimensions);

             // Bail if point cloud is empty
            if (g_pointCloud.empty()) return;

            // For each grid cell, count how many points fall inside it
            unsigned int totalCells = g_pointCloudGridDimensions.x * g_pointCloudGridDimensions.y * g_pointCloudGridDimensions.z;
            std::vector<unsigned int> cellCounts(totalCells, 0);

            for (const auto& point : g_pointCloud) {
                // Find out which grid cell this point belongs to
                glm::vec3 relativePos = glm::vec3(point.position) - g_pointGridWorldMin;
                glm::ivec3 cellCoords = glm::ivec3(relativePos / g_pointCloudOctrantSize);

                // Make sure the coordinates are within the grid bounds, just in case
                cellCoords = glm::clamp(cellCoords, glm::ivec3(0), glm::ivec3(g_pointCloudGridDimensions) - 1);

                // Convert the 3D cell coordinate into a 1D array index and increment the counter
                unsigned int cellIndex = (cellCoords.z * g_pointCloudGridDimensions.x * g_pointCloudGridDimensions.y) + (cellCoords.y * g_pointCloudGridDimensions.x) + cellCoords.x;
                cellCounts[cellIndex]++;
            }

            // Create the final PointGridCell structures with the correct offsets
            g_pointCloudOctrants.resize(totalCells);
            unsigned int currentOffset = 0;
            for (unsigned int i = 0; i < totalCells; ++i) {
                g_pointCloudOctrants[i].m_cloudPointCount = cellCounts[i];
                g_pointCloudOctrants[i].m_offset = currentOffset;
                currentOffset += cellCounts[i]; // The next cell's offset starts after all of this cell's points
            }

            // Finally, we create the master list of sorted point indices
            g_pointIndices.resize(g_pointCloud.size());
            std::vector<unsigned int> tempOffsets(totalCells);
            for (unsigned int i = 0; i < totalCells; ++i) {
                tempOffsets[i] = g_pointCloudOctrants[i].m_offset;
            }

            // Go through the original points again...
            for (unsigned int i = 0; i < g_pointCloud.size(); ++i) {
                const auto& point = g_pointCloud[i];

                // Find which cell it belongs to...
                glm::vec3 relativePos = glm::vec3(point.position) - g_pointGridWorldMin;
                glm::ivec3 cellCoords = glm::ivec3(relativePos / g_pointCloudOctrantSize);
                cellCoords = glm::clamp(cellCoords, glm::ivec3(0), glm::ivec3(g_pointCloudGridDimensions) - 1);
                unsigned int cellIndex = (cellCoords.z * g_pointCloudGridDimensions.x * g_pointCloudGridDimensions.y) + (cellCoords.y * g_pointCloudGridDimensions.x) + cellCoords.x;

                // Use the write counter to place the point's original index i in the correct slot
                unsigned int& insertionIndex = tempOffsets[cellIndex];
                g_pointIndices[insertionIndex] = i;

                // Increment the write counter for that cell
                insertionIndex++;
            }
        }
        std::cout << "Point cloud octrant grid created\n";
        std::cout << "g_pointIndices:       " << g_pointIndices.size() << "\n";
        std::cout << "g_pointCloudOctrants: " << g_pointCloudOctrants.size() << "\n";
        
    }

    uint64_t GetSceneBvhId() {
        return g_sceneBvhId;
    }

    const std::vector<BvhNode>& GetSceneNodes() {
        static std::vector<BvhNode> empty;
        if (g_sceneBvhId == 0) {
            return empty;
        }

        SceneBvh* sceneBvh = Bvh::Gpu::GetSceneBvhById(g_sceneBvhId);
        if (!sceneBvh) return empty;

        return sceneBvh->m_nodes;
    }

    std::vector<CloudPoint>& GetPointClound() {
        return g_pointCloud;
    }

    std::vector<LightVolume>& GetLightVolumes() {
        return g_lightVolumes;
    }

    void SetGlobalIlluminationStructuresDirtyState(bool state) {
        g_globalIlluminationStructuresDirty = state;
    }

    bool GlobalIlluminationStructuresAreDirty() {
        return g_globalIlluminationStructuresDirty;
    }

    void SetPointCloudNeedsGpuUpdateState(bool state) {
        g_pointCloudNeedsGpuUpdate = state;
    }

    bool PointCloudNeedsGpuUpdate() {
        return g_pointCloudNeedsGpuUpdate;
    }

    float GetProbeSpacing() {
        return g_probeSpacing;
    }

    std::vector<PointCloudOctrant>& GetPointCloudOctrants() {
        return g_pointCloudOctrants;
    }

    std::vector<unsigned int>& GetPointIndices() {
        return g_pointIndices;
    }

    glm::uvec3 GetPointCloudGridDimensions() {
        return g_pointCloudGridDimensions;
    }

    glm::vec3 GetPointGridWorldMin() {
        return g_pointGridWorldMin;
    }

    glm::vec3 GetPointGridWorldMax() {
        return g_pointGridWorldMax;
    }
}

void LightVolume::Init(const std::vector<Vertex>& vertices, const glm::vec3& aabbMin, const glm::vec3& aabbMax) {
    glm::vec3 inflatedAabbMin = aabbMin - glm::vec3(1.0f);
    glm::vec3 inflatedAabbMax = aabbMax + glm::vec3(2.0f);
    m_offset = inflatedAabbMin;
    m_worldSpaceWidth = inflatedAabbMax.x - inflatedAabbMin.x;
    m_worldSpaceHeight = inflatedAabbMax.y - inflatedAabbMin.y;
    m_worldSpaceDepth = inflatedAabbMax.z - inflatedAabbMin.z;
    m_textureWidth = int(m_worldSpaceWidth / GlobalIllumination::GetProbeSpacing());
    m_textureHeight = int(m_worldSpaceHeight / GlobalIllumination::GetProbeSpacing());
    m_textureDepth = int(m_worldSpaceDepth / GlobalIllumination::GetProbeSpacing());

    // Create the 3d textures
    glGenTextures(1, &m_lightVolumeA);
    glBindTexture(GL_TEXTURE_3D, m_lightVolumeA);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, m_textureWidth, m_textureHeight, m_textureDepth, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_lightVolumeB);
    glBindTexture(GL_TEXTURE_3D, m_lightVolumeB);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, m_textureWidth, m_textureHeight, m_textureDepth, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_lightVolumeTextures[0] = m_lightVolumeA;
    m_lightVolumeTextures[1] = m_lightVolumeB;

    glGenTextures(1, &m_lightVolumeMaskTexture);
    glBindTexture(GL_TEXTURE_3D, m_lightVolumeMaskTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, m_textureWidth, m_textureHeight, m_textureDepth, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void LightVolume::CleanUp() {
    glDeleteTextures(1, &m_lightVolumeA);
    glDeleteTextures(1, &m_lightVolumeB);
    glDeleteTextures(1, &m_lightVolumeMaskTexture);
}

GLuint LightVolume::GetLightingTextureHandle() {
    return m_lightVolumeTextures[m_pingPongReadIndex];
}

GLuint LightVolume::GetMaskTextureHandle() {
    return m_lightVolumeMaskTexture;
}
