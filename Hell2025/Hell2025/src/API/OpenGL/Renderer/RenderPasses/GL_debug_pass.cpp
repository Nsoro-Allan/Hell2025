#include "../GL_renderer.h" 
#include "Editor/Editor.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Physics/Physics.h"
#include "World/World.h"

#include "API/OpenGL/Types/GL_mesh.h"
#include "API/OpenGL/Types/GL_mesh_buffer.h"

#include "Pathfinding/AStarMap.h"

#include "Core/Game.h"

#include "Input/Input.h"

namespace OpenGLRenderer {
    OpenGLMesh g_debugMeshPoints2D;
    OpenGLMesh g_debugMeshPoints3D;
    OpenGLMesh g_debugMeshLines2D;
    OpenGLMesh g_debugMeshLines3D;

    inline int Index1D(int x, int y, int mapWidth) { return y * mapWidth + x; }

    void RenderAStarDebugMesh();

    void DebugPass() {
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* shader2D = GetShader("DebugVertex2D");
        OpenGLShader* shader3D = GetShader("DebugVertex3D");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        if (!gBuffer) return;
        if (!shader2D) return;
        if (!shader3D) return;

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glPointSize(8.0f);

        UpdateDebugMesh();

        // 3D
        shader3D->Bind();
        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader3D->SetInt("u_viewportIndex", i);
            shader3D->SetMat4("u_projectionView", viewportData[i].projectionView);

            if (g_debugMeshLines3D.GetVertexCount() > 0) {
                glBindVertexArray(g_debugMeshLines3D.GetVAO());
                glDrawArrays(GL_LINES, 0, g_debugMeshLines3D.GetVertexCount());
            }

            if (g_debugMeshPoints3D.GetVertexCount() > 0) {
                glBindVertexArray(g_debugMeshPoints3D.GetVAO());
                glDrawArrays(GL_POINTS, 0, g_debugMeshPoints3D.GetVertexCount());
            }
        }

        if (Debug::GetDebugRenderMode() == DebugRenderMode::ASTAR_MAP) {
            RenderAStarDebugMesh();
        }

        // 2D
        gBuffer->SetViewport();
        shader2D->Bind();
        shader2D->SetInt("u_viewportWidth", gBuffer->GetWidth());
        shader2D->SetInt("u_viewportHeight", gBuffer->GetHeight());

        if (g_debugMeshLines2D.GetVertexCount() > 0) {
            glBindVertexArray(g_debugMeshLines2D.GetVAO());
            glDrawArrays(GL_LINES, 0, g_debugMeshLines2D.GetVertexCount());
        }

        if (g_debugMeshPoints2D.GetVertexCount() > 0) {
            glBindVertexArray(g_debugMeshPoints2D.GetVAO());
            glDrawArrays(GL_POINTS, 0, g_debugMeshPoints2D.GetVertexCount());
        }
    }

    void RenderAStarDebugMesh() {
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLMeshBuffer& debugGridMesh = AStarMap::GetDebugGridMeshBuffer().GetGLMeshBuffer();
        OpenGLMeshBuffer& debugSolidMesh = AStarMap::GetDebugSolidMeshBuffer().GetGLMeshBuffer();

        OpenGLShader* solidColorShader = GetShader("DebugSolidColor");
        if (!solidColorShader) return;

        solidColorShader->Bind();
        solidColorShader->SetMat4("u_model", glm::mat4(1));
        solidColorShader->SetVec3("u_color", WHITE);

        // Line mesh
        if (debugGridMesh.GetIndexCount() > 0) {
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(debugGridMesh.GetVAO());
            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (!viewport->IsVisible()) continue;

                solidColorShader->SetMat4("u_projectionView", viewportData[i].projectionView);
                glDrawElements(GL_LINES, debugGridMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
        }

        int mapWidth = AStarMap::GetMapWidth();
        int mapHeight = AStarMap::GetMapHeight();

        std::vector<glm::ivec2> walls = AStarMap::GetWallCells();
        std::vector<glm::ivec2> path;

        if (World::GetKangaroos().size()) {
            Kangaroo& kangaroo = World::GetKangaroos()[0];
            path = kangaroo.GetPath();
        }

        // Render player cell
        //Player* player = Game::GetLocalPlayerByIndex(0);;
        //glm::vec3 position = player->GetCameraPosition();
        //glm::ivec2 playerCoords = AStarMap::GetCellCoordsFromWorldSpacePosition(position);
        //walls.push_back(playerCoords);
        //
        //// Render roo cell
        //Kangaroo& kangaroo = World::GetKangaroos()[0];
        //walls.push_back(kangaroo.GetGridPosition());



        // Solid mesh
        if (debugSolidMesh.GetIndexCount() > 0) {
            //glEnable(GL_DEPTH_TEST);
            glBindVertexArray(debugSolidMesh.GetVAO());

            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (!viewport->IsVisible()) continue;

                solidColorShader->SetMat4("u_projectionView", viewportData[i].projectionView);
                //glDrawElements(GL_TRIANGLES, debugSolidMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
                //glDrawElements(GL_POINTS, debugSolidMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);

                solidColorShader->SetBool("u_isPath", true);

                for (glm::ivec2& cell : path) {
                    int cellX = cell.x;
                    int cellY = cell.y;
                    int baseVertex = Index1D(cellX, cellY, mapWidth + 1);
                    int cellID = cellY * mapWidth + cellX;
                    int baseIndex = cellID * 6;
                    void* offset = (void*)(baseIndex * sizeof(uint32_t));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset); 
                }

                solidColorShader->SetBool("u_isPath", false);
                for (glm::ivec2& cell : walls) {
                    int cellX = cell.x;
                    int cellY = cell.y;
                    int baseVertex = Index1D(cellX, cellY, mapWidth + 1);
                    int cellID = cellY * mapWidth + cellX;
                    int baseIndex = cellID * 6;
                    void* offset = (void*)(baseIndex * sizeof(uint32_t));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset);
                }
            }
        }
    }

    void DebugViewPass() {
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");

        if (!miscFullSizeFBO) return;
        if (!gBuffer) return;

        // Tile based deferred heat map
        if (rendererSettings.rendererOverrideState == RendererOverrideState::TILE_HEATMAP) {

            OpenGLShader* shader = GetShader("DebugTileView");
            if (!shader) return;

            shader->Bind();
            shader->SetFloat("u_viewportWidth", gBuffer->GetWidth());
            shader->SetFloat("u_viewportHeight", gBuffer->GetHeight());
            shader->SetInt("u_tileXCount", gBuffer->GetWidth() / TILE_SIZE);
            shader->SetInt("u_tileYCount", gBuffer->GetHeight() / TILE_SIZE);

            glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindTextureUnit(1, gBuffer->GetDepthAttachmentHandle());
            glBindTextureUnit(2, miscFullSizeFBO->GetColorAttachmentHandleByName("ViewportIndex"));

            glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
        }

        // Other modes
        if (rendererSettings.rendererOverrideState == RendererOverrideState::BASE_COLOR ||
            rendererSettings.rendererOverrideState == RendererOverrideState::NORMALS ||
            rendererSettings.rendererOverrideState == RendererOverrideState::RMA ||
            rendererSettings.rendererOverrideState == RendererOverrideState::METALIC ||
            rendererSettings.rendererOverrideState == RendererOverrideState::AO ||
            rendererSettings.rendererOverrideState == RendererOverrideState::CAMERA_NDOTL ||
            rendererSettings.rendererOverrideState == RendererOverrideState::ROUGHNESS ) {

            OpenGLShader* shader = GetShader("DebugView");
            if (!shader) return;

            shader->Bind();
            shader->SetFloat("u_brushSize", Editor::GetMapHeightBrushSize());
            shader->SetBool("u_heightMapEditor", (Editor::GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) && Editor::IsOpen());
            glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

            glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("BaseColor"));
            glBindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("Normal"));
            glBindTextureUnit(3, gBuffer->GetColorAttachmentHandleByName("RMA"));
            glBindTextureUnit(4, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
            glBindTextureUnit(5, miscFullSizeFBO->GetColorAttachmentHandleByName("ViewportIndex"));
            glBindTextureUnit(7, gBuffer->GetColorAttachmentHandleByName("Emissive"));

            glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
        }
    }

    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color, bool depthEnabled, int exclusiveViewportIndex, int ignoredViewportIndex) {
        DebugVertex3D v0 = DebugVertex3D(begin, color, glm::ivec2(0, 0), int(depthEnabled), exclusiveViewportIndex);
        DebugVertex3D v1 = DebugVertex3D(end, color, glm::ivec2(0, 0), int(depthEnabled), exclusiveViewportIndex);
        g_lines3D.push_back(v0);
        g_lines3D.push_back(v1);
    }

    void DrawLine2D(const glm::ivec2& begin, const glm::ivec2& end, const glm::vec3& color) {
        g_lines2D.emplace_back(DebugVertex2D(begin, color));
        g_lines2D.emplace_back(DebugVertex2D(end, color));
    }

    void DrawPoint(glm::vec3 position, glm::vec3 color, bool depthEnabled, int exclusiveViewportIndex) {
        g_points3D.push_back(DebugVertex3D(position, color, glm::ivec2(0, 0), int(depthEnabled), exclusiveViewportIndex));
    }

    void DrawPoint2D(const glm::ivec2& position, const glm::vec3& color) {
        g_points2D.emplace_back(DebugVertex2D(position, color));
    }

    void DrawAABB(const AABB& aabb, const glm::vec3& color) {
        glm::vec3 FrontTopLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontTopRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontBottomLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontBottomRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z);
        glm::vec3 BackTopLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z);
        glm::vec3 BackTopRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z);
        glm::vec3 BackBottomLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z);
        glm::vec3 BackBottomRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z);
        DrawLine(FrontTopLeft, FrontTopRight, color);
        DrawLine(FrontBottomLeft, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackTopRight, color);
        DrawLine(BackBottomLeft, BackBottomRight, color);
        DrawLine(FrontTopLeft, FrontBottomLeft, color);
        DrawLine(FrontTopRight, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackBottomLeft, color);
        DrawLine(BackTopRight, BackBottomRight, color);
        DrawLine(FrontTopLeft, BackTopLeft, color);
        DrawLine(FrontTopRight, BackTopRight, color);
        DrawLine(FrontBottomLeft, BackBottomLeft, color);
        DrawLine(FrontBottomRight, BackBottomRight, color);
    }

    void DrawAABB(const AABB& aabb, const glm::vec3& color, const glm::mat4& worldTransform) {
        glm::vec3 FrontTopLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontTopRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontBottomLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontBottomRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 BackTopLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackTopRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackBottomLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackBottomRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z, 1.0f);
        DrawLine(FrontTopLeft, FrontTopRight, color);
        DrawLine(FrontBottomLeft, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackTopRight, color);
        DrawLine(BackBottomLeft, BackBottomRight, color);
        DrawLine(FrontTopLeft, FrontBottomLeft, color);
        DrawLine(FrontTopRight, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackBottomLeft, color);
        DrawLine(BackTopRight, BackBottomRight, color);
        DrawLine(FrontTopLeft, BackTopLeft, color);
        DrawLine(FrontTopRight, BackTopRight, color);
        DrawLine(FrontBottomLeft, BackBottomLeft, color);
        DrawLine(FrontBottomRight, BackBottomRight, color);
    }

    void DrawFrustum(const Frustum& frustum, const glm::vec3& color) {
        glm::vec3 ntl = frustum.GetCorner(0);
        glm::vec3 ntr = frustum.GetCorner(1);
        glm::vec3 nbl = frustum.GetCorner(2);
        glm::vec3 nbr = frustum.GetCorner(3);

        glm::vec3 ftl = frustum.GetCorner(4);
        glm::vec3 ftr = frustum.GetCorner(5);
        glm::vec3 fbl = frustum.GetCorner(6);
        glm::vec3 fbr = frustum.GetCorner(7);

        // near face
        DrawLine(ntl, ntr, color);
        DrawLine(ntr, nbr, color);
        DrawLine(nbr, nbl, color);
        DrawLine(nbl, ntl, color);

        // far face
        DrawLine(ftl, ftr, color);
        DrawLine(ftr, fbr, color);
        DrawLine(fbr, fbl, color);
        DrawLine(fbl, ftl, color);

        // connect near to far
        DrawLine(ntl, ftl, color);
        DrawLine(ntr, ftr, color);
        DrawLine(nbl, fbl, color);
        DrawLine(nbr, fbr, color);
    }

    void DrawCircle(const glm::vec3 center, float radius, const glm::vec3 axisU, const glm::vec3 axisV, int segments, const glm::vec3 color) {
        const float step = glm::two_pi<float>() / float(segments);
        glm::vec3 prev = center + radius * axisU;
        for (int i = 1; i <= segments; ++i) {
            float a = step * float(i);
            glm::vec3 p = center + radius * (axisU * cos(a) + axisV * sin(a));
            DrawLine(prev, p, color);
            prev = p;
        }
    }

    void DrawSphere(glm::vec3 position, float radius, glm::vec3 color) {
        const int numLongitudes = 12;
        const int numLatitudes = 8;
        const int segsPerCircle = 96;

        const glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::vec3(1, 0, 0);
        glm::vec3 axisA = glm::normalize(glm::cross(up, right));
        glm::vec3 axisB = glm::normalize(glm::cross(up, axisA));

        // Longitudes
        for (int m = 0; m < numLongitudes; ++m) {
            float theta = glm::two_pi<float>() * float(m) / float(numLongitudes);
            glm::vec3 dir = axisA * cos(theta) + axisB * sin(theta);
            DrawCircle(position, radius, up, dir, segsPerCircle, color);
        }

        // Latitudes
        for (int j = 1; j < numLatitudes; ++j) {
            float t = float(j) / float(numLatitudes);
            float phi = glm::pi<float>() * (t - 0.5f);
            float z = radius * sin(phi);
            float r = radius * cos(phi);
            DrawCircle(position + up * z, r, axisA, axisB, segsPerCircle, color);
        }
    }

    void UpdateDebugMesh() {
        g_debugMeshLines2D.UpdateVertexData(g_lines2D);
        g_debugMeshLines3D.UpdateVertexData(g_lines3D);
        g_debugMeshPoints2D.UpdateVertexData(g_points2D);
        g_debugMeshPoints3D.UpdateVertexData(g_points3D);

        g_lines3D.clear();
        g_lines2D.clear();
        g_points2D.clear();
        g_points3D.clear();
    }

    void RenderPointCloud() {

    }
}
