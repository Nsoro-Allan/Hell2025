#include "../GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/GlobalIllumination.h"
#include "World/World.h"
#include "Ocean/Ocean.h"

namespace OpenGLRenderer {

    void LightCullingPass() {
        ProfilerOpenGLZoneFunction();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("LightCulling");

        if (!gBuffer) return;
        if (!shader) return;

        size_t lightCount = World::GetLights().size();

        shader->Bind();
        shader->SetFloat("u_viewportWidth", gBuffer->GetWidth());
        shader->SetFloat("u_viewportHeight", gBuffer->GetHeight());
        shader->SetInt("u_lightCount", lightCount);
        shader->SetInt("u_tileXCount", gBuffer->GetWidth() / TILE_SIZE);
        shader->SetInt("u_tileYCount", gBuffer->GetHeight() / TILE_SIZE);
                
        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("Normal"));

        glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
    }

    void ComputeViewspaceDepth() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* fullSizeFBO = GetFrameBuffer("MiscFullSize");
        OpenGLShader* shader = GetShader("ViewspaceDepth");

        if (!gBuffer) return;
        if (!fullSizeFBO) return;
        if (!shader) return;

        shader->Bind();
        shader->BindImageTexture(0, fullSizeFBO->GetColorAttachmentHandleByName("ViewspaceDepth"), GL_WRITE_ONLY, GL_R32F);
        shader->BindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        shader->BindTextureUnit(2, fullSizeFBO->GetColorAttachmentHandleByName("ViewportIndex"));

        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    void LightingPass() {
        ProfilerOpenGLZoneFunction();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* finalImageFBO = GetFrameBuffer("FinalImage");
        OpenGLShadowMap* flashLightShadowMapsFBO = GetShadowMap("FlashlightShadowMaps");
        OpenGLShadowCubeMapArray* hiResShadowMaps = GetShadowCubeMapArray("HiRes");
        OpenGLShader* lightingShader = GetShader("Lighting");
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");

        if (!gBuffer) return;
        if (!miscFullSizeFBO) return;
        if (!finalImageFBO) return;
        if (!lightingShader) return;

        lightingShader->Bind();

        lightingShader->SetFloat("u_viewportWidth", gBuffer->GetWidth());
        lightingShader->SetFloat("u_viewportHeight", gBuffer->GetHeight());
        lightingShader->SetInt("u_tileXCount", gBuffer->GetWidth() / TILE_SIZE);
        lightingShader->SetInt("u_tileYCount", gBuffer->GetHeight() / TILE_SIZE);

        if (World::HasOcean()) {
            lightingShader->SetFloat("u_oceanHeight", Ocean::GetOceanOriginY());
        }
        else {
            lightingShader->SetFloat("u_oceanHeight", -1000);
        }

        // Warning this CSM shit is p1 only atm, especially cause of hardcoded FULL SCREEN viewport dimensions

        float viewportWidth = gBuffer->GetWidth();
        float viewportHeight = gBuffer->GetHeight();
        std::vector<float>& cascadeLevels = GetShadowCascadeLevels();

        lightingShader->SetVec3("u_moonlightDir", Game::GetMoonlightDirection());
        lightingShader->SetFloat("farPlane", FAR_PLANE);
        lightingShader->SetVec2("u_viewportSize", glm::vec2(viewportWidth, viewportHeight));
        lightingShader->SetInt("cascadeCount", cascadeLevels.size() + 1);
        for (size_t i = 0; i < cascadeLevels.size(); ++i) {
            lightingShader->SetFloat("u_cascadePlaneDistances[" + std::to_string(i) + "]", cascadeLevels[i]);
        }

        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("BaseColor"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("Normal"));
        glBindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("RMA"));
        glBindTextureUnit(3, gBuffer->GetDepthAttachmentHandle());
        glBindTextureUnit(4, gBuffer->GetColorAttachmentHandleByName("WorldPosition"));
        glBindTextureUnit(5, miscFullSizeFBO->GetColorAttachmentHandleByName("ViewportIndex"));
        glBindTextureUnit(6, gBuffer->GetColorAttachmentHandleByName("Emissive"));
        glBindTextureUnit(7, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        glBindTextureUnit(8, flashLightShadowMapsFBO->GetDepthTextureHandle());
        //glBindTextureUnit(9, hiResShadowMaps->GetDepthTexture());

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, hiResShadowMaps->GetDepthTexture());


        std::vector<LightVolume>& lightVolumes = GlobalIllumination::GetLightVolumes();
        if (lightVolumes.size()) {
            LightVolume& lightVolume = lightVolumes[0];

            lightingShader->SetFloat("u_lightVolumeSpacing", GlobalIllumination::GetProbeSpacing());
            lightingShader->SetVec3("u_lightVolumeOffset", lightVolume.m_offset);
            lightingShader->SetVec3("u_lightVolumeWorldSize", glm::vec3(lightVolume.m_worldSpaceWidth, lightVolume.m_worldSpaceHeight, lightVolume.m_worldSpaceDepth));


            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_3D, lightVolume.GetLightingTextureHandle());

        }


        OpenGLShadowMapArray* shadowMapArray = GetShadowMapArray("MoonlightCSM");

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray->GetDepthTexture());

        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

        //glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);

        glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
    }

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

        glDispatchCompute(gBuffer->GetWidth() / TILE_SIZE, gBuffer->GetHeight() / TILE_SIZE, 1);
    }
}