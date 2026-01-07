#include "../GL_renderer.h"
#include "Audio/Audio.h"
#include "Input/Input.h"

namespace OpenGLRenderer {
    void ScreenspaceReflectionsPass() {
        ProfilerOpenGLZoneFunction();

        static bool doMe = true;
        if (Input::KeyPressed(HELL_KEY_PERIOD)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            doMe = !doMe;
        }
        if (!doMe) return;

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* halfSizeFbo = GetFrameBuffer("HalfSize");
        OpenGLFrameBuffer* fullSizeFBO = GetFrameBuffer("MiscFullSize");
        OpenGLShader* shader = GetShader("ScreenspaceReflections");

        if (!gBuffer) return;
        if (!shader) return;
        if (!halfSizeFbo) return;
        if (!fullSizeFBO) return;

        // Down sample
        BlitFrameBuffer(gBuffer, halfSizeFbo, "FinalLighting", "DownsampledFinalLighting", GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);

        // Generate mips
        glGenerateTextureMipmap(halfSizeFbo->GetColorAttachmentHandleByName("DownsampledFinalLighting"));
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

        shader->Bind();
        shader->BindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), GL_READ_WRITE, GL_RGBA16F);
        shader->BindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("BaseColor"));
        shader->BindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("Normal"));
        shader->BindTextureUnit(3, gBuffer->GetColorAttachmentHandleByName("RMA"));
        shader->BindTextureUnit(4, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        shader->BindTextureUnit(5, halfSizeFbo->GetColorAttachmentHandleByName("DownsampledFinalLighting"));
        shader->BindTextureUnit(6, fullSizeFBO->GetColorAttachmentHandleByName("ViewspaceDepth"));

        // Screenspace Reflections
        glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
    }
}