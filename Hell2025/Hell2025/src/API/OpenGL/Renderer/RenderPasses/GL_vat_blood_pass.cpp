#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void VatBloodPass() {
        ProfilerOpenGLZoneFunction();

        SetRasterizerState("GeometryPass_Default");

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* shader = GetShader("VatBlood");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        if (!shader) return;
        if (!gBuffer) return;

        shader->Bind();

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });

        static int textureIndexBloodPos4 = AssetManager::GetTextureIndexByName("blood_pos4");
        static int textureIndexBloodPos6 = AssetManager::GetTextureIndexByName("blood_pos6");
        static int textureIndexBloodPos7 = AssetManager::GetTextureIndexByName("blood_pos7");
        static int textureIndexBloodPos9 = AssetManager::GetTextureIndexByName("blood_pos9");
        static int textureIndexBloodNorm4 = AssetManager::GetTextureIndexByName("blood_norm4");
        static int textureIndexBloodNorm6 = AssetManager::GetTextureIndexByName("blood_norm6");
        static int textureIndexBloodNorm7 = AssetManager::GetTextureIndexByName("blood_norm7");
        static int textureIndexBloodNorm9 = AssetManager::GetTextureIndexByName("blood_norm9");
        static int meshIndex4 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("blood_mesh4"))->GetMeshIndices()[0];
        static int meshIndex6 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("blood_mesh6"))->GetMeshIndices()[0];
        static int meshIndex7 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("blood_mesh7"))->GetMeshIndices()[0];
        static int meshIndex9 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("blood_mesh9"))->GetMeshIndices()[0];

        std::vector<VolumetricBloodSplatter>& volumetricBloodSplatters = World::GetVolumetricBloodSplatters();

        static std::vector<RenderItem> renderItems;
        renderItems.clear();

        for (VolumetricBloodSplatter& vatBlood : volumetricBloodSplatters) {
            RenderItem& renderItem = renderItems.emplace_back();
            renderItem.modelMatrix = vatBlood.GetModelMatrix();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.emissiveR = vatBlood.GetLifeTime();

            if (vatBlood.GetType() == 4) {
                renderItem.baseColorTextureIndex = textureIndexBloodPos4;
                renderItem.normalMapTextureIndex = textureIndexBloodNorm4;
                renderItem.meshIndex = meshIndex4;
            }
            else if (vatBlood.GetType() == 6) {
                renderItem.baseColorTextureIndex = textureIndexBloodPos6;
                renderItem.normalMapTextureIndex = textureIndexBloodNorm6;
                renderItem.meshIndex = meshIndex6;
            }
            else if (vatBlood.GetType() == 7) {
                renderItem.baseColorTextureIndex = textureIndexBloodPos7;
                renderItem.normalMapTextureIndex = textureIndexBloodNorm7;
                renderItem.meshIndex = meshIndex7;
            }
            else if (vatBlood.GetType() == 9) {
                renderItem.baseColorTextureIndex = textureIndexBloodPos9;
                renderItem.normalMapTextureIndex = textureIndexBloodNorm9;
                renderItem.meshIndex = meshIndex9;
            }
        }

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader->SetInt("u_viewportIndex", i);

            for (RenderItem& renderItem: renderItems) {

                Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                if (!mesh) continue;

                shader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
                shader->SetMat4("u_inverseModelMatrix", renderItem.inverseModelMatrix);
                shader->SetFloat("u_time", renderItem.emissiveR);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());

                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
            }
        }

    }
}
