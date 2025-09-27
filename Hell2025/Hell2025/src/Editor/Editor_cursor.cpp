#include "Editor.h"
#include "HellDefines.h"
#include "BackEnd/BackEnd.h"

namespace Editor {

    void UpdateCursor() {
        // Resizing dividers
        if (IsHorizontalDividerHovered() && IsVerticalDividerHovered()) {
            BackEnd::SetCursor(HELL_CURSOR_CROSSHAIR);
        }
        else if (IsHorizontalDividerHovered()) {
            BackEnd::SetCursor(HELL_CURSOR_HRESIZE);
        }
        else if (IsVerticalDividerHovered()) {
            BackEnd::SetCursor(HELL_CURSOR_VRESIZE);
        }
        // Hovering dividers
        else if (GetEditorState() == EditorState::RESIZING_HORIZONTAL_VERTICAL) {
            BackEnd::SetCursor(HELL_CURSOR_CROSSHAIR);
        }
        else if (GetEditorState() == EditorState::RESIZING_HORIZONTAL) {
            BackEnd::SetCursor(HELL_CURSOR_HRESIZE);
        }
        else if (GetEditorState() == EditorState::RESIZING_VERTICAL) {
            BackEnd::SetCursor(HELL_CURSOR_VRESIZE);
        }
        else if (GetEditorState() == EditorState::PLACE_DOOR ||
                 GetEditorState() == EditorState::PLACE_HOUSE ||
                 GetEditorState() == EditorState::PLACE_PICTURE_FRAME ||
                 GetEditorState() == EditorState::PLACE_TREE ||
                 GetEditorState() == EditorState::PLACE_WALL ||
                 GetEditorState() == EditorState::PLACE_WINDOW ||
                 GetEditorState() == EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN ||
                 GetEditorState() == EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN) {
            BackEnd::SetCursor(HELL_CURSOR_CROSSHAIR);
        }
        // Nothing? Then the arrow
        else {
            BackEnd::SetCursor(HELL_CURSOR_ARROW);
        }
    }
}