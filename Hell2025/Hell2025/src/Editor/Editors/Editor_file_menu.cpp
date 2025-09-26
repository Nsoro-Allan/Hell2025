#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Imgui/Types/Types.h"
#include <Util.h>

namespace Editor {
    EditorUI::FileMenu g_fileMenu;

    void InitFileMenuImGuiElements() {
        g_fileMenu.Reset();

        EditorUI::FileMenuNode& file = g_fileMenu.AddMenuNode("File", nullptr);
        file.AddChild("New", []() { ShowNewMapWindow(); }, "F2");
        file.AddChild("Open", []() { ShowOpenMapWindow(); }, "F3");
        file.AddChild("Save", []() { Callbacks::SaveMap("Shit"); }, "Ctrl+S");
        file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");

        EditorUI::FileMenuNode& editor = g_fileMenu.AddMenuNode("Editor");
        editor.AddChild("House", &Callbacks::OpenHouseEditor, "F4");
        editor.AddChild("Map Objects", &Callbacks::OpenMapObjectEditor, "F5");
        editor.AddChild("Map Height", &Callbacks::OpenMapHeightEditor, "F6");

        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            EditorUI::FileMenuNode& insert = g_fileMenu.AddMenuNode("Insert");
            insert.AddChild("Reinsert last", []() { nullptr; }, "Ctrl T");

            EditorUI::FileMenuNode& locations = insert.AddChild("Locations", nullptr);
            locations.AddChild("House", Callbacks::BeginAddingHouse);
            locations.AddChild("Player Spawn (Campaign)", nullptr);
            locations.AddChild("Player Spawn (Deathmatch)", nullptr);

            EditorUI::FileMenuNode& nature = insert.AddChild("Nature", nullptr);
            nature.AddChild("BlackBerries", Callbacks::BeginAddingBlackBerries);
            nature.AddChild("Tree", Callbacks::BeginAddingTree);

            EditorUI::FileMenuNode& pickups = insert.AddChild("Pick Ups", nullptr);

            EditorUI::FileMenuNode& weapons = pickups.AddChild("Weapons", nullptr);
            weapons.AddChild("AKS74U", nullptr);
            weapons.AddChild("FN-P90", nullptr);
            weapons.AddChild("Glock", nullptr);
            weapons.AddChild("Golden Glock", nullptr);
            weapons.AddChild("Remington 870", nullptr);
            weapons.AddChild("SPAS", nullptr);
            weapons.AddChild("Tokarev", nullptr);

            EditorUI::FileMenuNode& ammo = pickups.AddChild("Ammo", nullptr);
            ammo.AddChild("AKS74U", nullptr);
            ammo.AddChild("FN-P90", nullptr);
            ammo.AddChild("Glock", nullptr);
            ammo.AddChild("Shotgun Shells Buckshot", nullptr);
            ammo.AddChild("Shotgun Shells Slug", nullptr);
            ammo.AddChild("Tokarev", nullptr);

            Logging::Debug() << "Init house object editor file menu" << Util::EditorModeToString(GetEditorMode());
        }

        EditorUI::FileMenuNode& run = g_fileMenu.AddMenuNode("Run");
        run.AddChild("New Run", nullptr, "F1");
    }

    void CreateFileMenuImGuiElements() {
        g_fileMenu.CreateImguiElements();
    }
}
