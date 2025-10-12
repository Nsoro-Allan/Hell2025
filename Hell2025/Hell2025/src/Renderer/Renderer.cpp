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
            // Vulkan TODO:Renderer::PreGameLogicComputePasses();
        }
    }

    void RenderGame() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RenderGame();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // Vulkan TODO:Renderer::RenderLoadingScreen();
        }
    }

    void HotloadShaders() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::HotloadShaders();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            //VulkanRenderer::HotloadShaders();
        }
    }

    void RecalculateAllHeightMapData(bool blitWorldMap) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RecalculateAllHeightMapData(blitWorldMap);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawPoint(glm::vec3 position, glm::vec3 color, bool obeyDepth, int exclusiveViewportIndex) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawPoint(position, color, obeyDepth, exclusiveViewportIndex);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color, bool obeyDepth, int exclusiveViewportIndex, int ignoredViewportIndex) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawLine(begin, end, color, obeyDepth, exclusiveViewportIndex, ignoredViewportIndex);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawAABB(const AABB& aabb, const glm::vec3& color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawAABB(const AABB& aabb, const glm::vec3& color, const glm::mat4& worldTransform) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color, worldTransform);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }


    void DrawSphere(glm::vec3 position, float radius, glm::vec3 color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawSphere(position, radius, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void ReadBackHeightMapData(Map* map) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::ReadBackHeightMapData(map);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "Vulkan TODO: Renderer::GetHeightMapData()";
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
            Logging::ToDo() << "Vulkan TODO: Renderer::ClearAllWoundMasks()";
            //VulkanRenderer::ClearAllWoundMasks();
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