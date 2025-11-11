#pragma once
#include <string>
#include <vector>

#include "Input/Input.h"

namespace MidiFileManager {
    void Init();
    void LoadMidiFile(const std::string& filename);
    void Update(float deltaTime);

    void AddDebugTextTimes(const std::string& text);
    void AddDebugTextEvent(const std::string& text);
    void AddDebugTextVelocity(const std::string& text);
    void AddDebugTextDurations(const std::string& text);

    bool IsPlaying();

    std::string GetDebugTextTime();
    std::string GetDebugTextEvents();
    std::string GetDebugTextVelocity();
    std::string GetDebugTextTimeDurations();
}