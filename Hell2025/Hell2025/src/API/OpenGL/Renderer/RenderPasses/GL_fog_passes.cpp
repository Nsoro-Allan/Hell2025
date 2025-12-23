#include "HellLogging.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Core/Game.h"

namespace OpenGLRenderer {

    void InitFog() {
        OpenGLShader* shader = GetShader("PerlinNoise3D");
        OpenGLTexture3D* perlinNoiseTexture = GetTexture3D("PerlinNoise");

        if (!shader) return;
        if (!perlinNoiseTexture) return;

        glBindImageTexture(0, perlinNoiseTexture->GetHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

        int size = perlinNoiseTexture->GetSize();

        shader->Bind();
        shader->SetFloat("uScale", 8.0f);
        shader->SetVec3("uDimensions", glm::vec3(size));
        
        glDispatchCompute((size + 7) / 8, (size + 7) / 8, (size + 7) / 8);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        Logging::Init() << "Initialized the BERLIN NOISE";
    }

    void RayMarchFog() {
        ProfilerOpenGLZoneFunction();

        if (Editor::IsOpen()) return;

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* rayMarchShader = GetShader("FogRayMarch");
        OpenGLShader* compositeShader = GetShader("FogComposite");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* fogFbo = GetFrameBuffer("Fog");
        OpenGLTexture3D* perlinNoiseTexture = GetTexture3D("PerlinNoise");
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");

        if (!miscFullSizeFBO) return;
        if (!rayMarchShader) return;
        if (!compositeShader) return;
        if (!gBuffer) return;
        if (!fogFbo) return;
        if (!perlinNoiseTexture) return;

        // ATTENTIOMN!!! this only works for 1 player. FIX IT IMMEIDATELY!!
        // ATTENTIOMN!!! this only works for 1 player. FIX IT IMMEIDATELY!!
        // ATTENTIOMN!!! this only works for 1 player. FIX IT IMMEIDATELY!!
        // ATTENTIOMN!!! this only works for 1 player. FIX IT IMMEIDATELY!!
        
        glm::mat4 projection = viewportData[0].projection;
        glm::mat4 view = viewportData[0].view;
        glm::mat4 invViewProj = glm::inverse(projection * view);
        glm::vec3 viewPos = viewportData[0].viewPos;

        static float time = 0.0f;
        time += 1.0f / 60.0f;

        static int noiseSeed = 0;
        noiseSeed++;

        rayMarchShader->Bind();
        rayMarchShader->SetVec3("uCameraPos", viewPos);
        rayMarchShader->SetMat4("uInvViewProj", invViewProj);
        rayMarchShader->SetFloat("u_time", time);
        rayMarchShader->SetInt("u_noiseSeed", noiseSeed);

        SplitscreenMode splitscreenMode = Game::GetSplitscreenMode();
        bool isSplitscreen = splitscreenMode == SplitscreenMode::TWO_PLAYER;
        rayMarchShader->SetInt("u_isSplitscreen", isSplitscreen);
               

        glBindImageTexture(4, fogFbo->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthAttachmentHandle());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, perlinNoiseTexture->GetHandle());


        glBindTextureUnit(3, miscFullSizeFBO->GetColorAttachmentHandleByName("ViewportIndex"));
        //glBindTextureUnit(3, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));

        glDispatchCompute((fogFbo->GetWidth() + 15) / 16, (fogFbo->GetHeight() + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        // Composite
        compositeShader->Bind();
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fogFbo->GetColorAttachmentHandleByName("Color"));
        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
        
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }


    void BlitFog() {
        OpenGLTexture3D* perlinNoiseTexture = GetTexture3D("PerlinNoise");
        if (!perlinNoiseTexture) return;

        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

        static int z = 0;
        z = (z + 1) % 128;
        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, perlinNoiseTexture->GetHandle(), 0, z);

        GLenum status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "FBO incomplete: " << status << "\n";
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        int sliceSize = 128;

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_BACK);

        glBlitFramebuffer(
            0, 0, sliceSize, sliceSize,
            0, 0, sliceSize * 4, sliceSize * 4,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    }



}