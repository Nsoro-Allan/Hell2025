#pragma once
#include <string>

namespace Synth {
    void Init();
    void Update(float deltaTime);
    void PlayNote(int note, int velocity = 127);
    void ReleaseNote(int note);
    int LoadSoundFont(const std::string& filename);
    void SetSustain(bool value);
    bool LogToConsoleEnabled();
}