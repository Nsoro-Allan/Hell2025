#include "API/OpenGL/Renderer/GL_renderer.h"
#include "World/World.h"

namespace OpenGLRenderer {

    void BlitRoads() {
        OpenGLFrameBuffer* roadFramebuffer = GetFrameBuffer("Road");
        OpenGLShader* shader = GetShader("BlitRoad");

        if (!roadFramebuffer) return;
        if (!shader) return;

        if (World::GetRoads().empty()) return;

        roadFramebuffer->ClearTexImage("RoadMask", 0.0f, 0.0f, 0.0f, 1.0f);

        Road& road = World::GetRoads()[0];

        std::vector<glm::vec4> controlPoints;
        for (glm::vec3 point : road.m_worldPoints) {
            controlPoints.push_back(glm::vec4(point, 1.0f));
        }

        if (controlPoints.size() < 2) return;

        static GLuint roadPointsWorldSsbo = 0;
        if (roadPointsWorldSsbo == 0) glGenBuffers(1, &roadPointsWorldSsbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, roadPointsWorldSsbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, controlPoints.size() * sizeof(glm::vec4), controlPoints.data(), GL_DYNAMIC_DRAW);

        GLuint roadMapTextureHandle = roadFramebuffer->GetColorAttachmentHandleByName("RoadMask");
        glBindImageTexture(0, roadMapTextureHandle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);

        int textureWidthInPixels = roadFramebuffer->GetWidth();
        int textureHeightInPixels = roadFramebuffer->GetHeight();

        glm::vec2 u_worldSpanXZ = glm::vec2(World::GetWorldSpaceWidth(), World::GetWorldSpaceDepth());

        float u_roadWidthInMeters = 4.3f;
        float u_roadEdgeFeatherInMeters = 0.3f;
        float u_falloffExponent = 1.0f;
        
        shader->Bind();
        shader->SetInt("u_numberOfControlPoints", (int)controlPoints.size());
        shader->SetIVec2("u_textureSizeInPixels", { textureWidthInPixels, textureHeightInPixels });
        shader->SetVec2("u_worldSpanXZ", u_worldSpanXZ);
        shader->SetFloat("u_roadWidthInMeters", u_roadWidthInMeters);
        shader->SetFloat("u_roadEdgeFeatherInMeters", u_roadEdgeFeatherInMeters);
        shader->SetFloat("u_falloffExponent", u_falloffExponent);

        GLuint groupCountX = (textureWidthInPixels + 8 - 1) / 8;
        GLuint groupCountY = (textureHeightInPixels + 8 - 1) / 8;
        glDispatchCompute(groupCountX, groupCountY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}