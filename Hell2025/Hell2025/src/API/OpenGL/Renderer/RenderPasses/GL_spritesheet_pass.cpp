#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Util/Util.h"
#include "World/World.h"

#include "Core/Game.h"
#include "Renderer/Renderer.h"

namespace OpenGLRenderer {

    void SpriteSheetPass() {
        ProfilerOpenGLZoneFunction();

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLShader* shader = GetShader("SpriteSheet");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Quad");

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");
        shader->Bind();
        SetRasterizerState("SpriteSheetPass");



        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

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
                shader->SetInt("u_isBillboard", false); // check this shit, on the muzzleflash createinfo coz u have wrong uniform name here!!!!!!!
                shader->SetFloat("u_uOffset", renderItem.uOffset);
                shader->SetFloat("u_vOffset", renderItem.vOffset);
                shader->SetVec4("u_worldBoundsMin", renderItem.aabbMin);
                shader->SetVec4("u_worldBoundsMax", renderItem.aabbMax);
                shader->SetBool("u_useFireClipHeight", false);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex, i);
            }       

            // No depth test for fire
            //glDisable(GL_DEPTH_TEST);
            for (Fireplace& fireplace : World::GetFireplaces()) {
                const SpriteSheetRenderItem& renderItem = fireplace.GetFireSpriteSheetRenderItem();
                Texture* texture = AssetManager::GetTextureByIndex(renderItem.textureIndex);
                shader->SetInt("u_rowCount", renderItem.rowCount);
                shader->SetInt("u_columnCount", renderItem.columnCount);
                shader->SetInt("u_frameIndex", renderItem.frameIndex);
                shader->SetInt("u_frameNextIndex", renderItem.frameIndexNext);
                shader->SetFloat("u_mixFactor", renderItem.mixFactor);
                shader->SetVec4("u_position", renderItem.position);
                shader->SetVec4("u_rotation", renderItem.rotation);
                shader->SetVec4("u_scale", renderItem.scale);
                shader->SetInt("u_billboard", renderItem.isBillboard);
                shader->SetFloat("u_uOffset", renderItem.uOffset);
                shader->SetFloat("u_vOffset", renderItem.vOffset);
                shader->SetVec4("u_worldBoundsMin", renderItem.aabbMin);
                shader->SetVec4("u_worldBoundsMax", renderItem.aabbMax);
                shader->SetBool("u_useFireClipHeight", fireplace.m_useFireClipHeight);

                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex, i);
            }
        }
    }
}