#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include "UniqueID.h"

namespace OpenGLRenderer {

    void WinstonPass() {
        
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* shader = GetShader("Winston");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        static float time = 0.0f;
        time += Game::GetDeltaTime();

        shader->Bind();
        shader->SetVec3("color", { 0, 0.9f, 1 });
        shader->SetFloat("alpha", 0.01f);
        shader->SetVec2("screensize", gBuffer->GetWidth(), gBuffer->GetHeight());
        shader->SetFloat("near", NEAR_PLANE);
        shader->SetFloat("far", FAR_PLANE);
        shader->SetFloat("u_time", time);

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDepthFunc(GL_EQUAL);

        glBindTextureUnit(0, gBuffer->GetDepthAttachmentHandle());
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            glm::mat4 projectionMatrix = viewportData[i].projection;
            glm::mat4 viewMatrix = viewportData[i].view;

            shader->SetMat4("projection", projectionMatrix);
            shader->SetMat4("view", viewMatrix);
            shader->SetBool("useUniformColor", false);

            Player* player = Game::GetLocalPlayerByIndex(i);
            if (!player) continue;

            if (player->InteractFound()) {

                uint64_t interactObjectId = player->GetInteractObjectId();
                ObjectType interactObjectType = UniqueID::GetType(interactObjectId);

                if (interactObjectType == ObjectType::PICK_UP) {
                    PickUp* pickUp = World::GetPickUpByObjectId(interactObjectId);
                    if (pickUp) {
                        const std::vector<RenderItem>& renderItems = pickUp->GetRenderItems();

                        for (const RenderItem& renderItem : renderItems) {

                            shader->SetMat4("model", renderItem.modelMatrix);

                            Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                            if (!mesh) continue;

                            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                        }
                    }
                }
            }
        } 
        
        glDepthFunc(GL_LEQUAL);
    }
}