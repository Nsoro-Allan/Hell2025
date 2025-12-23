#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "UI/UIBackEnd.h"

namespace OpenGLRenderer {
    GLint g_quadVAO = 0;
    GLuint g_linearSampler = 0;
    GLuint g_nearestSampler = 0;

    void UIPass() {
        ProfilerOpenGLZoneFunction();

        const Resolutions& resolutions = Config::GetResolutions();

        OpenGLShader* shader = GetShader("UI");
        OpenGLFrameBuffer* uiFrameBuffer = GetFrameBuffer("UI");

        if (!shader) return;
        if (!uiFrameBuffer) return;

        if (g_linearSampler == 0) {
            glGenSamplers(1, &g_linearSampler); 
            glSamplerParameteri(g_linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(g_linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenSamplers(1, &g_nearestSampler); 
            glSamplerParameteri(g_nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glSamplerParameteri(g_nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        Mesh2D& mesh = UIBackEnd::GetUIMesh();
        OpenGLMesh2D& glMesh = mesh.GetGLMesh2D();

        uiFrameBuffer->Bind();
        uiFrameBuffer->SetViewport();
        uiFrameBuffer->ClearAttachment("Color", 0.0f, 0.0f, 0.0f, 1.0f);
        uiFrameBuffer->DrawBuffer("Color");
        shader->Bind();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // vERY IMPORTANT. Preserves alpha accumulation correclty. Required for blit into main image.

        glBindVertexArray(glMesh.GetVAO());

        glEnable(GL_CLIP_DISTANCE0);
        glEnable(GL_CLIP_DISTANCE1);
        glEnable(GL_CLIP_DISTANCE2);
        glEnable(GL_CLIP_DISTANCE3);

        shader->SetFloat("u_renderTargetWidth", resolutions.ui.x);
        shader->SetFloat("u_renderTargetHeight", resolutions.ui.y);
        
        int lastFilter = -1; // -1 = unknown, 0 = linear, 1 = nearest

        for (UIRenderItem& renderItem : UIBackEnd::GetRenderItems()) {

            OpenGLTexture& glTexture = AssetManager::GetTextureByIndex(renderItem.textureIndex)->GetGLTexture();
            glBindTextureUnit(0, glTexture.GetHandle());

            if (renderItem.filter != lastFilter) {
                switch (renderItem.filter) {
                    case 0: glBindSampler(0, g_linearSampler);  break;
                    case 1: glBindSampler(0, g_nearestSampler); break;
                }
                lastFilter = renderItem.filter;
            }

            shader->SetFloat("u_clipMinX", renderItem.clipMinX);
            shader->SetFloat("u_clipMinY", renderItem.clipMinY);
            shader->SetFloat("u_clipMaxX", renderItem.clipMaxX);
            shader->SetFloat("u_clipMaxY", renderItem.clipMaxY);

            glDrawElementsInstancedBaseVertex(GL_TRIANGLES, renderItem.indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * renderItem.baseIndex), 1, renderItem.baseVertex);
        }

        glDisable(GL_CLIP_DISTANCE0);
        glDisable(GL_CLIP_DISTANCE1);
        glDisable(GL_CLIP_DISTANCE2);
        glDisable(GL_CLIP_DISTANCE3);

        // Create QUAD if it don't exist
        if (g_quadVAO == 0) {
            g_quadVAO = OpenGLRenderer::CreateQuadVAO();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glBindSampler(0, g_nearestSampler);
        glBindSampler(0, g_linearSampler);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uiFrameBuffer->GetColorAttachmentHandleByName("Color"));
        glBindVertexArray(g_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Cleanup
        glDisable(GL_BLEND);
        glBindSampler(0, 0);
    }
}