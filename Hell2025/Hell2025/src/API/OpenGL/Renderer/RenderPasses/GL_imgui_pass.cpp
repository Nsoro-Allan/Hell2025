#include "API/OpenGL/Renderer/GL_renderer.h"

#include "BackEnd/GLFWIntegration.h"
#include "Input/Input.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "AssetManagement/AssetManager.h"
#include "BackEnd/BackEnd.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "UI/UIBackEnd.h"
#include "Config/Config.h"
#include "Audio/Audio.h"

#include "Editor/Editor.h"

#include <fstream>
#include <string>

namespace OpenGLRenderer {

    void ImGuiPass() {
        const Resolutions& resolutions = Config::GetResolutions();
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();
        int fullScreenWidth = BackEnd::GetFullScreenWidth();
        int fullScreenHeight = BackEnd::GetFullScreenHeight();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());

        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(10.0f, 10.0f);
        style.ItemSpacing = ImVec2(10.0f, 10.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        if (Editor::IsOpen()) {
            Editor::CreateFileMenuImGuiElements();

            if (Editor::GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) {
                Editor::CreateMapHeightEditorImGuiElements();
            }

            if (Editor::GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
                Editor::CreateMapObjectEditorImGuiElements();
            }

            if (Editor::GetEditorMode() == EditorMode::HOUSE_EDITOR) {
                Editor::CreateHouseEditorImGuiElements();
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}