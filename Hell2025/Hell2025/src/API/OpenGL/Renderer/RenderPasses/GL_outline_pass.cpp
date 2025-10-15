#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_BackEnd.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

#include <vector>
#include <set>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>


namespace OpenGLRenderer {

    std::vector<glm::vec2> GenerateOutlineOffsets(int lineThickness = 1) {
        std::vector<glm::vec2> offsets;
        for (int y = -lineThickness; y <= lineThickness; y++) {
            for (int x = -lineThickness; x <= lineThickness; x++) {
                // Only include the outer perimeter of the square ring
                if (abs(x) == lineThickness || abs(y) == lineThickness) {
                    offsets.emplace_back(x, y);
                }
            }
        }
        std::cout << "OLD count: " << offsets.size() << "\n";

        // Spherical approach
        offsets.clear();
        float radius = static_cast<float>(lineThickness);
        float quality = 1.0f;

        int numSamples = static_cast<int>(2 * HELL_PI * radius * quality);
        numSamples = std::max(8, numSamples);

        for (int i = 0; i < numSamples; i++) {
            float angle = (2.0f * HELL_PI * i) / static_cast<float>(numSamples);

            float x =(std::round(cos(angle) * radius));
            float y =(std::round(sin(angle) * radius));

            offsets.emplace_back(x, y);
        }

        std::cout << "NEW count: " << offsets.size() << "\n";

        // Spherical appraoch with removed duplicates
        offsets.clear();
        struct ivec2_less {
            bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
                return std::tie(a.x, a.y) < std::tie(b.x, b.y);
            }
        };
        std::set<glm::ivec2, ivec2_less> unique_integer_offsets;

        for (int i = 0; i < numSamples; i++) {
            float angle = (2.0f * HELL_PI * i) / static_cast<float>(numSamples);

            // Generate points and round them to the nearest integer
            int x = static_cast<int>(std::round(cos(angle) * radius));
            int y = static_cast<int>(std::round(sin(angle) * radius));

            // Insert into the set (duplicates are automatically discarded)
            unique_integer_offsets.insert({ x, y });
        }

        // Create the final vec2 vector for the result
        offsets.reserve(unique_integer_offsets.size());

        // Convert the unique ivec2s from the set back into vec2s
        for (const glm::ivec2& p : unique_integer_offsets) {
            offsets.emplace_back(static_cast<float>(p.x), static_cast<float>(p.y));
        }

        std::cout << "SUPER count: " << offsets.size() << "\n";
        return offsets;
    }

    

    void OutlinePass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* outlineFBO = GetFrameBuffer("Outline");
        OpenGLShader* maskShader = GetShader("OutlineMask");
        OpenGLShader* outlineShader = GetShader("Outline");
        OpenGLShader* compositeShader = GetShader("OutlineComposite");

        // Compute offsets given the outline width
        const int outlineWidth = 3;
        static std::vector<glm::vec2> offsets = GenerateOutlineOffsets(outlineWidth);

        //Setup
        //outlineFBO->BindDepthAttachmentFrom(*gBuffer);
        outlineFBO->Bind();
        outlineFBO->ClearAttachmentI("Mask", 0);
        outlineFBO->ClearAttachmentI("Result", 0);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glDisable(GL_DEPTH_TEST);

        // For each viewport
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            // Render the mask (by drawing all the mesh into it)
            glDrawBuffer(outlineFBO->GetColorAttachmentSlotByName("Mask"));
            glDisable(GL_BLEND);
            maskShader->Bind();
            maskShader->SetInt("u_viewportIndex", i);
            for (const RenderItem& renderItem : RenderDataManager::GetOutlineRenderItems()) {
                maskShader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
                Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);
            }

            // Render the outline (by drawing an instanced quad offset many times)
            outlineShader->Bind();
            outlineShader->SetVec2Array("u_offsets", offsets);
            outlineShader->SetInt("u_viewportIndex", i);
            int instanceCount = offsets.size();
            Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Quad");
            glDrawBuffer(outlineFBO->GetColorAttachmentSlotByName("Result"));
            glBindTextureUnit(1, outlineFBO->GetColorAttachmentHandleByName("Mask"));
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), instanceCount, mesh->baseVertex);
        }

        // Composite the outline
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, outlineFBO->GetColorAttachmentHandleByName("Mask"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        glBindImageTexture(2, outlineFBO->GetColorAttachmentHandleByName("Result"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        compositeShader->Bind();
        glDispatchCompute(gBuffer->GetWidth() / 16, gBuffer->GetHeight() / 16, 1);

        // Clean Up
        glBlendEquation(GL_FUNC_ADD);
        glBindVertexArray(0);
    }
}