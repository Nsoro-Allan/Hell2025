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
        OpenGLShader* shader = GetShader("ShadowMap");
        OpenGLShadowMap* shadowMapsFBO = GetShadowMap("FlashlightShadowMaps");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();
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
            for (RagdollV2& ragdoll : RagdollManager::GetRagdolls()) {
                MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
                if (meshBuffer.GetIndices().size() == 0) continue;

                glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());
                for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
                    Mesh* mesh = meshBuffer.GetMeshByIndex(j);
                    glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
                    shader->SetMat4("u_modelMatrix", modelMatrix);
                    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                }
            }
        }

        glBindVertexArray(0);
        glCullFace(GL_BACK);
    }

    void RenderPointLightShadowMaps() {

        OpenGLShader* shader = GetShader("ShadowCubeMap");
        OpenGLShadowCubeMapArray* hiResShadowMaps = GetShadowCubeMapArray("HiRes");
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        if (!shader) return;
        if (!hiResShadowMaps) return;

        shader->Bind();
        shader->SetBool("u_useInstanceData", true);

        //hiResShadowMaps->ClearDepthLayers(1.0f);

        const std::vector<GPULight>& gpuLightsHighRes = RenderDataManager::GetGPULightsHighRes();

        // Clear any shadow map that needs redrawing
        for (int i = 0; i < gpuLightsHighRes.size(); i++) {
            const GPULight& gpuLight = gpuLightsHighRes[i];
            Light* light = World::GetLightByIndex(gpuLight.lightIndex);

            if (light->IsDirty()) {
                hiResShadowMaps->ClearDepthLayer(i, 1.0f);
            }
        }

        //GLuint shadowMapTextureID = hiResShadowMaps->GetDepthTexture();
        //if (shadowMapTextureID != 0) {
        //    float clearDepthValue = 1.0f;
        //    glClearTexImage(shadowMapTextureID, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &clearDepthValue);
        //}
        //else {
        //    std::cout << "Error: Invalid shadow map texture handle for clearing.\n";
        //}

        glDepthMask(true);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, hiResShadowMaps->GetSize(), hiResShadowMaps->GetSize());
        glBindFramebuffer(GL_FRAMEBUFFER, hiResShadowMaps->GetHandle());

        glCullFace(GL_FRONT);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

 //     const std::vector<GPULight>& gpuLightsHighRes = RenderDataManager::GetGPULightsHighRes();

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
            }
        }

        size_t maxLights = 4;
        size_t lightCount = std::min(maxLights, World::GetLights().size());

        OpenGLMeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer().GetGLMeshBuffer();
        glBindVertexArray(houseMeshBuffer.GetVAO());
        shader->SetBool("u_useInstanceData", false);
        shader->SetMat4("u_modelMatrix", glm::mat4(1.0f));

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
                for (RagdollV2& ragdoll : RagdollManager::GetRagdolls()) {
                    MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
                    if (meshBuffer.GetIndices().size() == 0) continue;

                    glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());
                    for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
                        Mesh* mesh = meshBuffer.GetMeshByIndex(j);
                        glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
                        shader->SetMat4("u_modelMatrix", modelMatrix);
                        glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                    }
                }
            }
        }

        glCullFace(GL_BACK);
    }


    void RenderMoonLightCascadedShadowMaps() {

        const ViewportData& viewportData = RenderDataManager::GetViewportData()[0];

        glm::vec3 lightDir = Game::GetMoonlightDirection();
        glm::mat4 viewMatrix = viewportData.view;
        
        float viewportWidth = viewportData.width;
        float viewportHeight = viewportData.height;
        float fov = viewportData.fov;

        std::vector<float>& cascadeLevels = GetShadowCascadeLevels();

        const std::vector<glm::mat4>& lightProjectionViews = Util::GetLightProjectionViews(viewMatrix, lightDir, cascadeLevels, viewportWidth, viewportHeight, fov);

        OpenGLSSBO* lightProjViewSSBO = GetSSBO("CSMLightProjViewMatrices");
        OpenGLShader* shader = GetShader("ShadowMap");
        OpenGLShadowMapArray* shadowMapArray = GetShadowMapArray("MoonlightPlayer1");

        if (!lightProjViewSSBO) return;
        if (!shader) return;

        lightProjViewSSBO->Update(sizeof(glm::mat4x4) * lightProjectionViews.size(), &lightProjectionViews[0]);
        lightProjViewSSBO->Bind(15);
        
        shader->Bind();
        shader->SetBool("u_useInstanceData", false);
        
        size_t numLayers = lightProjectionViews.size();
        
        shadowMapArray->Bind();
        shadowMapArray->SetViewport();

        glDisable(GL_CULL_FACE);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);  // peter panning

        for (size_t i = 0; i < numLayers; ++i) {

            shadowMapArray->SetTextureLayer(i);
            shadowMapArray->ClearDepth();

        
            shader->SetMat4("u_projectionView", lightProjectionViews[i]);
                    
            // Geometry
            glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
            for (const RenderItem& renderItem : RenderDataManager::GetRenderItems()) {
                uint32_t meshIndex = renderItem.meshIndex;
                glm::mat4 modelMatrix = renderItem.modelMatrix;
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
                Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
                shader->SetMat4("u_modelMatrix", modelMatrix);
                glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex);
            }

            // House
            shader->SetMat4("u_modelMatrix", glm::mat4(1.0f));
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
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}