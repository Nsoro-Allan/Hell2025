#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void StainedGlassPass() {
        ProfilerOpenGLZoneFunction();

        SetRasterizerState("GlassPass");

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* shader = GetShader("StainedGlass");
        OpenGLShader* compositeShader = GetShader("GlassComposite");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* miscFullSizeFrameBuffer = GetFrameBuffer("MiscFullSize");
        OpenGLShadowMap* flashLightShadowMapsFBO = GetShadowMap("FlashlightShadowMaps");

        if (!shader) return;
        if (!compositeShader) return;
        if (!gBuffer) return;
        if (!flashLightShadowMapsFBO) return;

        shader->Bind();
        shader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glBindTextureUnit(0, gBuffer->GetDepthAttachmentHandle());
        glBindTextureUnit(7, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        glBindTextureUnit(8, flashLightShadowMapsFBO->GetDepthTextureHandle());
        glBindTextureUnit(9, miscFullSizeFrameBuffer->GetColorAttachmentHandleByName("GaussianFinalLightingIntermediate"));

        // Forward render each glass render item into each viewport
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            Player* player = Game::GetLocalPlayerByIndex(i);
            if (!player) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader->SetInt("u_viewportIndex", i);

            // Sort by distance to camera
            static std::vector<RenderItem> sortedRenderItems;
            sortedRenderItems = RenderDataManager::GetStainedGlassRenderItems();

            std::sort(sortedRenderItems.begin(), sortedRenderItems.end(), [player](RenderItem& a, RenderItem& b) {
                float distA = glm::distance(player->GetCameraPosition(), glm::vec3(a.modelMatrix[3]));
                float distB = glm::distance(player->GetCameraPosition(), glm::vec3(b.modelMatrix[3]));
                return distA > distB;
            });

            for (const RenderItem& renderItem : sortedRenderItems) {
                glm::vec3 tintColor = { renderItem.tintColorR , renderItem.tintColorG , renderItem.tintColorB };

                shader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
                shader->SetVec3("u_tintColor", tintColor);
                shader->SetInt("u_meshIndex", renderItem.meshIndex);

                Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                if (!mesh) continue;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());

                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
            }
        }

        // Composite that render back into the lighting texture
        gBuffer->SetViewport();
        compositeShader->Bind();
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, gBuffer->GetColorAttachmentHandleByName("Glass"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
        glDispatchCompute(gBuffer->GetWidth() / 16, gBuffer->GetHeight() / 4, 1);

        glDepthMask(GL_TRUE);
    }
}