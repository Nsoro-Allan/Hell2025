#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Util/Util.h"

#include "Core/Game.h"
#include "Renderer/Renderer.h"

namespace OpenGLRenderer {

    void SpriteSheetPass() {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLShader* shader = GetShader("SpriteSheet");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Quad");

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");
        shader->Bind();
        SetRasterizerState("SpriteSheetPass");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        static bool runOnce = true;
        static SpriteSheetObject spriteSheetObject;
        if (runOnce) {
            SpriteSheetObjectCreateInfo createInfo;
            createInfo.textureName = "FireplaceFire2_8x8";
            createInfo.loop = true;
            createInfo.billboard = true;
            createInfo.renderingEnabled = true;
            createInfo.scale = glm::vec3(1.5f);
            createInfo.uvOffset = glm::vec2(0.0f, 0.8f);
            spriteSheetObject.Init(createInfo);
            runOnce = false;
        }

        glm::vec3 firePosition = glm::vec3(36.0f, 31.0f, 36.0f);
        Renderer::DrawPoint(firePosition, RED);

        spriteSheetObject.Update(Game::GetDeltaTime());
        spriteSheetObject.SetSpeed(30.0f);
        //spriteSheetObject.SetRotation(glm::vec3(HELL_PI, 0.0f, 0.0f));
        spriteSheetObject.SetPosition(firePosition);

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            Player* player = Game::GetLocalPlayerByIndex(i);
            if (!player) continue;

            const std::vector<SpriteSheetRenderItem>& renderItems = player->GetSpriteSheetRenderItems();
            for (const SpriteSheetRenderItem& renderItem : renderItems) {

                Texture* texture = AssetManager::GetTextureByIndex(renderItem.textureIndex);

                shader->SetInt("u_rowCount", renderItem.rowCount);
                shader->SetInt("u_columnCount", renderItem.columnCount);
                shader->SetInt("u_frameIndex", renderItem.frameIndex);
                shader->SetInt("u_frameNextIndex", renderItem.frameIndexNext);
                shader->SetFloat("u_mixFactor", renderItem.mixFactor);
                shader->SetVec4("u_position", renderItem.position);
                shader->SetVec4("u_rotation", renderItem.rotation);
                shader->SetVec4("u_scale", renderItem.scale);
                shader->SetInt("u_isBillboard", renderItem.isBillboard);
                shader->SetFloat("u_uOffset", renderItem.uOffset);
                shader->SetFloat("u_vOffset", renderItem.vOffset);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex, i);
            }       

            // Fire
            const SpriteSheetRenderItem& renderItem = spriteSheetObject.GetRenderItem();
            Texture* texture = AssetManager::GetTextureByIndex(renderItem.textureIndex);
            shader->SetInt("u_rowCount", renderItem.rowCount);
            shader->SetInt("u_columnCount", renderItem.columnCount);
            shader->SetInt("u_frameIndex", renderItem.frameIndex);
            shader->SetInt("u_frameNextIndex", renderItem.frameIndexNext);
            shader->SetFloat("u_mixFactor", renderItem.mixFactor);
            shader->SetVec4("u_position", renderItem.position);
            shader->SetVec4("u_rotation", renderItem.rotation);
            shader->SetVec4("u_scale", renderItem.scale);
            shader->SetInt("u_isBillboard", renderItem.isBillboard);
            shader->SetFloat("u_uOffset", renderItem.uOffset);
            shader->SetFloat("u_vOffset", renderItem.vOffset);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
            glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex, i);
        }
    }
}