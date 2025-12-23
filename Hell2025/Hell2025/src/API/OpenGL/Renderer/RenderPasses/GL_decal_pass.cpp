#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void DecalPass() {
        ProfilerOpenGLZoneFunction();

        SetRasterizerState("DecalPass");

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        const std::vector<RenderItem>& decalRenderItems = RenderDataManager::GetDecalRenderItems();

        OpenGLShader* shader = GetShader("Decals");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        shader->Bind();

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader->SetInt("u_viewportIndex", i);

            for (const RenderItem& renderItem : decalRenderItems) {
                Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                if (!mesh) continue;
                                
                shader->SetMat4("u_modelMatrix", renderItem.modelMatrix);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());

                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
            }
        }
    }
}