#pragma once
#include "HellTypes.h"
#include "HellFunctionTypes.h"
#include <functional>
#include <map>
#include <vector>

namespace EditorUI {

    struct FileMenuNode {
        FileMenuNode() = default;
        FileMenuNode(const std::string& text, std::function<void()> callback = nullptr, const std::string& shortcut = "", bool addPadding = true);
        FileMenuNode& AddChild(const std::string& text, std::function<void()> callback = nullptr, const std::string& shortcut = "", bool addPadding = true);
        void CreateImguiElement();

    private:
        std::string m_text;
        std::string m_shortcut = "";
        std::function<void()> m_callback = nullptr;
        std::vector<FileMenuNode> m_children;
    };

    struct FileMenu {
        FileMenuNode& AddMenuNode(const std::string& text, std::function<void()> callback = nullptr, bool addPadding = false);
        void CreateImguiElements();
        void Reset();

    private:
        std::vector<FileMenuNode> m_menuNodes;
    };

    struct NewFileWindow {
        void SetTitle(const std::string& title);
        void SetCallback(NewFileCallback callback);
        void Show();
        void Close();
        void CreateImGuiElements();
        bool IsVisible();

    private:
        std::string m_title = "";
        bool m_isVisible = false;
        bool m_openedThisFrame = false;
        char m_textBuffer[256] = "";
        NewFileCallback m_callback = nullptr;
    };

    struct OpenFileWindow {
        void SetTitle(const std::string& title);
        void SetPath(const std::string& path);
        void SetCallback(OpenFileCallback callback);
        void Show();
        void Close();
        void CreateImGuiElements();
        bool IsVisible();

    private:
        std::string m_title = "";
        std::string m_path = "";
        int m_selectedIndex = -1;
        bool m_isVisible = false;
        bool m_openedThisFrame = false;
        OpenFileCallback m_callback = nullptr;
    };

    struct StringInput {
        void SetLabel(const std::string& label);
        void SetText(const std::string& text);
        void CreateImGuiElement();
        const std::string& GetText();

    private:
        std::string m_label = "";
        char m_buffer[128];
    };

    struct LeftPanel {
        void BeginImGuiElement();
        void EndImGuiElement();
    };

    struct CollapsingHeader {
        void SetTitle(const std::string& text);
        bool CreateImGuiElement();

    private:
        std::string m_text;
    };

    struct DropDown {
        void SetText(const std::string text);
        void SetOptions(std::vector<std::string> options);
        void SetCurrentOption(const std::string& optionName);
        //bool CreateImGuiElements();
        bool CreateImGuiElements(const std::vector<std::string>& disabledOptions = {});
        const std::string& GetSelectedOptionText();

    private:
        std::string m_text = "";
        std::vector<std::string> m_options;
        int32_t m_currentOption = 0;
    };

    struct CheckBox {
        void SetText(const std::string& text);
        void SetState(bool state);
        bool CreateImGuiElements();
        bool GetState();

    private:
        std::string m_text = "";
        uint32_t m_selectionState = 0;
    };

    struct Vec3Input {
        void SetText(const std::string& text);
        void SetValue(glm::vec3 value);
        bool CreateImGuiElements();
        glm::vec3 GetValue();

    private:
        std::string m_text = "";
        glm::vec3 m_value = glm::vec3(0.0f);
    };

    struct IntegerInput {
        void SetText(const std::string& text);
        void SetValue(int32_t value);
        void SetRange(int32_t min, int32_t max);
        bool CreateImGuiElements();
        int32_t GetValue();

    private:
        std::string m_text = "";
        int32_t m_value = 0;
        int32_t m_min = 0;
        int32_t m_max = 1;
    };

    struct FloatInput{
        void SetText(const std::string& text);
        void SetValue(float value);
        void SetRange(float min, float max);
        bool CreateImGuiElements();
        float GetValue();

    private:
        std::string m_text;
        float m_value = 0;
        float m_min = 0;
        float m_max = 1;
    };

    struct FloatSliderInput {
        void SetText(const std::string& text);
        void SetValue(float value);
        void SetRange(float min, float max);
        bool CreateImGuiElements();
        float GetValue();

    private:
        std::string m_text;
        float m_value = 0;
        float m_min = 0.0f;
        float m_max = 1.0f;
    };

    struct Outliner {
        bool CreateImGuiElements();
        void AddType(const std::string name);
        void SetItems(const std::string name, std::vector<std::string>& items);
        void SetSelectedType(const std::string& type);
        void SetSelectedItem(const std::string& item);
        const std::string& GetSelectedType();
        const std::string& GetSelectedItem();
    private:
        std::map<std::string, std::vector<std::string>> m_types;
        std::string m_selectedItem;
        std::string m_selectedType;
    };
}