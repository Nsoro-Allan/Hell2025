#include "MidiFileManager.h"
#include "MidiFile.h"
#include <iostream>

namespace MidiFileManager {

    std::vector<MidiFile> g_midiFiles;
    int g_trackIndex = 0;
    bool g_playMusic = false;

    std::vector<std::string> g_debugTextTime;
    std::vector<std::string> g_debugTextEvents;
    std::vector<std::string> g_debugTextTimeDurations;
    std::vector<std::string> g_debugTextTimeVelocities;

    void Init() {
        LoadMidiFile("Goat.mid");
        LoadMidiFile("Nocturne.mid");
        LoadMidiFile("Czardas.mid");
    }

    void LoadMidiFile(const std::string& filename) {
        if (MidiFile midiFile; midiFile.LoadFromFile(filename)) {
            g_midiFiles.push_back(std::move(midiFile));
        }
    }

    void Reset() {
        for (MidiFile& midiFile : g_midiFiles) {
            midiFile.Stop();
        }
        g_debugTextTime.clear();
        g_debugTextEvents.clear();
        g_debugTextTimeDurations.clear();
        g_debugTextTimeVelocities.clear();
        g_playMusic = false;
    }

    void PlayTrack(int trackIndex) {
        if (trackIndex < 0 || trackIndex >= g_midiFiles.size()) return;

        Reset();

        g_playMusic = true;
        g_trackIndex = trackIndex;
        g_midiFiles[g_trackIndex].Play();
    }

    void Update(float deltaTime) {
        // Play Goat
        if (Input::KeyPressed(HELL_KEY_NUM_LOCK)) {
            PlayTrack(0);
        }

        // Play Nocturne
        if (Input::KeyPressed(HELL_KEY_PAUSE)) {
            PlayTrack(1);
        }

        // Play Czardas
        if (Input::KeyPressed(HELL_KEY_PAGE_UP)) {
            PlayTrack(2);
        }

        // Stop
        if (Input::KeyPressed(HELL_KEY_END)) {
            Reset();
        }

        // Update
        if (g_playMusic) {
            g_midiFiles[g_trackIndex].Update(deltaTime);

            // Done?
            if (g_midiFiles[g_trackIndex].IsComplete()) {
                Reset();
            }
        }
    }

    void AddDebugTextTimes(const std::string& text) {
        g_debugTextTime.push_back(text);
        while (g_debugTextTime.size() > 10) {
            g_debugTextTime.erase(g_debugTextTime.begin());
        }
    }

    void AddDebugTextEvent(const std::string& text) {
        g_debugTextEvents.push_back(text);
        while (g_debugTextEvents.size() > 10) {
            g_debugTextEvents.erase(g_debugTextEvents.begin());
        }
    }
    void AddDebugTextDurations(const std::string& text) {
        g_debugTextTimeDurations.push_back(text);
        while (g_debugTextTimeDurations.size() > 10) {
            g_debugTextTimeDurations.erase(g_debugTextTimeDurations.begin());
        }
    }

    void AddDebugTextVelocity(const std::string& text) {
        g_debugTextTimeVelocities.push_back(text);
        while (g_debugTextTimeVelocities.size() > 10) {
            g_debugTextTimeVelocities.erase(g_debugTextTimeVelocities.begin());
        }
    }

    bool IsPlaying() {
        return g_playMusic;
    }

    std::string GetDebugTextTime() {
        std::string result;
        for (const std::string& text : g_debugTextTime) {
            result += text + "\n";
        }
        return result;
    }

    std::string GetDebugTextEvents() {
        std::string result;
        for (const std::string& text : g_debugTextEvents) {
            result += text + "\n";
        }
        return result;
    }

    std::string GetDebugTextTimeDurations() {
        std::string result;
        for (const std::string& text : g_debugTextTimeDurations) {
            result += text + "\n";
        }
        return result;
    }

    std::string GetDebugTextVelocity() {
        std::string result;
        for (const std::string& text : g_debugTextTimeVelocities) {
            result += text + "\n";
        }
        return result;
    }
}