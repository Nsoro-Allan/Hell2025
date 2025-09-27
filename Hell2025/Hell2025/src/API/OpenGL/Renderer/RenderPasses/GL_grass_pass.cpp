#include "API/OpenGL/Renderer/GL_renderer.h" 

#include "HellLogging.h"

#include "AssetManagement/AssetManager.h"
#include "Types/Renderer/GrassMesh.h"

#include "API/OpenGL/GL_backend.h"
#include "Config/Config.h"
#include "Editor/Editor.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Core/Game.h"
#include "Util/Util.h"
#include "Ocean/Ocean.h"

#include "World/World.h"

struct GrassVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct OpenGLGrassMesh {

private:
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;

public:

    int GetVAO() {
        return VAO;
    }

    int GetVBO() {
        return VBO;
    }

    int GetEBO() {
        return EBO;
    }

    void AllocateBuffers(size_t vertexCount, size_t indexCount) {
        if (vertexCount == 0 || indexCount == 0) {
            if (VAO != 0) {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
                VAO = VBO = EBO = 0;
            }
            return;
        }

        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GrassVertex), nullptr, GL_DYNAMIC_DRAW); // Allocate, no data

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW); // Allocate, no data

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)offsetof(GrassVertex, normal));

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

namespace OpenGLRenderer {

    OpenGLGrassMesh g_grassGeometryMesh;
    GLuint g_indirectBuffer = 0;

#define BLADE_SPACING 0.0185185185185185f
#define BLADES_PER_TILE_AXIS 432

    void GenerateBladePositions(float xOffset, float zOffset, int viewportIndex);
    void RenderGrass(int viewportIndex);

    void InitGrass() {
        int bladesPerHeightMapAxis = HEIGHT_MAP_SIZE * HEIGHTMAP_SCALE_XZ / BLADE_SPACING;
        int bufferSize = bladesPerHeightMapAxis * bladesPerHeightMapAxis * sizeof(glm::vec4);
        //std::cout << "Grass SSBO allocated: " << Util::BytesToMBString(bufferSize) << "\n";
        CreateSSBO("BladePositions", bufferSize, GL_DYNAMIC_STORAGE_BIT);
        CreateGrassGeometry();
            
        // Create indirect buffer
        if (g_indirectBuffer == 0) {
            glGenBuffers(1, &g_indirectBuffer);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
            glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndexedIndirectCommand), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }
        Logging::Init() << "Initialized grass geometry";
    }

    void CreateGrassGeometry() {        
        int bladeCount = 360;

        // Allocate mesh memory
        if (g_grassGeometryMesh.GetVAO() == 0) {
            int maxVertexCount = bladeCount * 6 * 2;
            int maxIndexCount = bladeCount * 12 * 2;
            g_grassGeometryMesh.AllocateBuffers(maxVertexCount, maxIndexCount);
        }

        OpenGLShader* geometryShader = GetShader("GrassGeometryGeneration");
        geometryShader->Bind();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_grassGeometryMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_grassGeometryMesh.GetEBO());
        glDispatchCompute(bladeCount, 1, 1);
    }

    void GrassPass() {
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();
        if (!rendererSettings.drawGrass) return;

        //return;
        //if (Input::KeyPressed(HELL_KEY_X)) {
        //    CreateGrassGeometry();
        //}

        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        OpenGLFrameBuffer* roadFramebuffer = GetFrameBuffer("Road");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* wipBuffer = GetFrameBuffer("WIP");
        OpenGLSSBO* bladeositionsSSBO = GetSSBO("BladePositions");

        BlitFrameBuffer(gBuffer, wipBuffer, "WorldPosition", "WorldPosition", GL_COLOR_BUFFER_BIT, GL_NEAREST);
       
        // Bindings
        glBindVertexArray(g_grassGeometryMesh.GetVAO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, g_indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, bladeositionsSSBO->GetHandle());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, g_grassGeometryMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, g_grassGeometryMesh.GetEBO());
        glBindTextureUnit(0, worldFramebuffer->GetColorAttachmentHandleByName("HeightMap"));
        glBindTextureUnit(1, wipBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        glBindTextureUnit(2, AssetManager::GetTextureByName("Perlin")->GetGLTexture().GetHandle());
        glBindTextureUnit(3, roadFramebuffer->GetColorAttachmentHandleByName("RoadMask"));

        // GL State
        SetRasterizerState("GeometryPass_NonBlended");

        // Generate and draw
        for (int i = 0; i < 4; i++) {   // CHANGE TO VIEWPORT NOT PLAYER!!!!

            int viewportIndex = i;
                
            Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
            if (!viewport->IsVisible()) continue;

            Frustum& frustum = viewport->GetFrustum();

            const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
            glm::vec3 viewPos = viewportData[viewportIndex].inverseView[3];

            int maxChunkDrawDistance = 3; 

            ivecXZ cameraChunk(static_cast<int>(std::floor(viewPos.x / HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE)),
                                static_cast<int>(std::floor(viewPos.z / HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE)));

            int32_t xBegin = cameraChunk.x - maxChunkDrawDistance;
            int32_t xEnd = cameraChunk.x + maxChunkDrawDistance;
            int32_t zBegin = cameraChunk.z - maxChunkDrawDistance;
            int32_t zEnd = cameraChunk.z + maxChunkDrawDistance;

            if (Editor::IsOpen()) {
                xBegin = 0;
                zBegin = 0;
                xEnd = World::GetChunkCountX();
                zEnd = World::GetChunkCountZ();
            }

            xBegin = std::max(xBegin, 0);
            zBegin = std::max(zBegin, 0);
            xEnd = std::min(xEnd, (int32_t)World::GetChunkCountX());
            zEnd = std::min(zEnd, (int32_t)World::GetChunkCountZ());

            std::vector<vecXZ> chunkOffsets;
            AABB chunkAABB;
            glm::vec3 chunkBoundsMin;
            glm::vec3 chunkBoundsMax;

            /*
            for (int x = xBegin; x < xEnd; x++) {
                for (int z = zBegin; z < zEnd; z++) {

                    // Skip chunks that don't exist
                    const HeightMapChunk* chunk = World::GetChunk(x, z);
                    if (!chunk) continue;

                    chunkAABB = AABB(chunk->aabbMin, chunk->aabbMax);

                    // Check if within threshold to camera
                    //float threshold = 40.0f;
                    //glm::vec3 viewPosNormalized = viewPos * glm::vec3(1.0f, 0.0f, 1.0f);
                    //glm::vec3 aabbCenterNormalized = chunkAABB.GetCenter() * glm::vec3(1.0f, 0.0f, 1.0f);

                    //float distance = Util::ManhattanDistance(viewPosNormalized, aabbCenterNormalized);
                    //if (distance >= threshold) {
                    //    continue;
                    //}
                
                    if (frustum.IntersectsAABBFast(chunkAABB)) {
                        float xOffset = x * CHUNK_SIZE_WORLDSPACE;
                        float zOffset = z * CHUNK_SIZE_WORLDSPACE;
                        chunkOffsets.emplace_back(vecXZ(xOffset, zOffset));
                        //DrawAABB(chunkAABB, GREEN);
                    }
                }
            }
            */

            for (HeightMapChunk& chunk : World::GetHeightMapChunks()) {
                chunkAABB = AABB(chunk.aabbMin, chunk.aabbMax);

                // Check if within threshold to camera
                float threshold = 30.0f;
                glm::vec3 viewPosNormalized = viewPos * glm::vec3(1.0f, 0.0f, 1.0f);
                glm::vec3 aabbCenterNormalized = chunkAABB.GetCenter() * glm::vec3(1.0f, 0.0f, 1.0f);

                float distance = Util::ManhattanDistance(viewPosNormalized, aabbCenterNormalized);
                if (distance >= threshold) {
                    //std::cout << "skipping " << chunk.coord.x << ", " << chunk.coord.z << "\n";
                    continue;
                }

                if (frustum.IntersectsAABBFast(chunkAABB)) {
                    float xOffset = chunk.coord.x * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
                    float zOffset = chunk.coord.z * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
                    chunkOffsets.emplace_back(vecXZ(xOffset, zOffset));
                    //DrawAABB(chunkAABB, GREEN);
                }
            }

            // Zero out indirect buffer
            DrawIndexedIndirectCommand initialCmd;
            initialCmd.instanceCount = 1;
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
            glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawIndexedIndirectCommand), &initialCmd);
                                
            // Generate grass positions
            for (vecXZ& chunkOffset: chunkOffsets) {
                GenerateBladePositions(chunkOffset.x, chunkOffset.z, viewportIndex);
            }
            // Then render for the current viewport
            RenderGrass(viewportIndex);
        }
    }

    void GenerateBladePositions(float xOffset, float zOffset, int viewportIndex) {
        int bladeCount = BLADES_PER_TILE_AXIS * BLADES_PER_TILE_AXIS;
        int maxVertexCount = bladeCount * 6;
        int maxIndexCount = bladeCount * 12;

        // Uniforms
        OpenGLShader* generationShader = GetShader("GrassPositionGeneration");
        generationShader->Bind();
        generationShader->SetInt("gridSize", BLADES_PER_TILE_AXIS);
        generationShader->SetInt("u_viewportIndex", viewportIndex);
        generationShader->SetFloat("spacing", BLADE_SPACING);
        generationShader->SetVec3("offset", glm::vec3(xOffset, 0.0f, zOffset));
        generationShader->SetFloat("u_heightMapWorldSpaceSize", HEIGHT_MAP_SIZE * HEIGHTMAP_SCALE_XZ);
        generationShader->SetFloat("u_waterHeight", Ocean::GetOceanOriginY());
        
        // Dispatch compute shader
        int workGroupsX = BLADES_PER_TILE_AXIS / 16;
        int workGroupsY = BLADES_PER_TILE_AXIS / 16;
        glDispatchCompute(workGroupsX, workGroupsY, 1);
    }

    void RenderGrass(int viewportIndex) {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLSSBO* bladeositionsSSBO = GetSSBO("BladePositions");
        OpenGLShader* geometryShader = GetShader("Grass");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        if (!viewport->IsVisible()) return;

        OpenGLRenderer::SetViewport(gBuffer, viewport);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
           
        glm::mat4 projectionView = viewportData[viewportIndex].projectionView;

        geometryShader->Bind();
        geometryShader->SetMat4("projectionView", projectionView);


        glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer); 
        glDrawArraysIndirect(GL_TRIANGLES, 0);


        //float bladeCount = 360;            
        //glm::mat4 projectionMatrix = viewportData[i].projection;
        //glm::mat4 viewMatrix = viewportData[i].view;            
        //Transform transform;
        //transform.position = glm::vec3(17, -4.1, 19);
        //transform.scale = glm::vec3(5);            
        //OpenGLShader* shader = GetShader("SolidColor");
        //shader->Use();
        //shader->SetMat4("projection", projectionMatrix);
        //shader->SetMat4("view", viewMatrix);
        //shader->SetMat4("model", transform.to_mat4());
        //shader->SetBool("useUniformColor", false);            
        //glBindVertexArray(g_grassGeometryMesh.GetVAO());
        //glEnable(GL_CULL_FACE);
        //glDrawElements(GL_TRIANGLES, bladeCount * 24, GL_UNSIGNED_INT, 0);
      
    }
}