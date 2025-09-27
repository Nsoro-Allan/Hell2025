#include "HellLogging.h"
#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/Types/GL_heightmap_mesh.h"
#include "API/OpenGL/Types/GL_texture_readback.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Editor/Gizmo.h"
#include "Imgui/ImguiBackEnd.h"
#include "Input/Input.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"
#include "Viewport/ViewportManager.h"
#include "World/SectorManager.h"
#include "World/World.h"

#include "Pathfinding/AStarMap.h"
#include "lodepng/lodepng.h"

#include "Audio/Audio.h"

#include "Physics/Physics.h"

#include "Managers/MapManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void BlitWorldMap();
    void GenerateHeightMapVertexData();
    void GeneratePhysXTextures();
    void DrawHeightMap();

    void RecalculateAllHeightMapData(bool blitWorldMap) {
        if (blitWorldMap) {
            BlitWorldMap();
        }
        GenerateHeightMapVertexData();
        GeneratePhysXTextures();
        AStarMap::Init();
        AStarMap::UpdateDebugMeshesFromHeightField();
    }

    void HeightMapPass() {

        //if (Input::KeyPressed(HELL_KEY_SPACE))
        //BlitHeightMapWorld();

        DrawHeightMap();

        if (Editor::IsOpen() && Editor::GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) {

            //if (Input::KeyPressed(HELL_KEY_L)) {                
            //    HeightMapData heightMapData = File::LoadHeightMap("TEST.heightmap");
            //    OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
            //    GLuint textureHandle = heightmapFBO->GetColorAttachmentHandleByName("Color");
            //}
            //if (Input::KeyPressed(HELL_KEY_S)) {
            //    SaveHeightMap();
            //}
        }

        if (Input::KeyPressed(HELL_KEY_U)) {
            if (Util::RenameFile("res/shit.txt", "res/fuck.txt")) {
                std::cout << "rename successful\n";
            }
        }
    }

    void BlitWorldMap() {
        Map* map = MapManager::GetMapByName("Shit");
        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        OpenGLFrameBuffer* roadFramebuffer = GetFrameBuffer("Road"); 
        OpenGLShader* shader = GetShader("HeightMapToWorldBlit");

        if (!map) return;
        if (!shader) return;
        if (!worldFramebuffer) return;


        int textureWidth = (World::GetChunkCountX() * HEIGHT_MAP_CHUNK_PIXEL_SIZE) + 1;
        int textureHeight = (World::GetChunkCountZ() * HEIGHT_MAP_CHUNK_PIXEL_SIZE) + 1;

        const glm::ivec2 textureSize = glm::ivec2(textureWidth, textureHeight);

        // Resize world framebuffer if it is too small for the heightmap
        if (worldFramebuffer->GetWidth() != textureSize.x || worldFramebuffer->GetHeight() != textureSize.y) {
            worldFramebuffer->Resize(textureSize.x, textureSize.y);

            int roadScale = 4;
            roadFramebuffer->Resize(textureSize.x * roadScale, textureSize.y * roadScale);
        }

        // Blit height maps
        shader->Bind();
        for (MapInstance& mapInstance : World::GetMapInstances()) {
            int offsetX = mapInstance.spawnOffsetChunkX * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
            int offsetZ = mapInstance.spawnOffsetChunkZ * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
            int heightMapTextureWidth = mapInstance.GetChunkCountX() * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
            int heightMapTextureHeight = mapInstance.GetChunkCountZ() * HEIGHT_MAP_CHUNK_PIXEL_SIZE;
            shader->SetInt("u_offsetX", offsetX);
            shader->SetInt("u_offsetZ", offsetZ);
            glBindImageTexture(0, worldFramebuffer->GetColorAttachmentHandleByName("HeightMap"), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);
            glBindImageTexture(1, map->GetHeightMapGLTexture().GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);
            glDispatchCompute(heightMapTextureWidth / 8, heightMapTextureHeight / 8, 1);
        }

        // Blit roads
    }

    void PaintHeightMap() {
        if (!IsMouseRayWorldPositionReadBackReady()) return;
        if (!Editor::IsOpen()) return;
        if (Editor::GetEditorMode() != EditorMode::MAP_HEIGHT_EDITOR) return;
        if (ImGuiBackEnd::OwnsMouse()) return;

        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("HeightMapPaint");

        if (Input::LeftMouseDown() || Input::RightMouseDown()) {
            shader->Bind();
            shader->SetInt("u_paintX", static_cast<int>(GetMouseRayWorldPostion().x / (float)worldFramebuffer->GetWidth()));
            shader->SetInt("u_paintY", static_cast<int>(GetMouseRayWorldPostion().z / (float)worldFramebuffer->GetHeight()));
            shader->SetFloat("u_brushSize", Editor::GetMapHeightBrushSize());
            shader->SetFloat("u_brushStrength", Editor::GetMapHeightBrushStrength() * (Input::RightMouseDown() ? -1.0f : 1.0f));
            shader->SetFloat("u_noiseStrength", Editor::GetMapHeightNoiseStrength());
            shader->SetFloat("u_noiseScale", Editor::GetMapHeightNoiseScale());
            shader->SetFloat("u_minPaintHeight", Editor::GetMapHeightMinPaintHeight());
            shader->SetFloat("u_maxPaintHeight", Editor::GetMapHeightMaxPaintHeight());

            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            glBindImageTexture(0, worldFramebuffer->GetColorAttachmentHandleByName("HeightMap"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);
            glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
            glDispatchCompute(worldFramebuffer->GetWidth() / 32, worldFramebuffer->GetHeight() / 32, 1);

            GenerateHeightMapVertexData();
        }
    }

    void GenerateHeightMapVertexData() {
        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapVertexGeneration");

        int heightMapWidth = 256;
        int heightMapDepth = 512;

        std::vector<HeightMapChunk>& chunks = World::GetHeightMapChunks();

        heightMapMesh.AllocateMemory(chunks.size());
        
        shader->Bind();
        glBindImageTexture(0, worldFramebuffer->GetColorAttachmentHandleByName("HeightMap"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, heightMapMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, heightMapMesh.GetEBO());

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        for (HeightMapChunk& chunk : chunks) {
            shader->SetInt("u_baseIndex", chunk.baseIndex);
            shader->SetInt("u_baseVertex", chunk.baseVertex);
            shader->SetInt("u_chunkX", chunk.coord.x);
            shader->SetInt("u_chunkZ", chunk.coord.z);
            int chunkSize = HEIGHT_MAP_SIZE / 8;
            int chunkWidth = chunkSize + 1;
            int chunkDepth = chunkSize + 1;
            int groupSizeX = (chunkWidth + 16 - 1) / 16;
            int groupSizeY = (chunkDepth + 16 - 1) / 16;
            glDispatchCompute(groupSizeX, groupSizeY, 1);
        }

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }

    void GeneratePhysXTextures() {
        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");

        GLuint handle = worldFramebuffer->GetColorAttachmentHandleByName("HeightMap");
        GLint level = 0;
        GLint zOffset = 0;
        GLsizei width = 33;
        GLsizei height = 33;
        GLsizei depth = 1;
        GLenum format = GL_RED;
        GLenum type = GL_FLOAT;
        GLsizei numPixels = width * height * depth;
        GLsizei dataSize = numPixels * sizeof(float);
        std::vector<float> pixels(numPixels);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        struct ChunkReadBackData {
            float vertices[VERTICES_PER_CHUNK];
        };

        int chunkCount = World::GetChunkCount();
        std::vector<ChunkReadBackData> chunkReadBackDataSet(chunkCount);

        // Readback height chunk data from gpu
        std::vector<HeightMapChunk>& chunks = World::GetHeightMapChunks();
        for (int i = 0; i < chunkCount; i++) {
            HeightMapChunk& chunk = chunks[i];
            GLint xOffset = chunk.coord.x * 32;
            GLint yOffset = chunk.coord.z * 32;

            if (xOffset + width > worldFramebuffer->GetWidth() ||
                yOffset + height > worldFramebuffer->GetHeight()) {
                std::cout << "YOU HAVE PROBLEMS: \n";
                std::cout << " - worldFramebuffer->GetWidth(): " << worldFramebuffer->GetWidth() << "\n";
                std::cout << " - worldFramebuffer->GetHeight(): " << worldFramebuffer->GetHeight() << "\n";
                std::cout << " - xOffset: " << xOffset << "\n";
                std::cout << " - yOffset: " << yOffset << "\n";
                std::cout << " - width: " << width << "\n";
                std::cout << " - height: " << height << "\n";
                std::cout << " - chunkCount: " << chunkCount << "\n";
            }

            glGetTextureSubImage(handle, level, xOffset, yOffset, zOffset, width, height, depth, GL_RED, GL_FLOAT, dataSize, chunkReadBackDataSet[i].vertices);
        }

        Physics::MarkAllHeightFieldsForRemoval();

        // For each chunk determine the AABB
        for (int i = 0; i < chunkCount; i++) {
            HeightMapChunk& chunk = chunks[i];
            glm::vec3 aabbMin(std::numeric_limits<float>::max());
            glm::vec3 aabbMax(std::numeric_limits<float>::lowest());

            for (size_t j = 0; j < VERTICES_PER_CHUNK; j++) {
                float x = ((j % 33) + (chunk.coord.x * 32)) * HEIGHTMAP_SCALE_XZ;
                float y = chunkReadBackDataSet[i].vertices[j] * HEIGHTMAP_SCALE_Y;
                float z = ((j / 33) + (chunk.coord.z * 32)) * HEIGHTMAP_SCALE_XZ;

                glm::vec3 position(x, y, z);
                aabbMin = glm::min(aabbMin, position);
                aabbMax = glm::max(aabbMax, position);
            }
            chunk.aabbMin = aabbMin;
            chunk.aabbMax = aabbMax;

            vecXZ worldSpaceOffest = vecXZ(chunk.coord.x * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE, chunk.coord.z * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE);
            Physics::CreateHeightField(worldSpaceOffest, chunkReadBackDataSet[i].vertices);
       }
    }

    void DrawHeightMap() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* roadFramebuffer = GetFrameBuffer("Road");
        OpenGLShader* shader = GetShader("HeightMapColor");

        if (!gBuffer) return;
        if (!roadFramebuffer) return;
        if (!shader) return;

        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();

        Transform transform;
        transform.scale = glm::vec3(HEIGHTMAP_SCALE_XZ, HEIGHTMAP_SCALE_Y, HEIGHTMAP_SCALE_XZ);
        glm::mat4 modelMatrix = transform.to_mat4();
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });

        shader->Bind();
        shader->SetMat4("modelMatrix", modelMatrix);
        shader->SetMat4("inverseModelMatrix", inverseModelMatrix);
        shader->SetFloat("u_textureScaling", 1);
        shader->SetFloat("u_worldWidth", World::GetWorldSpaceWidth());
        shader->SetFloat("u_worldDepth", World::GetWorldSpaceDepth());

        SetRasterizerState("GeometryPass_NonBlended");

        Material* material = AssetManager::GetDefaultMaterial();
        int materialIndex = AssetManager::GetMaterialIndexByName("Ground_MudVeg");
        material = AssetManager::GetMaterialByIndex(materialIndex);

        Material* dirtRoadMaterial = AssetManager::GetMaterialByName("DirtRoad");

        if (Editor::IsOpen() && Editor::GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) {
            material = AssetManager::GetDefaultMaterial();
            shader->SetFloat("u_textureScaling", 0.1);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_basecolor)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_normal)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_rma)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(dirtRoadMaterial->m_basecolor)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(dirtRoadMaterial->m_normal)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(dirtRoadMaterial->m_rma)->GetGLTexture().GetHandle());;
        glBindTextureUnit(6, roadFramebuffer->GetColorAttachmentHandleByName("RoadMask"));

        glBindVertexArray(heightMapMesh.GetVAO());

        int verticesPerChunk = 33 * 33;
        int verticesPerHeightMap = verticesPerChunk * 8 * 8;
        int indicesPerChunk = 32 * 32 * 6;
        int indicesPerHeightMap = indicesPerChunk * 8 * 8;

        int culled = 0;

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            Frustum& frustum = viewport->GetFrustum();

            int test = 0;
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                std::vector<HeightMapChunk>& chunks = World::GetHeightMapChunks();

                //std::cout << "chunks.size(): " << chunks.size() << "\n";
                
                for (HeightMapChunk& chunk : chunks) {

                    if (Editor::IsClosed()) {
                        if (!frustum.IntersectsAABBFast(AABB(chunk.aabbMin, chunk.aabbMax))) {
                            culled++;
                            continue;
                        }
                    }

                    int indexCount = INDICES_PER_CHUNK;
                    int baseVertex = 0;
                    int baseIndex = chunk.baseIndex;
                    void* indexOffset = (GLvoid*)(baseIndex * sizeof(GLuint));
                    int instanceCount = 1;
                    int viewportIndex = i;
                    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, instanceCount, baseVertex, viewportIndex);
                }
            }
        }
        glBindVertexArray(0);
        //std::cout << "Culled: " << culled << "\n";
    }


    void ReadBackHeightMapData(Map* map) {
        if (!map) {
            Logging::Error() << "OpenGLRenderer::ReadBackHeightMapData() failed coz map was nullptr";
            return;
        }

        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        if (!worldFramebuffer) {
            Logging::Error() << "OpenGLRenderer::ReadBackHeightMapData() failed coz could not retrieve World framebuffer";
            return;
        }

        GLuint textureHandle = worldFramebuffer->GetColorAttachmentHandleByName("HeightMap");

        GLuint width = map->GetTextureWidth();
        GLuint height = map->GetTextureHeight();
        size_t dataSize = width * height * sizeof(float);

        map->GetHeightMapData().resize(width * height);

        // Readback
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glGetTextureSubImage(textureHandle, 0, 0, 0, 0, width, height, 1, GL_RED, GL_FLOAT, dataSize, map->GetHeightMapData().data());

        Logging::Debug() << "ReadBackHeightMapData() width: " << width;
        Logging::Debug() << "ReadBackHeightMapData() height: " << height;
        Logging::Debug() << "ReadBackHeightMapData() dataSize: " << dataSize;
    }
}