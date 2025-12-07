#include "Renderer.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/Vulkan/Renderer/VK_renderer.h"
#include "BackEnd/BackEnd.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Timer.hpp"

namespace Renderer {

    struct RendererSettingsSet {
        RendererSettings game;
        RendererSettings houseEditor;
        RendererSettings mapHeightEditor;
        RendererSettings mapObjectEditor;
    } g_rendererSettingsSet;

    std::vector<bool> g_freeWoundMaskIndices;

    void InitMain() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::InitMain();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan TODO: Renderer::InitMain()";
        }

        g_rendererSettingsSet.houseEditor.rendererOverrideState = RendererOverrideState::CAMERA_NDOTL;
        g_rendererSettingsSet.houseEditor.drawGrass = false;

        g_rendererSettingsSet.mapHeightEditor.rendererOverrideState = RendererOverrideState::CAMERA_NDOTL;
        g_rendererSettingsSet.mapHeightEditor.drawGrass = false;

        g_rendererSettingsSet.mapObjectEditor.rendererOverrideState = RendererOverrideState::CAMERA_NDOTL;
        g_rendererSettingsSet.mapObjectEditor.drawGrass = true;
    }

    void RenderLoadingScreen() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RenderLoadingScreen();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            VulkanRenderer::RenderLoadingScreen();
        }
    }

    void PreGameLogicComputePasses() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::PreGameLogicComputePasses();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: PreGameLogicComputePasses()";
        }
    }

    void RenderGame() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RenderGame();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: RenderGame()";
        }
    }

    void HotloadShaders() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::HotloadShaders();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: HotloadShaders()";
        }
    }

    void RecalculateAllHeightMapData(bool blitWorldMap) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RecalculateAllHeightMapData(blitWorldMap);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: RecalculateAllHeightMapData()";
        }
    }

    void DrawPoint(const glm::vec3& position, const glm::vec4& color, bool obeyDepth, int exclusiveViewportIndex) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawPoint(position, color, obeyDepth, exclusiveViewportIndex);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawPoint()";
        }
    }

    void DrawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color, bool obeyDepth, int exclusiveViewportIndex, int ignoredViewportIndex) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawLine(begin, end, color, obeyDepth, exclusiveViewportIndex, ignoredViewportIndex);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawLine()";
        }
    }

    void DrawLine2D(const glm::ivec2& begin, const glm::ivec2& end, const glm::vec4& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawLine2D(begin, end, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawLine2D()";
        }
    }

    void DrawAABB(const AABB& aabb, const glm::vec4& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawAABB()";
        }
    }

    void DrawAABB(const AABB& aabb, const glm::vec4& color, const glm::mat4& worldTransform) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color, worldTransform);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawAABB()";
        }
    }

    void DrawOBB(const OBB& obb, const glm::vec4& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawOBB(obb, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawOBB()";
        }
    }

    void DrawFrustum(const Frustum& frustum, const glm::vec4& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawFrustum(frustum, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawFrustum()";
        }
    }

    void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawSphere(position, radius, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: DrawSphere()";
        }
    }

    void ReadBackHeightMapData(Map* map) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::ReadBackHeightMapData(map);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: ReadBackHeightMapData()";
        }
    }

    RendererSettings& GetCurrentRendererSettings() {
        if (Editor::IsOpen()) {
            switch (Editor::GetEditorMode()) {
                case EditorMode::HOUSE_EDITOR:      return g_rendererSettingsSet.houseEditor;
                case EditorMode::MAP_HEIGHT_EDITOR: return g_rendererSettingsSet.mapHeightEditor;
                case EditorMode::MAP_OBJECT_EDITOR: return g_rendererSettingsSet.mapObjectEditor;
            }
        }
        return g_rendererSettingsSet.game;
    }

    void NextRendererOverrideState() {
        RendererSettings& rendererSettings = GetCurrentRendererSettings();
        int i = static_cast<int>(rendererSettings.rendererOverrideState);
        i = (i + 1) % static_cast<int>(RendererOverrideState::STATE_COUNT);
        rendererSettings.rendererOverrideState = static_cast<RendererOverrideState>(i);
    }

    int32_t GetNextFreeWoundMaskIndexAndMarkItTaken() {
        for (int i = 0; i < g_freeWoundMaskIndices.size(); i++) {
            if (g_freeWoundMaskIndices[i] == true) {
                g_freeWoundMaskIndices[i] = false;
                return i;
            }
        }

        // Should never happen, unless you ran out of array levels, in which case you need to increase the size of the array
        for (int i = 0; i < g_freeWoundMaskIndices.size(); i++) {
            Logging::Error() << "GetNextFreeWoundMaskIndexAndMarkItTaken() failed because you ran out of free wound mask textures";
            std::cout << i << ": " << g_freeWoundMaskIndices[i] << "\n";
        }
        return -1;
    }

    void InitWoundMaskArray() {
        // Create and init all wound mask indices to true, aka available
        g_freeWoundMaskIndices.assign(WOUND_MASK_TEXTURE_ARRAY_SIZE, true);
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::ClearAllWoundMasks();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan: InitWoundMaskArray()";
        }
    }

    void MarkWoundMaskIndexAsAvailable(int32_t index) {
        if (index < 0 || index >= g_freeWoundMaskIndices.size()) {
            Logging::Error() << "Renderer::MarkWoundMaskIndexAsAvailable() failed. Index '" << index << "' is out of range of size '" << g_freeWoundMaskIndices.size() << "'";
            return;
        }
        g_freeWoundMaskIndices[index] = true;
    }
}