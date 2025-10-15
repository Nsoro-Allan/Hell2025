#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Core/Game.h"
#include "Viewport/ViewportManager.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"
#include "Modelling/Clipping.h"
#include "Modelling/Unused/Modelling.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void InventoryGaussianPass() {
        if (Editor::IsOpen()) return;

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        if (!gBuffer) return;

        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            Player* player = Game::GetLocalPlayerByIndex(i);

            if (!viewport->IsVisible()) continue;
            if (!player->InventoryIsOpen()) continue;

            SpaceCoords gBufferSpaceCooords = viewport->GetGBufferSpaceCoords();


            if (Game::GetSplitscreenMode() == SplitscreenMode::FULLSCREEN && i == 0) {
                BlitRect blitRect;
                blitRect.x0 = 0;
                blitRect.x1 = 1920;
                blitRect.y0 = 0;
                blitRect.y1 = 1080;
                GaussianBlur(gBuffer, gBuffer, "FinalLighting", "FinalLighting", blitRect, blitRect, 5, 1);
            }

            if (Game::GetSplitscreenMode() == SplitscreenMode::TWO_PLAYER && i == 0) {
                BlitRect blitRect;
                blitRect.x0 = 0;
                blitRect.x1 = 1920;
                blitRect.y0 = 540;
                blitRect.y1 = 1080;
                GaussianBlur(gBuffer, gBuffer, "FinalLighting", "FinalLighting", blitRect, blitRect, 5, 1);
            }

            if (Game::GetSplitscreenMode() == SplitscreenMode::TWO_PLAYER && i == 1) {
                BlitRect blitRect;
                blitRect.x0 = 0;
                blitRect.x1 = 1920;
                blitRect.y0 = 0;
                blitRect.y1 = 540;
                GaussianBlur(gBuffer, gBuffer, "FinalLighting", "FinalLighting", blitRect, blitRect, 5, 1);
            }
        }
    }

    void ExamineItemPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("ExamineItem");
        if (!gBuffer) return;
        if (!shader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "FinalLighting" });

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        gBuffer->ClearDepthAttachment();

        SetRasterizerState("GeometryPass_NonBlended");

        glm::vec3 cameraPosition = glm::vec3(0, 0, 1.5f); // Remember the item is rendered at (0,0,0)

        Transform cameraTransform;
        cameraTransform.position = cameraPosition;
        glm::mat4 viewMatrix = glm::inverse(cameraTransform.to_mat4());

        shader->Bind();
        shader->SetMat4("u_model", glm::mat4(1));
        shader->SetMat4("u_viewMatrix", viewMatrix);
        shader->SetVec3("u_viewPos", cameraPosition);

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        // Non blended
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            Player* player = Game::GetLocalPlayerByIndex(i);

            if (!viewport->IsVisible()) continue;
            if (player->InventoryIsClosed()) continue;
            if (player->GetInvetoryState() != InventoryState::EXAMINE_ITEM) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);                
                //float m_fov = glm::radians(20.0f);
                //float m_aspect = 1920.0f / 1080.0f;
                //float m_nearPlane = NEAR_PLANE;
                //float m_farPlane = FAR_PLANE;
                //glm::mat4 perspectiveMatrix = glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
                //shader->SetMat4("u_poMatrix", perspectiveMatrix); // make this a per viewport "inventory perspective matrix"

                shader->SetInt("u_viewportIndex", 0);

                Inventory& inventory = player->GetInventory();
                std::vector<RenderItem> m_renderItems = inventory.GetRenderItems();

                for (RenderItem& renderItem : m_renderItems) {
                    Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                    shader->SetMat4("u_model", renderItem.modelMatrix);
                    shader->SetMat4("u_inverseModel", renderItem.inverseModelMatrix);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());

                    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);
                }
        }

    }
}


