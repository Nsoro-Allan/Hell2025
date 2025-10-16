#pragma once
#include "HellEnums.h"
#include <string>

namespace Debug {
    void Update();
    void AddText(const std::string& text);
    void EndFrame();
    void NextDebugRenderMode();
    void NextDebugTextMode();
    void SetDebugRenderMode(DebugRenderMode mode);

    const std::string& GetText();
    const DebugRenderMode& GetDebugRenderMode();
    const DebugTextMode& GetDebugTextMode();
}