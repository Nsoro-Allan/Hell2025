#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void DecalPaintingPass() {
        if (Editor::IsOpen()) return;

        const std::vector<DecalPaintingInfo>& decalPaintingInfoSet = RenderDataManager::GetDecalPaintingInfo();
        for (const DecalPaintingInfo& decalPaintingInfo : decalPaintingInfoSet) {

            OpenGLFrameBuffer* decalPaintingFBO = GetFrameBuffer("DecalPainting");
            OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");
            OpenGLShader* uvShader = GetShader("DecalPaintUVs");
            OpenGLShader* maskShader = GetShader("DecalPaintMask");
            OpenGLTextureArray* woundMaskArray = GetTextureArray("WoundMasks");

            if (!decalPaintingFBO) return;
            if (!decalMasksFBO) return;
            if (!uvShader) return;
            if (!maskShader) return;
            if (!woundMaskArray) return;

            decalMasksFBO->Bind();
            decalMasksFBO->SetViewport();
           
            decalPaintingFBO->Bind();
            decalPaintingFBO->SetViewport();
            decalPaintingFBO->ClearTexImage("UVMap", 0, 0, 0, 1);

            glClear(GL_DEPTH_BUFFER_BIT);

            Player* player = Game::GetLocalPlayerByIndex(0);

            const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
            glm::mat4 viewMatrix = viewportData[0].view;
            glm::mat4 projectionView = viewportData[0].projectionView;

            glm::vec2 decalSize = glm::vec2(0.05f);
            decalSize = glm::vec2(0.15f);
            decalSize *= glm::vec2(0.5f);

            glm::vec3 bulletOrigin = decalPaintingInfo.rayOrigin;
            glm::vec3 bulletDir = -decalPaintingInfo.rayDirection; // FIND OUT WHY THIS NEEDS TO BE NEGATED

        //    glm::vec3 bulletOrigin = viewportData[0].viewPos;
        //    glm::vec3 bulletDir = viewportData[0].cameraForward;

         // std::cout << "viewportData[0].viewPos: " << viewportData[0].viewPos << "\n";
         // std::cout << "viewportData[0].cameraForward: " << viewportData[0].cameraForward << "\n";
         // std::cout << "\n";
         // std::cout << "decalPaintingInfo.rayOrigin: " << decalPaintingInfo.rayOrigin << "\n";
         // std::cout << "decalPaintingInfo.rayDirection: " << decalPaintingInfo.rayDirection << "\n";
         // std::cout << "\n";

            glm::vec3 eye = bulletOrigin;
            glm::vec3 forward = glm::normalize(bulletDir);
            glm::vec3 worldUp = glm::vec3(0, 1, 0);

            // Avoid gimbal if forward equals world up
            if (glm::abs(glm::dot(forward, worldUp)) > 0.99f) {
                worldUp = glm::vec3(1, 0, 0);
            }

            glm::mat4 view = glm::lookAt(eye, eye - forward, worldUp);

            float zNear = 0.001f;
            float zFar = 50.1f;
            float halfW = decalSize.x * 0.5f;
            float halfH = decalSize.y * 0.5f;
            glm::mat4 proj = glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);

            projectionView = proj * view;

            uvShader->Bind();
            uvShader->SetMat4("u_projectionView", projectionView);

            // Regular mesh VAO
            // glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

            // Bind skinned mesh VAO shit
            glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
            glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());

            RenderDataManager::GetAnimatedGameObjectToSkin();





            //OpenGLTextureArray* woundMaskTextureArray = GetTextureArray("WoundMasks");
            //int layerIndex = 0;
            //glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, woundMaskTextureArray->GetHandle(), 0, layerIndex);



            std::vector<RenderItem>& skinnedRenderItems = World::GetSkinnedRenderItems();

            for (const RenderItem& renderItem : skinnedRenderItems) {

                if (renderItem.customFlag == 1) {
                    uint32_t meshIndex = renderItem.meshIndex;
                    glm::mat4 modelMatrix = renderItem.modelMatrix;
                    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
                    SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshIndex);
                    uvShader->SetMat4("u_model", modelMatrix);
                    //glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex);

                    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, renderItem.baseSkinnedVertex);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Calculate mask
            maskShader->Bind();


            glBindImageTexture(0, decalMasksFBO->GetColorAttachmentHandleByName("DecalMask0"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
            glBindImageTexture(1, woundMaskArray->GetHandle(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, decalPaintingFBO->GetColorAttachmentHandleByName("UVMap"));
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("Decal_Wound0")->GetGLTexture().GetHandle());

            glDispatchCompute((decalPaintingFBO->GetWidth() + 7) / 8, (decalPaintingFBO->GetHeight() + 7) / 8, 1);
        }       

        OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");
        if (Input::MiddleMousePressed()) {
            decalMasksFBO->ClearTexImage("DecalMask0", 0, 0, 0, 1);
        }

    }

    glm::mat4 makeDecalProjView(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec2& decalSize, float nearZ, float farZ)  
    {
        // pick an up that isn't collinear with rayDir
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        if (fabs(glm::dot(rayDir, worldUp)) > 0.99f) worldUp = glm::vec3(1, 0, 0);

        glm::mat4 view = glm::lookAt(
            rayOrigin,
            rayOrigin + rayDir,
            worldUp
        );

        float hw = decalSize.x * 0.5f;
        float hh = decalSize.y * 0.5f;
        glm::mat4 proj = glm::ortho(
            -hw, hw,
            -hh, hh,
            nearZ, farZ
        );

        return proj * view;
    }
}