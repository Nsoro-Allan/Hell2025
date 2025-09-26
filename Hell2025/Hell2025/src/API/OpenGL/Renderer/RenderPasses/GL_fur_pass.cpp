#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void FurPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer"); 
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");
        OpenGLShader* shader = GetShader("Fur");
        OpenGLShader* compositeShader = GetShader("FurComposite");
        OpenGLShadowCubeMapArray* hiResShadowMaps = GetShadowCubeMapArray("HiRes");
        OpenGLShadowMap* flashLightShadowMapsFBO = GetShadowMap("FlashlightShadowMaps");

        if (!gBuffer) return;
        if (!hairFrameBuffer) return;
        if (!shader) return;
        if (!compositeShader) return;
        if (!hiResShadowMaps) return;
        if (!flashLightShadowMapsFBO) return;

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "FinalLighting" });

        shader->Bind();

        SetRasterizerState("GeometryPass_Blended");

        static bool skip = false;
        if (Input::KeyPressed(HELL_KEY_X)) {
            skip = !skip;
        }
        if (skip) {
            return;
        }

        glEnable(GL_BLEND);
        int hairLayerCount = 25;

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("BlueNoise")->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, hiResShadowMaps->GetDepthTexture());
        glBindTextureUnit(5, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        glBindTextureUnit(6, flashLightShadowMapsFBO->GetDepthTextureHandle());

        // Non skinned models
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);

                if (World::GetGameObjects().size() == 0) {
                    continue;
                }

                GameObject& bunny = World::GetGameObjects()[0];

                for (const RenderItem& renderItem : bunny.GetRenderItems()) {

                    uint32_t meshIndex = renderItem.meshIndex;
                    glm::mat4 modelMatrix = renderItem.modelMatrix;
                    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

                    Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);

                    shader->SetMat4("u_model", modelMatrix);
                    shader->SetMat3("u_normalMatrix", normalMatrix);
                    shader->SetInt("u_viewportIndex", i);
                    shader->SetInt("u_hairLayerCount", hairLayerCount);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE7);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("Kangaroo_FurMask")->GetGLTexture().GetHandle());

                    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), hairLayerCount, mesh->baseVertex);
                    
                }
            }
        }

        // Skinned models
        //glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
        //glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());
        //
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("WaterNormals")->GetGLTexture().GetHandle());
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, hiResShadowMaps->GetDepthTexture());
        //glBindTextureUnit(5, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        //glBindTextureUnit(6, flashLightShadowMapsFBO->GetDepthTextureHandle());
        //
        //if (false)
        //for (int i = 0; i < 4; i++) {
        //    Viewport* viewport = ViewportManager::GetViewportByIndex(i);
        //    if (viewport->IsVisible()) {
        //        OpenGLRenderer::SetViewport(gBuffer, viewport);
        //
        //        std::vector<AnimatedGameObject*> animatedgameObjects = RenderDataManager::GetAnimatedGameObjectToSkin();
        //
        //        for (AnimatedGameObject* animatedGameObject : animatedgameObjects) {
        //
        //            std::vector<RenderItem>& renderItems = animatedGameObject->GetRenderItems();
        //
        //            for (const RenderItem& renderItem : renderItems) {
        //
        //                if (renderItem.furLength == 0) continue;
        //
        //                uint32_t meshIndex = renderItem.meshIndex;
        //                glm::mat4 modelMatrix = renderItem.modelMatrix;
        //                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        //
        //                SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshIndex);
        //
        //                shader->SetMat4("u_model", modelMatrix);
        //                shader->SetMat3("u_normalMatrix", normalMatrix);
        //                shader->SetInt("u_viewportIndex", i);
        //                shader->SetInt("u_hairLayerCount", hairLayerCount);
        //
        //                glActiveTexture(GL_TEXTURE0);
        //                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
        //                glActiveTexture(GL_TEXTURE1);
        //                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
        //                glActiveTexture(GL_TEXTURE2);
        //                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
        //
        //                glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), hairLayerCount, renderItem.baseSkinnedVertex);
        //            }
        //        }
        //    }
        //}

        glDisable(GL_BLEND);
    }


/*
    void FurPass() {


        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");
        OpenGLShader* shader = GetShader("Fur");
        OpenGLShader* compositeShader = GetShader("FurComposite");
        OpenGLShadowCubeMapArray* hiResShadowMaps = GetShadowMapArray("HiRes");
        OpenGLShadowMap* flashLightShadowMapsFBO = GetShadowMap("FlashlightShadowMaps");

        if (!gBuffer) return;
        if (!hairFrameBuffer) return;
        if (!shader) return;
        if (!compositeShader) return;
        if (!hiResShadowMaps) return;
        if (!flashLightShadowMapsFBO) return;

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        hairFrameBuffer->Bind();
        hairFrameBuffer->ClearAttachment("Lighting", 0, 0, 0, 0);
        hairFrameBuffer->DrawBuffers({ "Lighting" });

        shader->Bind();

        SetRasterizerState("GeometryPass_NonBlended");

        static bool skip = false;

        if (Input::KeyPressed(HELL_KEY_X)) {
            skip = !skip;
        }

        if (skip) {
            return;
        }

        glEnable(GL_BLEND);
        int hairLayerCount = 20;

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);

                GameObject& bunny = World::GetGameObjects()[0];

                for (const RenderItem& renderItem : bunny.GetRenderItems()) {

                    uint32_t meshIndex = renderItem.meshIndex;
                    glm::mat4 modelMatrix = renderItem.modelMatrix;

                    Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);

                    //if (mesh->name != "fur") continue;

                    shader->SetMat4("u_model", modelMatrix);
                    shader->SetInt("u_viewportIndex", i);
                    shader->SetInt("u_hairLayerCount", hairLayerCount);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("BlueNoise")->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, hiResShadowMaps->GetDepthTexture());
                    glBindTextureUnit(5, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
                    glBindTextureUnit(6, flashLightShadowMapsFBO->GetDepthTextureHandle());

                    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), hairLayerCount, mesh->baseVertex);
                }
            }
        }



        glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
        glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());

        glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
        glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);

                std::vector<AnimatedGameObject*> animatedgameObjects = RenderDataManager::GetAnimatedGameObjectToSkin();

                for (AnimatedGameObject* animatedGameObject : animatedgameObjects) {

                    std::vector<RenderItem>& renderItems = animatedGameObject->GetRenderItems();

                    for (const RenderItem& renderItem : renderItems) {

                        if (renderItem.furLength == 0) continue;

                        uint32_t meshIndex = renderItem.meshIndex;
                        glm::mat4 modelMatrix = renderItem.modelMatrix;

                        SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshIndex);

                        shader->SetMat4("u_model", modelMatrix);
                        shader->SetInt("u_viewportIndex", i);
                        shader->SetInt("u_hairLayerCount", hairLayerCount);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("BlueNoise")->GetGLTexture().GetHandle());
                        glActiveTexture(GL_TEXTURE4);
                        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, hiResShadowMaps->GetDepthTexture());
                        glBindTextureUnit(5, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
                        glBindTextureUnit(6, flashLightShadowMapsFBO->GetDepthTextureHandle());

                        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), hairLayerCount, renderItem.baseSkinnedVertex);
                    }
                }
            }
        }

        // Add fur to final image
        compositeShader->Bind();
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindTextureUnit(1, hairFrameBuffer->GetColorAttachmentHandleByName("Lighting"));
        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }
*/
}