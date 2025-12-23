#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

#include "Ragdoll/RagdollManager.h"

namespace OpenGLRenderer {

    void RenderFlashLightShadowMaps();
    void RenderPointLightShadowMaps();
    void RenderMoonLightCascadedShadowMaps();

    void RenderShadowMaps() {
        RenderFlashLightShadowMaps();
        RenderPointLightShadowMaps();
        RenderMoonLightCascadedShadowMaps();
    }

    void RenderFlashLightShadowMaps() {
        ProfilerOpenGLZoneFunction();

        OpenGLShader* shader = GetShader("ShadowMap");
        OpenGLShadowMap* shadowMapsFBO = GetShadowMap("FlashlightShadowMaps");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        //const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();
        const FlashLightShadowMapDrawInfo& flashLightShadowMapDrawInfo = RenderDataManager::GetFlashLightShadowMapDrawInfo();
        
        glm::mat4 heightMapModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(HEIGHTMAP_SCALE_XZ, HEIGHTMAP_SCALE_Y, HEIGHTMAP_SCALE_XZ)); // move to heightmap manager

        glEnable(GL_DEPTH_TEST);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        shadowMapsFBO->Bind();
        shadowMapsFBO->SetViewport();

        shader->Bind();

        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            shadowMapsFBO->BindLayer(i);
            shadowMapsFBO->ClearLayer(i);

            glm::mat4 lightProjectionView = Game::GetLocalPlayerByIndex(i)->GetFlashlightProjectionView();
            shader->SetMat4("u_projectionView", lightProjectionView);

            Frustum frustum;
            frustum.Update(lightProjectionView);

            // Scene geometry
            shader->SetBool("u_useInstanceData", true);
            glCullFace(GL_FRONT);
            glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

            MultiDrawIndirect(flashLightShadowMapDrawInfo.flashlightShadowMapGeometry[i]);

            // Heightfield chunks
            std::vector<HeightMapChunk>& chunks = World::GetHeightMapChunks();
            OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
            glBindVertexArray(heightMapMesh.GetVAO());
            shader->SetMat4("u_modelMatrix", heightMapModelMatrix);
            shader->SetBool("u_useInstanceData", false);

            for (uint32_t chunkIndex : flashLightShadowMapDrawInfo.heightMapChunkIndices[i]) {
                HeightMapChunk& chunk = chunks[chunkIndex];
                int indexCount = INDICES_PER_CHUNK;
                int baseVertex = 0;
                int baseIndex = chunk.baseIndex;
                void* indexOffset = (GLvoid*)(baseIndex * sizeof(GLuint));
                int instanceCount = 1;
                int viewportIndex = i;
                if (indexCount > 0) {
                    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, instanceCount, baseVertex, viewportIndex);
                }
            }

            // House render items
            OpenGLMeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer().GetGLMeshBuffer();
            glBindVertexArray(houseMeshBuffer.GetVAO());
            shader->SetMat4("u_modelMatrix", glm::mat4(1.0f));

            const std::vector<HouseRenderItem>& renderItems = flashLightShadowMapDrawInfo.houseMeshRenderItems[i];
            for (const HouseRenderItem& renderItem : renderItems) {
                int indexCount = renderItem.indexCount;
                int baseVertex = renderItem.baseVertex;
                int baseIndex = renderItem.baseIndex;
                glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);
            }

            // Ragdoll
            //auto& ragdolls = RagdollManager::GetRagdolls();
            //for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
            //    RagdollV2& ragdoll = it->second;
            //    std::cout << "hi\n";
            //    if (ragdoll.RenderingEnabled()) {
            //        MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
            //        if (meshBuffer.GetIndices().size() == 0) continue;
            //
            //        glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());
            //        for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
            //            Mesh* mesh = meshBuffer.GetMeshByIndex(j);
            //            glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
            //            shader->SetMat4("u_modelMatrix", modelMatrix);
            //            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
            //        }
            //    }
            //    ++it;
            //}
        }

        glBindVertexArray(0);
        glCullFace(GL_BACK);
    }

    void RenderPointLightShadowMaps() {
        ProfilerOpenGLZoneFunction();

        OpenGLShader* shader = GetShader("ShadowCubeMap");
        OpenGLShadowCubeMapArray* hiResShadowMaps = GetShadowCubeMapArray("HiRes");
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        if (!shader) return;
        if (!hiResShadowMaps) return;

        shader->Bind();
        shader->SetBool("u_useInstanceData", true);

        const std::vector<GPULight>& gpuLightsHighRes = RenderDataManager::GetGPULightsHighRes();

        // Clear any shadow map that needs redrawing
        for (int i = 0; i < gpuLightsHighRes.size(); i++) {
            const GPULight& gpuLight = gpuLightsHighRes[i];
            Light* light = World::GetLightByIndex(gpuLight.lightIndex);

            if (light->IsDirty()) {
                hiResShadowMaps->ClearDepthLayer(i, 1.0f);
            }
        }

        glDepthMask(true);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, hiResShadowMaps->GetSize(), hiResShadowMaps->GetSize());
        glBindFramebuffer(GL_FRAMEBUFFER, hiResShadowMaps->GetHandle());

        glCullFace(GL_FRONT);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < gpuLightsHighRes.size(); i++) {
            const GPULight& gpuLight = gpuLightsHighRes[i];

            Light* light = World::GetLightByIndex(gpuLight.lightIndex);
            if (!light || !light->IsDirty()) continue;

            shader->SetFloat("farPlane", light->GetRadius());
            shader->SetVec3("lightPosition", light->GetPosition());
            shader->SetMat4("shadowMatrices[0]", light->GetProjectionView(0));
            shader->SetMat4("shadowMatrices[1]", light->GetProjectionView(1));
            shader->SetMat4("shadowMatrices[2]", light->GetProjectionView(2));
            shader->SetMat4("shadowMatrices[3]", light->GetProjectionView(3));
            shader->SetMat4("shadowMatrices[4]", light->GetProjectionView(4));
            shader->SetMat4("shadowMatrices[5]", light->GetProjectionView(5));

            for (int face = 0; face < 6; ++face) {
                GLuint layer = i * 6 + face;
                shader->SetInt("faceIndex", face);
                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, hiResShadowMaps->GetDepthTexture(), 0, layer);
                MultiDrawIndirect(drawInfoSet.shadowMapHiRes[i][face]);

                //const std::vector<DrawIndexedIndirectCommand>& commands = drawInfoSet.shadowMapHiRes[i][face];
                //std::cout << gpuLight.lightIndex << ": " << face << " " << commands.size() << "\n";

            }
        }

        OpenGLMeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer().GetGLMeshBuffer();
        glBindVertexArray(houseMeshBuffer.GetVAO());
        shader->SetBool("u_useInstanceData", false);
        shader->SetMat4("u_modelMatrix", glm::mat4(1.0f));

        // OPTIMIZE ME! 
        // Make lights store a list of their HouseRenderItems per frustum face that is only updated when the map changes
        // That will be when a HousePlane or Wall is added/modified

        for (int i = 0; i < gpuLightsHighRes.size(); i++) {
            const GPULight& gpuLight = gpuLightsHighRes[i];

            Light* light = World::GetLightByIndex(gpuLight.lightIndex);
            if (!light || !light->IsDirty()) continue;

            shader->SetFloat("farPlane", light->GetRadius());
            shader->SetVec3("lightPosition", light->GetPosition());
            shader->SetMat4("shadowMatrices[0]", light->GetProjectionView(0));
            shader->SetMat4("shadowMatrices[1]", light->GetProjectionView(1));
            shader->SetMat4("shadowMatrices[2]", light->GetProjectionView(2));
            shader->SetMat4("shadowMatrices[3]", light->GetProjectionView(3));
            shader->SetMat4("shadowMatrices[4]", light->GetProjectionView(4));
            shader->SetMat4("shadowMatrices[5]", light->GetProjectionView(5));

            for (int face = 0; face < 6; ++face) {
                shader->SetInt("faceIndex", face);
                int shadowMapIndex = i;
                GLuint layer = shadowMapIndex * 6 + face;

                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, hiResShadowMaps->GetDepthTexture(), 0, layer);
                
                Frustum* frustum = light->GetFrustumByFaceIndex(face);
                if (!frustum) return;

                const std::vector<HouseRenderItem>& renderItems = RenderDataManager::GetHouseRenderItems();
                for (const HouseRenderItem& renderItem : renderItems) {

                    if (!frustum->IntersectsAABBFast(renderItem)) continue;

                    int indexCount = renderItem.indexCount;
                    int baseVertex = renderItem.baseVertex;
                    int baseIndex = renderItem.baseIndex; 
                    glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);
                }
            }
        }


        return;


        // Ragdoll
        //RagdollInfo& ragdoll = RagdollManager::GetRagdoll();
        //MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
        //glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());
        shader->SetBool("u_useInstanceData", false);

        for (int i = 0; i < gpuLightsHighRes.size(); i++) {
            const GPULight& gpuLight = gpuLightsHighRes[i];

            Light* light = World::GetLightByIndex(gpuLight.lightIndex);
            if (!light || !light->IsDirty()) continue;

            shader->SetFloat("farPlane", light->GetRadius());
            shader->SetVec3("lightPosition", light->GetPosition());
            shader->SetMat4("shadowMatrices[0]", light->GetProjectionView(0));
            shader->SetMat4("shadowMatrices[1]", light->GetProjectionView(1));
            shader->SetMat4("shadowMatrices[2]", light->GetProjectionView(2));
            shader->SetMat4("shadowMatrices[3]", light->GetProjectionView(3));
            shader->SetMat4("shadowMatrices[4]", light->GetProjectionView(4));
            shader->SetMat4("shadowMatrices[5]", light->GetProjectionView(5));

            for (int face = 0; face < 6; ++face) {
                shader->SetInt("faceIndex", face);
                int shadowMapIndex = i;
                GLuint layer = shadowMapIndex * 6 + face;

                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, hiResShadowMaps->GetDepthTexture(), 0, layer);

                Frustum* frustum = light->GetFrustumByFaceIndex(face);
                if (!frustum) return;

                // Ragdoll
                //auto& ragdolls = RagdollManager::GetRagdolls();
                //for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
                //    RagdollV2& ragdoll = it->second;
                //
                //    if (ragdoll.RenderingEnabled()) {
                //        MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
                //        if (meshBuffer.GetIndices().size() == 0) continue;
                //
                //        glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());
                //        for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
                //            Mesh* mesh = meshBuffer.GetMeshByIndex(j);
                //            glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
                //            shader->SetMat4("u_modelMatrix", modelMatrix);
                //            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                //        }
                //    }
                //    ++it;
                //}
            }
        }

        glCullFace(GL_BACK);
    }


    void RenderMoonLightCascadedShadowMaps() {
        ProfilerOpenGLZoneFunction();

        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        OpenGLShader* shader = GetShader("ShadowMap");
        OpenGLShadowMapArray* shadowMapArray = GetShadowMapArray("MoonlightCSM");

        if (!shader) return;
        if (!shadowMapArray) return;

        int viewportCount = std::min(4, Game::GetLocalPlayerCount());

        for (int j = 0; j < viewportCount; j++) {
            Player* player = Game::GetLocalPlayerByIndex(j);
            if (!player || !player->ViewportIsVisible()) continue;

            const ViewportData& viewportData = RenderDataManager::GetViewportData()[j];

            shader->Bind();
            shader->SetBool("u_useInstanceData", false);

            size_t numLayers = SHADOW_CASCADE_COUNT;

            shadowMapArray->Bind();
            shadowMapArray->SetViewport();

            glDisable(GL_CULL_FACE);
            //glEnable(GL_CULL_FACE);
            //glCullFace(GL_FRONT);  // peter panning

            for (size_t i = 0; i < numLayers; ++i) {

                //int textureLayer = i + (viewportCount * j * numLayers);
                int textureLayer = int(i) + (j * int(numLayers)); // numLayers == SHADOW_CASCADE_COUNT

                shadowMapArray->SetTextureLayer(textureLayer);
                shadowMapArray->ClearDepth();

                const glm::mat4& lightProjectionView = viewportData.csmLightProjectionView[i];

                shader->SetMat4("u_projectionView", lightProjectionView);

                // Geometry
                glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

                shader->SetBool("u_useInstanceData", true);
                MultiDrawIndirect(drawInfoSet.moonLightCascades[j][i]);

                shader->SetBool("u_useInstanceData", false);
                shader->SetMat4("u_modelMatrix", glm::mat4(1.0f));

                // House
                OpenGLMeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer().GetGLMeshBuffer();
                glBindVertexArray(houseMeshBuffer.GetVAO());
                //glDisable(GL_CULL_FACE);
                const std::vector<HouseRenderItem>& renderItems = RenderDataManager::GetHouseRenderItems();
                for (const HouseRenderItem& renderItem : renderItems) {
                    int indexCount = renderItem.indexCount;
                    int baseVertex = renderItem.baseVertex;
                    int baseIndex = renderItem.baseIndex;
                    glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);
                }
                // glEnable(GL_CULL_FACE);

                 // Weather boards
                MeshBuffer weatherboardMeshBuffer = World::GetWeatherBoardMeshBuffer();
                glBindVertexArray(weatherboardMeshBuffer.GetGLMeshBuffer().GetVAO());
                int indexCount = weatherboardMeshBuffer.GetGLMeshBuffer().GetIndexCount();
                if (indexCount > 0) {
                    int baseIndex = 0;
                    int baseVertex = 0;
                    glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);
                }
            }
        }
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}