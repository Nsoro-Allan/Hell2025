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
            int baseWidth = woundMaskArray->GetWidth();
            int baseHeight = woundMaskArray->GetHeight();
            const GLubyte black[4] = { 0, 0, 0, 0 };
            int layer = clearIndex;
            int mipmapLevel = 0;
            int w = std::max(1, baseWidth >> mipmapLevel);
            int h = std::max(1, baseHeight >> mipmapLevel);
            glClearTexSubImage(woundMaskArray->GetHandle(), mipmapLevel, 0, 0, layer, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, black);
            std::cout << "Cleared " << clearIndex << "\n";
        }
        

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition" });

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        shader->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, decalMasksFBO->GetColorAttachmentHandleByName("DecalMask0"));
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooBlood_ALB")->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooBlood_NRM")->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooBlood_RMA")->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D_ARRAY, woundMaskArray->GetHandle());
  

        //glActiveTexture(GL_TEXTURE7);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooFlesh_ALB")->GetGLTexture().GetHandle());
        //glActiveTexture(GL_TEXTURE8);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooFlesh_NRM")->GetGLTexture().GetHandle());
        //glActiveTexture(GL_TEXTURE9);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("KangarooFlesh_RMA")->GetGLTexture().GetHandle());

        // Non blended
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometry[i]);
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
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometryAlphaDiscarded[i]);
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
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometryBlended[i]);
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
                   SplitMultiDrawIndirect(shader, drawInfoSet.skinnedGeometry.perViewport[i]);
               }
               else {
                   MultiDrawIndirect(drawInfoSet.skinnedGeometry.perViewport[i]);
               }
           }
       }

        // Render Christmas lights
       //const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

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
               for (RagdollV2& ragdoll : RagdollManager::GetRagdolls()) {
                   MeshBuffer& meshBuffer = ragdoll.GetMeshBuffer();
                   glBindVertexArray(meshBuffer.GetGLMeshBuffer().GetVAO());

                   for (int j = 0; j < meshBuffer.GetMeshCount(); j++) {
                       if (meshBuffer.GetIndices().size() == 0) continue;

                       Mesh* mesh = meshBuffer.GetMeshByIndex(j);
                       glm::mat4 modelMatrix = ragdoll.GetModelMatrixByRigidIndex(j);
                       ragdollShader->SetMat4("u_model", modelMatrix);
                       ragdollShader->SetVec3("u_color", ragdoll.GetMarkerColorByRigidIndex(j));

                      //glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                   }
               }
           }
       }

//
//
//   OpenGLShader* shader2 = GetShader("GBuffer_DEBUG");
//   shader2->Bind();
//   gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
//   SetRasterizerState("GeometryPass_NonBlended");
//
//   glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
//   glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());
//
//   glFinish();
//   for (int i = 0; i < 4; i++) {
//       Viewport* viewport = ViewportManager::GetViewportByIndex(i);
//       if (viewport->IsVisible()) {
//           OpenGLRenderer::SetViewport(gBuffer, viewport);
//
//           const std::vector<RenderItem>& instanceData = RenderDataManager::GetInstanceData();
//
//           auto commands = drawInfoSet.skinnedGeometry.perViewport[i];
//
//           for (const DrawIndexedIndirectCommand& command : commands) {
//               int viewportIndex = command.baseInstance >> VIEWPORT_INDEX_SHIFT;
//               int instanceOffset = command.baseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);
//
//               for (GLuint j = 0; j < command.instanceCount; ++j) {
//                   const RenderItem& renderItem = instanceData[instanceOffset + j];
//
//                   shader2->SetInt("u_viewportIndex", viewportIndex);
//                   shader2->SetInt("u_globalInstanceIndex", instanceOffset + j);
//                   shader2->SetMat4("u_modelMatrix", renderItem.modelMatrix);
//                   shader2->SetMat4("u_inverseModelMatrix", renderItem.inverseModelMatrix);
//
//                   SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(renderItem.meshIndex);
//
//
//
//                   std::cout << "[" << i << "] instance: " << j;
//                   std::cout << " count: " << command.instanceCount << "";
//                   std::cout << " viewportIndex: " << renderItem.exclusiveViewportIndex << "";
//                   std::cout << " meshIdx: " << renderItem.meshIndex << " " << mesh->name << "\n";
//
//
//                   glActiveTexture(GL_TEXTURE0);
//                   glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
//                   glActiveTexture(GL_TEXTURE1);
//                   glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
//                   glActiveTexture(GL_TEXTURE2);
//                   glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
//
//                   glDrawElementsBaseVertex(GL_TRIANGLES, command.indexCount, GL_UNSIGNED_INT, (GLvoid*)(command.firstIndex * sizeof(GLuint)), command.baseVertex);
//               }
//           }
//
//
//           //if (BackEnd::RenderDocFound()) {
//           //    SplitMultiDrawIndirect(shader2, drawInfoSet.skinnedGeometry.perViewport[i]);
//           //}
//           //else {
//           //    MultiDrawIndirect(drawInfoSet.skinnedGeometry.perViewport[i]);
//           //}
//       }
//   }
//
//   std::cout << " \n";

        glBindVertexArray(0);

        
    }
}

