#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Viewport/ViewportManager.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"
#include "Modelling/Clipping.h"
#include "Modelling/Unused/Modelling.h"
#include "World/World.h"

#include "Ragdoll/RagdollManager.h"
#include "Input/Input.h"

namespace OpenGLRenderer {

    void HouseGeometryPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("DebugTextured");

        if (!gBuffer) return;
        if (!shader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        shader->Bind();
        shader->SetMat4("u_model", glm::mat4(1));
        shader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        MeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer();
        OpenGLMeshBuffer& glHouseMeshBuffer = houseMeshBuffer.GetGLMeshBuffer();

        glBindVertexArray(glHouseMeshBuffer.GetVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;
            if (glHouseMeshBuffer.GetIndexCount() <= 0) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader->SetInt("u_viewportIndex", i);

            const std::vector<HouseRenderItem>& renderItems = RenderDataManager::GetHouseRenderItems();

            for (const HouseRenderItem& renderItem : renderItems) {
                int indexCount = renderItem.indexCount;
                int baseVertex = renderItem.baseVertex;
                int baseIndex = renderItem.baseIndex;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);
            }
        }
    }


    void GeometryPass() {
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("GBuffer");
        OpenGLShader* editorMeshShader = GetShader("EditorMesh");
        OpenGLTextureArray* woundMaskArray = GetTextureArray("WoundMasks");

        if (!gBuffer) return;
        if (!shader) return;
        if (!editorMeshShader) return;
        if (!woundMaskArray) return;


        // Test clear
        int clearIndex = -1;
        if (Input::KeyPressed(HELL_KEY_NUMPAD_0)) {
            clearIndex = 0;
        }
        if (Input::KeyPressed(HELL_KEY_NUMPAD_1)) {
            clearIndex = 1;
        }
        if (clearIndex != -1) {
            woundMaskArray->Clear(0, 0, 0, 0, clearIndex);
        }
        if (Input::KeyPressed(HELL_KEY_NUMPAD_2)) {
            ClearAllWoundMasks();
        }
        
        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition" });

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        shader->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        shader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D_ARRAY, woundMaskArray->GetHandle());
  
        // Non blended
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometry[i], true, false);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.geometry[i]);
                }
            }
        }
        // Alpha discard
        shader->SetBool("u_alphaDiscard", true);
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometryAlphaDiscarded[i], true, false);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.geometryAlphaDiscarded[i]);
                }
            }
        }
        shader->SetBool("u_alphaDiscard", false);

        gBuffer->DrawBuffers({ "BaseColor" });
        SetRasterizerState("GeometryPass_Blended");

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometryBlended[i], true, false);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.geometryBlended[i]);
                }
            }
        }

       shader->Bind();
       gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
       SetRasterizerState("GeometryPass_NonBlended");
       
       glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
       glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());
       
       for (int i = 0; i < 4; i++) {
           Viewport* viewport = ViewportManager::GetViewportByIndex(i);
           if (viewport->IsVisible()) {
               OpenGLRenderer::SetViewport(gBuffer, viewport);
               if (BackEnd::RenderDocFound()) {
                   SplitMultiDrawIndirect(shader, drawInfoSet.skinnedGeometry.perViewport[i], true, true);
               }
               else {
                   MultiDrawIndirect(drawInfoSet.skinnedGeometry.perViewport[i]);
               }
           }
       }

       OpenGLShader* christmasLightWireShader = GetShader("ChristmasLightsWire");
       christmasLightWireShader->Bind();
       SetRasterizerState("GeometryPass_NonBlended");

       for (int i = 0; i < 4; i++) {
           Viewport* viewport = ViewportManager::GetViewportByIndex(i);
           if (viewport->IsVisible()) {
               OpenGLRenderer::SetViewport(gBuffer, viewport);

               christmasLightWireShader->SetInt("playerIndex", i);
               christmasLightWireShader->SetMat4("projection", viewportData[i].projection);
               christmasLightWireShader->SetMat4("view", viewportData[i].view);

               // Draw Christmas lights
               for (ChristmasLights& lights : World::GetChristmasLights()) {
                   auto& mesh = lights.m_meshBuffer.GetGLMeshBuffer();
                   glBindVertexArray(mesh.GetVAO());
                   glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
               }

               // Draw power pole wires
               for (PowerPoleSet& powerPoleSet : World::GetPowerPoleSets()) {
                   std::vector<Wire>& wires = powerPoleSet.GetWires();
                   for (Wire& wire : wires) {
                       MeshBuffer& meshBuffer = wire.GetMeshBuffer();
                       OpenGLMeshBuffer& glMeshBuffer = meshBuffer.GetGLMeshBuffer();
                       glBindVertexArray(glMeshBuffer.GetVAO());
                       glDrawElements(GL_TRIANGLES, glMeshBuffer.GetIndexCount(), GL_UNSIGNED_INT, 0);
                   }
               }
           }
       }

       OpenGLShader* ragdollShader = GetShader("DebugRagdoll");
       ragdollShader->Bind();
       SetRasterizerState("GeometryPass_NonBlended");

       for (int i = 0; i < 4; i++) {
           Viewport* viewport = ViewportManager::GetViewportByIndex(i);
           if (viewport->IsVisible()) {
               OpenGLRenderer::SetViewport(gBuffer, viewport);

               ragdollShader->SetInt("u_playerIndex", i);
               ragdollShader->SetMat4("u_projectionView", viewportData[i].projectionView);
               ragdollShader->SetMat4("u_projection", viewportData[i].projection);
               ragdollShader->SetMat4("u_view", viewportData[i].view);

               // Ragdoll
               auto& ragdolls = RagdollManager::GetRagdolls();

               for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
                   RagdollV2& ragdoll = it->second;

                   if (ragdoll.RenderingEnabled()) {
                       MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
                       glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());

                       for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
                           if (meshBuffer.GetIndices().size() == 0) continue;

                           Mesh* mesh = meshBuffer.GetMeshByIndex(j);
                           glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
                           ragdollShader->SetMat4("u_model", modelMatrix);
                           ragdollShader->SetVec3("u_color", ragdoll.GetMarkerColorByRigidIndex(j));

                           glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                       }
                   }
                   it++;
               }
           }
       }

       glBindVertexArray(0);  
    }
}

