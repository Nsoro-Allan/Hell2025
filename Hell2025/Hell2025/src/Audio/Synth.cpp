#include "Synth.h"
#include "fluidsynth.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include <iostream>

void PrintSettings(void* data, const char* name, int type) {
    std::cout << "Setting: Name='" << name << "', Type=" << type << "\n";
}

void ErrorCallback(int level, const char* message, void* user_data) {
    if (Synth::LogToConsoleEnabled()) {
        fprintf(stderr, "%s\n", message);
    }
}

namespace Synth {
    fluid_audio_driver_t* g_driver;
    fluid_settings_t* g_settings;
    fluid_synth_t* g_synth;
    bool g_logToConsole = false;
        
    void Init() {
        g_settings = new_fluid_settings();
        g_synth = new_fluid_synth(g_settings);
        g_driver = new_fluid_audio_driver(g_settings, g_synth);

        fluid_settings_setstr(g_settings, "audio.driver", "wasapi");
        fluid_settings_setnum(g_settings, "synth.sample-rate", 44100);
        fluid_settings_setint(g_settings, "audio.wasapi.exclusive-mode", 1);
        fluid_settings_setint(g_settings, "audio.period-size", 128);
        fluid_settings_setint(g_settings, "audio.periods", 2);
        fluid_settings_setstr(g_settings, "audio.driver", "wasapi");
        fluid_settings_setnum(g_settings, "synth.sample-rate", 44100);
        fluid_settings_setint(g_settings, "synth.reverb.active", 0);
        fluid_settings_setint(g_settings, "synth.chorus.active", 0);
        fluid_settings_setint(g_settings, "synth.verbose", 0);

        fluid_set_log_function(FLUID_WARN, ErrorCallback, nullptr);
       

        //fluid_settings_foreach(g_settings, nullptr, PrintSettings);

        // Load sound font
        int sfid0 = LoadSoundFont("YamahaGrandLiteV2.sf2");
        int sfid1 = LoadSoundFont("DiscardedWurlitzer.sf2");
        //int sfid2 = LoadSoundFont("OutOfTune.sf2");
        
        fluid_synth_program_select(g_synth, 0, sfid0, 0, 0);
        fluid_synth_program_select(g_synth, 1, sfid1, 0, 0);
        //fluid_synth_program_select(g_synth, 2, sfid2, 0, 0);

        fluid_synth_set_gain(g_synth, 4.0);
    }

    int LoadSoundFont(const std::string& filename) {
        std::string filepath = "res/audio/piano/" + filename;
        int id = fluid_synth_sfload(g_synth, filepath.c_str(), 1);
        if (id == FLUID_FAILED) {
            if (LogToConsoleEnabled()) {
                std::cout << "Synth::LoadSoundFont() failed to load " << filepath << "\n";
            }
            return 0;
        }
        if (LogToConsoleEnabled()) {
            std::cout << "Loaded sound font: " << filename << "\n";
        }
        return id;
    }

    void PlayNote(int note, int velocity) {
        fluid_synth_noteon(g_synth, 0, note, velocity);
    }

    void ReleaseNote(int note) {
        fluid_synth_noteoff(g_synth, 0, note);
    }

    void Update(float deltaTime) {
       
    }

    void CleanUp() {
        delete_fluid_audio_driver(g_driver);
        delete_fluid_synth(g_synth);
        delete_fluid_settings(g_settings);
    }

    void SetSustain(bool sustainOn) {
        if (!g_synth) {
            return; // Safety check
        }

        int MIDI_CHANNEL = 0;

        // MIDI CC 64: Sustain Pedal (Damper)
        // Value >= 64 is ON, Value < 64 is OFF.
        // Common values are 127 for ON and 0 for OFF.
        int value = sustainOn ? 127 : 0;
        int controller = 64; // Sustain pedal controller number

        // Send the Control Change message to FluidSynth
        int result = fluid_synth_cc(g_synth, MIDI_CHANNEL, controller, value);

        if (result != FLUID_OK && LogToConsoleEnabled()) {
            std::cerr << "FluidSynth Error: fluid_synth_cc failed for sustain." << std::endl;
        }
        else {
          // Optional log
          // std::cout << "Synth::SetSustain: Sent CC 64, Channel=" << MIDI_CHANNEL << ", Value=" << value << std::endl;
        }
    }

    bool LogToConsoleEnabled() {
        return g_logToConsole;
    }
}