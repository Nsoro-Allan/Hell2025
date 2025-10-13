#include "Types.h"
#include "BackEnd/BackEnd.h"
#include "Audio/Audio.h"
#include "Imgui/ImguiBackEnd.h"
#include "Util/Util.h"
#include <Imgui/imgui.h>

namespace EditorUI {

    void CreateRightJustifiedText(const std::string& text, float right_margin);

    void FileMenuNode::CreateImguiElement() {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, ImGui::GetStyle().ItemInnerSpacing.y));

        // If this node has children, treat it as a submenu.
        if (!m_children.empty()) {
            if (ImGui::BeginMenu(m_text.c_str())) {
                for (auto& child : m_children) {
                    child.CreateImguiElement();
                }
                ImGui::EndMenu();
            }
        }
        else {
            // No children? Then it's a leaf item.
            if (ImGui::MenuItem(m_text.c_str(), Util::ShortcutToString(m_shortcut))) {
                if (m_callback) {
                    m_callback();
                }
            }
        }

        ImGui::PopStyleVar();
    }

    void FileMenu::CreateImguiElements() {
        if (ImGui::BeginMainMenuBar()) {
            ImGuiBackEnd::RecordFileMenuHeight(ImGui::GetWindowSize().y);
            for (auto& node : m_menuNodes) {
                node.CreateImguiElement();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void FileMenu::Reset() {
        m_menuNodes.clear();
    }

    void NewFileWindow::SetTitle(const std::string& title) {
        m_title = title;
    }

    void NewFileWindow::SetCallback(NewFileCallback callback) {
        m_callback = callback;
    }

    void NewFileWindow::Show() {
        m_isVisible = true;
        m_openedThisFrame = true;
        m_textBuffer[0] = '\0';
    }

    void NewFileWindow::Close() {
        m_isVisible = false;
        m_openedThisFrame = false;
        m_textBuffer[0] = '\0';
    }

    void NewFileWindow::CreateImGuiElements() {
        ImVec2 padding(40.0f, 30.0f);
        float buttonWidth = 150.0f;

        float windowWidth = (float)BackEnd::GetCurrentWindowWidth();
        float windowHeight = (float)BackEnd::GetCurrentWindowHeight();

        ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.5f, windowHeight * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
        ImGui::OpenPopup(m_title.c_str());

        if (ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Enter file name:");
            if (m_openedThisFrame) {
                ImGui::SetKeyboardFocusHere();
                m_openedThisFrame = false;
            }
            ImGui::InputText("##newfile", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));

            // Compute space to center 2 buttons side by side
            float availWidth = ImGui::GetContentRegionAvail().x;
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float totalButtonsWidth = (buttonWidth * 2.0f) + spacing;
            float offsetX = (availWidth - totalButtonsWidth) * 0.5f;

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

            // "Create" button
            if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f))) {
                std::string filename = m_textBuffer;
                if (m_callback) {
                    m_callback(filename);
                }
                Close();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            // "Cancel" button
            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
                Close();
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleVar();

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
    }

    bool NewFileWindow::IsVisible() {
        return m_isVisible;
    }

    void OpenFileWindow::SetTitle(const std::string& title) {
        m_title = title;
    }

    void OpenFileWindow::SetPath(const std::string& path) {
        m_path = path;
    }

    void OpenFileWindow::SetCallback(OpenFileCallback callback) {
        m_callback = callback;
    }

    void OpenFileWindow::Show() {
        m_isVisible = true;
        m_openedThisFrame = true;
        m_selectedIndex = -1;
    }

    void OpenFileWindow::Close() {
        m_isVisible = false;
        m_openedThisFrame = false;
        m_selectedIndex = -1;
    }

    void OpenFileWindow::CreateImGuiElements() {
        ImVec2 padding(40.0f, 30.0f);
        float buttonWidth = 150.0f;
        float listBoxWidth = 600;

        float windowWidth = (float)BackEnd::GetCurrentWindowWidth();
        float windowHeight = (float)BackEnd::GetCurrentWindowHeight();

        ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.5f, windowHeight * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
        ImGui::OpenPopup(m_title.c_str());

        if (ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {

            std::vector<std::string> filenames;
            std::vector<FileInfo> files = Util::IterateDirectory(m_path);
            for (FileInfo& file : files) {
                filenames.push_back(file.name);
            }
            if (m_openedThisFrame) {
                if (filenames.size()) {
                    m_selectedIndex = 0;
                }
                m_openedThisFrame = false;
            }

            int numItems = static_cast<int>(filenames.size());
            float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            float spacingY = ImGui::GetStyle().ItemSpacing.y;
            float framePadY = ImGui::GetStyle().FramePadding.y * 2.0f;
            float desiredHeight = (numItems > 0) ? (numItems * lineHeight + (numItems - 1) * spacingY + framePadY) : lineHeight + framePadY;
            float maxHeight = 800.0f;
            desiredHeight = std::min(desiredHeight, maxHeight);

            if (ImGui::BeginListBox("##FileListBox", ImVec2(listBoxWidth, desiredHeight))) {
                for (int i = 0; i < (int)filenames.size(); i++) {
                    bool isSelected = (m_selectedIndex == i);

                    // Render a selectable item
                    if (ImGui::Selectable(filenames[i].c_str(), isSelected)) {
                        m_selectedIndex = i;
                    }

                    // Set the focus on the newly selected item
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }

            //ImGui::InputText("##Openfile", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));

            // Compute space to center 2 buttons side by side
            float availWidth = ImGui::GetContentRegionAvail().x;
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float totalButtonsWidth = (buttonWidth * 2.0f) + spacing;
            float offsetX = (availWidth - totalButtonsWidth) * 0.5f;

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

            // "Create" button
            if (ImGui::Button("Open", ImVec2(buttonWidth, 0.0f))) {
                if (m_callback && m_selectedIndex >= 0 && m_selectedIndex < filenames.size()) {
                    m_callback(filenames[m_selectedIndex]);
                }
                Close();
                ImGui::CloseCurrentPopup();
            }
            // "Cancel" button
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
                Close();
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
    }

    bool OpenFileWindow::IsVisible() {
        return m_isVisible;
    }

    void StringInput::SetLabel(const std::string& label) {
        m_label = label;
    }

    void StringInput::SetText(const std::string& text) {
        snprintf(m_buffer, sizeof(m_buffer), "%s", text.c_str());
    }

    void StringInput::CreateImGuiElement() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);
        CreateRightJustifiedText(m_label, labelMargin);
        ImGui::SameLine(valueMargin);
        float nameInputFieldWidth = 220.0f;
        if (ImGui::InputText(std::string("##" + m_label).c_str(), m_buffer, IM_ARRAYSIZE(m_buffer))) {
            std::cout << "'" << GetText() << "'\n";
        }
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();
    }

    const std::string& StringInput::GetText() {
        return std::string(m_buffer).c_str();
    }

    void LeftPanel::BeginImGuiElement() {
        float panelHeight = BackEnd::GetCurrentWindowWidth() - ImGuiBackEnd::GetFileMenuHeight();
        ImGui::SetNextWindowPos(ImVec2(0, ImGuiBackEnd::GetFileMenuHeight()), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(EDITOR_LEFT_PANEL_WIDTH, panelHeight), ImGuiCond_Always);
        ImGui::Begin("LeftPanel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    }

    void LeftPanel::EndImGuiElement() {
        ImGui::End();
    }

    void CollapsingHeader::SetTitle(const std::string& text) {
        m_text = text;
    }


    bool CollapsingHeader::CreateImGuiElement() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        //ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
        //ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
        //ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.5f, 0.6f, 1.0f));
        bool isOpen = ImGui::CollapsingHeader(m_text.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
        //ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
        return isOpen;
    }

    void DropDown::SetCurrentOption(const std::string& optionName) {
        for (int i = 0; i < m_options.size(); i++) {
            if (m_options[i] == optionName) {
                m_currentOption = i;
                return;
            }
        }
        std::cout << "DropDown::SetCurrentOption() failed: name '" << optionName << "' not found\n";
        m_currentOption = 0;
    }

    void DropDown::SetText(const std::string text) {
        m_text = text;
    }

    void DropDown::SetOptions(std::vector<std::string> options) {
        m_options = options;
    }

    bool DropDown::CreateImGuiElements(const std::vector<std::string>& disabledOptions) {
        if (m_options.empty())
            return false;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));

        float padding = 40.0f;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;

        ImGui::PushItemWidth(inputFieldWidth);
        CreateRightJustifiedText(m_text, labelMargin);
        ImGui::SameLine(valueMargin);

        std::string comboLabel = "##" + m_text;
        bool        selectionChanged = false;

        // Current selected item (preview) 
        const char* previewStr = m_options[m_currentOption].c_str();

        if (ImGui::BeginCombo(comboLabel.c_str(), previewStr))
        {
            for (int i = 0; i < static_cast<int>(m_options.size()); i++)
            {
                const std::string& option = m_options[i];

                // Check if this option is in the disabled list
                bool isDisabled = (std::find(disabledOptions.begin(), disabledOptions.end(), option) != disabledOptions.end());

                if (isDisabled)
                {
                    // Draw as greyed out and unselectable
                    ImGui::BeginDisabled(true);
                    ImGui::Selectable(option.c_str(), false);
                    ImGui::EndDisabled();
                }
                else
                {
                    // Normal selectable item
                    bool isSelected = (m_currentOption == i);
                    if (ImGui::Selectable(option.c_str(), isSelected))
                    {
                        m_currentOption = i;
                        selectionChanged = true;
                    }
                    // Set default focus for keyboard nav if this is the selected item
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

        return selectionChanged;
    }

    const std::string& DropDown::GetSelectedOptionText() {
        if (m_currentOption >= 0 && m_currentOption < m_options.size()) {
            return m_options[m_currentOption];
        }
        else {
            return "";
        }
    }

    void CheckBox::SetText(const std::string& text) {
        m_text = text;
    }

    void CheckBox::SetState(bool state) {
        m_selectionState = static_cast<uint32_t>(state);
    }

    bool CheckBox::CreateImGuiElements() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));
        float padding = 40.0f;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        CreateRightJustifiedText(m_text, labelMargin);
        ImGui::SameLine(valueMargin);
        bool state = GetState();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        bool stateChanged = ImGui::Checkbox(("##" + m_text).c_str(), &state);
        if (stateChanged) {
            SetState(state);
        }
        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(4);
        return stateChanged;
    }

    bool CheckBox::GetState() {
        return m_selectionState != 0;
    }

    void Vec3Input::SetText(const std::string& text) {
        m_text = text;
    }

    void Vec3Input::SetValue(glm::vec3 value) {
        m_value = value;
    }

    bool Vec3Input::CreateImGuiElements() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));

        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);

        bool valueChanged = false;

        // X Input
        CreateRightJustifiedText(m_text + " X", labelMargin);
        ImGui::SameLine(valueMargin);
        if (ImGui::InputFloat(std::string("##X" + m_text).c_str(), &m_value[0], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal)) {
            valueChanged = true;
        }

        // Y Input
        CreateRightJustifiedText("Y", labelMargin);
        ImGui::SameLine(valueMargin);
        if (ImGui::InputFloat(std::string("##Y" + m_text).c_str(), &m_value[1], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal)) {
            valueChanged = true;
        }

        // Z Input
        CreateRightJustifiedText("Z", labelMargin);
        ImGui::SameLine(valueMargin);
        if (ImGui::InputFloat(std::string("##Z" + m_text).c_str(), &m_value[2], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal)) {
            valueChanged = true;
        }

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

        return valueChanged;
    }

    glm::vec3 Vec3Input::GetValue() {
        return m_value;
    }

    void IntegerInput::SetText(const std::string& text) {
        m_text = text;
    }

    void IntegerInput::SetValue(int32_t value) {
        m_value = value;
    }

    void IntegerInput::SetRange(int32_t min, int32_t max) {
        m_min = min;
        m_max = max;
    }

    bool IntegerInput::CreateImGuiElements() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));

        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);

        bool valueChanged = false;

        // Input
        CreateRightJustifiedText(m_text, labelMargin);
        ImGui::SameLine(valueMargin);
        if (ImGui::InputInt(std::string("##" + m_text).c_str(), &m_value)) {
            valueChanged = true;
            m_value = std::clamp(m_value, m_min, m_max);
        }

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

        return valueChanged;
    }

    int32_t IntegerInput::GetValue() {
        return m_value;
    }

    //

    void FloatInput::SetText(const std::string& text) {
        m_text = text;
    }

    void FloatInput::SetValue(float value) {
        m_value = value;
    }

    void FloatInput::SetRange(float min, float max) {
        m_min = min;
        m_max = max;
    }

    bool FloatInput::CreateImGuiElements() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));

        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);

        bool valueChanged = false;

        // Input
        CreateRightJustifiedText(m_text, labelMargin);
        ImGui::SameLine(valueMargin);
        if (ImGui::InputFloat(std::string("##" + m_text).c_str(), &m_value)) {
            valueChanged = true;
            m_value = std::clamp(m_value, m_min, m_max);
        }

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

        return valueChanged;
    }

    float FloatInput::GetValue() {
        return m_value;
    }


    //

    void FloatSliderInput::SetText(const std::string& text) {
        m_text = text;
    }

    void FloatSliderInput::SetValue(float value) {
        m_value = value;
    }

    void FloatSliderInput::SetRange(float min, float max) {
        m_min = min;
        m_max = max;
    }

    bool FloatSliderInput::CreateImGuiElements() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));

        float padding = 40.0f;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);

        bool valueChanged = false;

        CreateRightJustifiedText(m_text, labelMargin);
        ImGui::SameLine(valueMargin);

        // if (ImGui::InputFloat(std::string("##" + m_text).c_str(), &m_value, 0.0f, 0.0f, "%.3f")) {
        if (ImGui::SliderFloat(std::string("##" + m_text).c_str(), &m_value, m_min, m_max, "%.3f")) {
            valueChanged = true;
        }

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

        return valueChanged;
    }

    float FloatSliderInput::GetValue() {
        return m_value;
    }

    void Outliner::AddType(const std::string name) {
        m_types[name] = std::vector<std::string>();
    }

    void Outliner::SetItems(const std::string name, const std::vector<std::string>& items) {
        m_types[name] = items;
    }

    bool Outliner::CreateImGuiElements(float height) {
        const float objectIndent = 60.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

        ImGuiWindowFlags childFlags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened;
        ImGui::BeginChild("OutlinerScroll", ImVec2(0.0f, height), false, childFlags);

        for (auto& kv : m_types) {
            const std::string& type = kv.first;
            const std::vector<std::string>& items = kv.second;
            if (items.empty()) continue;

            if (ImGui::TreeNodeEx(type.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Indent(objectIndent);

                // Optional: clip for large lists
                ImGuiListClipper clipper;
                clipper.Begin((int)items.size());
                while (clipper.Step()) {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                        const std::string& item = items[i];
                        bool isSelected = (m_selectedItem == item && m_selectedType == type);
                        if (ImGui::Selectable(item.c_str(), isSelected)) {
                            m_selectedItem = item;
                            m_selectedType = type;
                            ImGui::SetScrollHereY(0.25f);
                        }
                    }
                }

                ImGui::Unindent(objectIndent);
                ImGui::TreePop();
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        return true;
    }

    bool Outliner::CreateImGuiElements() {
        float objectIndent = 60.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

        for (auto& kv : m_types) {
            const std::string& type = kv.first;
            const std::vector<std::string>& items = kv.second;

            // Skip if no items in this category
            if (items.empty()) continue;

            if (ImGui::TreeNodeEx(type.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Indent(objectIndent);

                for (const std::string& item : items) {
                    // Check if this item is currently selected
                    bool isSelected = (m_selectedItem == item && m_selectedType == type);

                    // Pass isSelected to ImGui::Selectable
                    if (ImGui::Selectable(item.c_str(), isSelected)) {
                        // On click, record the new selection
                        m_selectedItem = item;
                        m_selectedType = type;
                      //  std::cout << "Selected: " << item << "\n";
                    }
                }

                ImGui::Unindent(objectIndent);
                ImGui::TreePop();
            }
        }

        ImGui::PopStyleVar();
        return true;
    }

    void Outliner::SetSelectedType(const std::string& type) {
        m_selectedType = type;
    }

    void Outliner::SetSelectedItem(const std::string& item) {
        m_selectedItem = item;
    }

    const std::string& Outliner::GetSelectedType() {
        return m_selectedType;
    }

    const std::string& Outliner::GetSelectedItem() {
        return m_selectedItem;
    }

    void CreateRightJustifiedText(const std::string& text, float right_margin) {
        ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
        float cursor_x = right_margin - text_size.x;
        ImGui::SetCursorPosX(cursor_x);
        ImGui::TextUnformatted(text.c_str());
    }
}