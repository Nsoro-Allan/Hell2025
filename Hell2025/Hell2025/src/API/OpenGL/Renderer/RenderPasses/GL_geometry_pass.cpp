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
#include "HellLogging.h"
#include "Physics/Physics.h"

#include "Types/Mirror.h"
#include "Managers/MirrorManager.h"

namespace OpenGLRenderer {

    void HouseGeometryPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("DebugTextured");

        if (!gBuffer) return;
        if (!shader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

        shader->Bind();
        shader->SetMat4("u_model", glm::mat4(1));
        shader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        MeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer();
        OpenGLMeshBuffer& glHouseMeshBuffer = houseMeshBuffer.GetGLMeshBuffer();

        glBindVertexArray(glHouseMeshBuffer.GetVAO());

        // ATTENTION! You are not frustum culling your house mesh bro
        // ATTENTION! You are not frustum culling your house mesh bro
        // ATTENTION! You are not frustum culling your house mesh bro

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
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("GBuffer");
        OpenGLShader* editorMeshShader = GetShader("EditorMesh");
        OpenGLTextureArray* woundMaskArray = GetTextureArray("WoundMasks");

        if (!gBuffer) return;
        if (!shader) return;
        if (!editorMeshShader) return;
        if (!woundMaskArray) return;

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D_ARRAY, woundMaskArray->GetHandle());

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });

        shader->Bind();
        shader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");

        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

        // Default (Non blended)
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
        SetRasterizerState("GeometryPass_AlphaDiscard"); 
        EditorRasterizerStateOverride();

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

        // Blended
        shader->SetBool("u_alphaDiscard", false);
        gBuffer->DrawBuffers({ "BaseColor" });
        SetRasterizerState("GeometryPass_Blended");
        EditorRasterizerStateOverride();

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

        // Skinned mesh
        shader->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });
        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

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
        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

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
        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

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

    void MirrorGeometryPass() {
        const RenderItem& mirrorRenderItem = RenderDataManager::GetMirrorRenderItems()[0];
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* gBufferBackup = GetFrameBuffer("GBufferBackup");
        OpenGLShader* geometryShader = GetShader("GBuffer");
        OpenGLShader* houseGeometryShader = GetShader("DebugTextured");
        OpenGLShader* solidColorShader = GetShader("DebugSolidColor");

        if (!gBuffer) return;
        if (!gBufferBackup) return;
        if (!geometryShader) return;
        if (!houseGeometryShader) return;
        if (!solidColorShader) return;

        // Render the mirror mask
        // - First you copy the depth buffer from the GBuffer so you can render your mirror plane against scene depth
        // - Then you just do a standard stencil buffer mask write for each viewport
        OpenGLRenderer::BlitFrameBufferDepth(gBuffer, gBufferBackup);

        gBuffer->Bind();
        gBuffer->BindDepthAttachmentFrom(*gBufferBackup);

        solidColorShader->Bind();

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE); // Test depth, but don't write it
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);

                Mirror* mirror = MirrorManager::GetMirrorByObjectId(viewport->GetMirrorId());
                if (!mirror) continue;

                Mesh* mesh = AssetManager::GetMeshByIndex(mirror->GetGlobalMeshIndex());
                if (!mesh) continue;

                solidColorShader->SetMat4("u_projectionView", viewportData[i].projectionView);
                solidColorShader->SetMat4("u_model", mirrorRenderItem.modelMatrix);

                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);
            }
        }

        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });

        // Clear the depth buffer so that the mirror world has a clean depth state to test against
        gBuffer->ClearDepthAttachment();

        SetRasterizerState("GeometryPass_Default");

        glEnable(GL_CLIP_DISTANCE0);
        glFrontFace(GL_CW);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);

        geometryShader->Bind();
        geometryShader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        // Regular geometry
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);

                Mirror* mirror = MirrorManager::GetMirrorByObjectId(viewport->GetMirrorId());
                if (!mirror) continue;

                geometryShader->SetBool("u_useMirrorMatrix", true);
                geometryShader->SetMat4("u_mirrorViewMatrix", mirror->GetViewMatrix(i));
                geometryShader->SetVec4("u_mirrorClipPlane", mirror->GetClipPlane(i));

                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(geometryShader, drawInfoSet.mirrorRenderItems[i], true, false);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.mirrorRenderItems[i]);
                }
            }
        }
        geometryShader->SetBool("u_useMirrorMatrix", false);

        // House geometry
        houseGeometryShader->Bind();
        houseGeometryShader->SetMat4("u_model", glm::mat4(1));
        houseGeometryShader->SetBool("u_flipNormalMapY", ShouldFlipNormalMapY());

        MeshBuffer& houseMeshBuffer = World::GetHouseMeshBuffer();
        OpenGLMeshBuffer& glHouseMeshBuffer = houseMeshBuffer.GetGLMeshBuffer();

        glBindVertexArray(glHouseMeshBuffer.GetVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;
            if (glHouseMeshBuffer.GetIndexCount() <= 0) continue;

            Mirror* mirror = MirrorManager::GetMirrorByObjectId(viewport->GetMirrorId());
            if (!mirror) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            houseGeometryShader->SetInt("u_viewportIndex", i);
            houseGeometryShader->SetBool("u_useMirrorMatrix", true);
            houseGeometryShader->SetMat4("u_mirrorViewMatrix", mirror->GetViewMatrix(i));
            houseGeometryShader->SetVec4("u_mirrorClipPlane", mirror->GetClipPlane(i));

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
        houseGeometryShader->SetBool("u_useMirrorMatrix", false);

        // Clean up
        glStencilMask(0xFF);
        glDisable(GL_CLIP_DISTANCE0);
        glFrontFace(GL_CCW);
        glDisable(GL_STENCIL_TEST);

        gBuffer->BindDepthAttachmentFrom(*gBuffer);
    }
}

