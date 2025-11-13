#include "API/OpenGL/Renderer/GL_Renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

#include "Input/Input.h"

namespace OpenGLRenderer {

    void InitDecalCube();
    void MaskPass();
    void CompositePass();
    void DrawDecals();

    GLuint g_cubeVao = 0;

    void ScreenSpaceDecalsPass() {
        if (g_cubeVao == 0) InitDecalCube();
        
        DrawDecals();
        CompositePass();
    }

    void InitDecalCube() {
        float vertices[] = {
             0.5f,  0.5f,  0.5f,  
             0.5f,  0.5f, -0.5f, 
            -0.5f,  0.5f, -0.5f, 
            -0.5f,  0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,  
             0.5f, -0.5f, -0.5f, 
            -0.5f, -0.5f, -0.5f, 
            -0.5f, -0.5f,  0.5f
        };

        unsigned int indices[] = {
            0, 1, 2,  2, 3, 0, // Top
            1, 5, 6,  6, 2, 1, // Back
            7, 4, 0,  0, 3, 7, // Front
            3, 2, 6,  6, 7, 3, // Left
            5, 1, 0,  0, 4, 5, // Right
            6, 5, 4,  4, 7, 6  // Bottom
        };

        unsigned int VBO, EBO;
        glGenVertexArrays(1, &g_cubeVao);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(g_cubeVao);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void MaskPass() {
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("BloodScreenSpaceDecalsMask");

        if (!miscFullSizeFBO) return;
        if (!gBuffer) return;
        if (!shader) return;

        miscFullSizeFBO->Bind();
        miscFullSizeFBO->DrawBuffers({ "ScreenSpaceBloodDecalMask" });

        SetRasterizerState("GeometryPass_Default");

        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("Normal"));

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(g_cubeVao);
        glBindTextureUnit(2, AssetManager::GetTextureByName("BloodDecal4")->GetGLTexture().GetHandle());
        glBindTextureUnit(3, AssetManager::GetTextureByName("BloodDecal6")->GetGLTexture().GetHandle());
        glBindTextureUnit(4, AssetManager::GetTextureByName("BloodDecal7")->GetGLTexture().GetHandle());
        glBindTextureUnit(5, AssetManager::GetTextureByName("BloodDecal9")->GetGLTexture().GetHandle());

        glEnable(GL_BLEND);
        glBlendEquation(GL_MAX);
        glBlendFunc(GL_ONE, GL_ONE);

        shader->Bind();

        int instanceCount = RenderDataManager::GetScreenSpaceBloodDecalInstanceData().size();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);

            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);

                shader->SetMat4("u_projectionView", viewportData[i].projectionView);
                glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
            }
        }    

        // Clean up
        glBlendEquation(GL_FUNC_ADD);
    }

    void CompositePass() {
        OpenGLShader* shader = GetShader("BloodScreenSpaceDecalsComposite");
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        if (!miscFullSizeFBO) return;
        if (!shader) return;
        if (!gBuffer) return;

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        shader->Bind();

        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("BaseColor"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glBindImageTexture(1, gBuffer->GetColorAttachmentHandleByName("RMA"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glBindTextureUnit(2, miscFullSizeFBO->GetColorAttachmentHandleByName("ScreenSpaceBloodDecalMask"));
        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }

    // This was a compute pass, but you rewrote it to be a vert/frag quad style screenspace pass for the hardware mipmapping
    // This was a compute pass, but you rewrote it to be a vert/frag quad style screenspace pass for the hardware mipmapping
    // This was a compute pass, but you rewrote it to be a vert/frag quad style screenspace pass for the hardware mipmapping

    void DrawDecals() {
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");
        OpenGLShader* shader = GetShader("ScreenSpaceDecals");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        if (!miscFullSizeFBO) return;
        if (!shader) return;
        if (!gBuffer) return;

        miscFullSizeFBO->Bind();
        miscFullSizeFBO->SetViewport();
        miscFullSizeFBO->DrawBuffers({ "ScreenSpaceBloodDecalMask" });

        shader->Bind();
        shader->SetInt("u_decalCount", RenderDataManager::GetScreenSpaceBloodDecalInstanceData().size());

        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("RMA"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        glBindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("Normal"));

        if (BackEnd::RenderDocFound()) {
            glBindTextureUnit(3, AssetManager::GetTextureByName("BloodDecal4")->GetGLTexture().GetHandle());
            glBindTextureUnit(4, AssetManager::GetTextureByName("BloodDecal6")->GetGLTexture().GetHandle());
            glBindTextureUnit(5, AssetManager::GetTextureByName("BloodDecal7")->GetGLTexture().GetHandle());
            glBindTextureUnit(6, AssetManager::GetTextureByName("BloodDecal9")->GetGLTexture().GetHandle());
        }

        // Draw full screen triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

}