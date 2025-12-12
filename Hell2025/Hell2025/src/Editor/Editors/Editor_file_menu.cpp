#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Imgui/Types/Types.h"
#include <Util.h>

namespace Editor {
    EditorUI::FileMenu g_fileMenu;

    void InitFileMenuImGuiElements() {
        g_fileMenu.Reset();

        EditorUI::FileMenuNode& file = g_fileMenu.AddMenuNode("File", Shortcut::NONE);
        file.AddChild("New",    Shortcut::F2,       ShowNewMapWindow);
        file.AddChild("Open",   Shortcut::F3,       ShowOpenMapWindow);
        file.AddChild("Save",   Shortcut::CTRL_S,   Editor::Save);
        file.AddChild("Quit",   Shortcut::ESC,      Callbacks::QuitProgram);

        EditorUI::FileMenuNode& editor = g_fileMenu.AddMenuNode("Editor", Shortcut::NONE);
        editor.AddChild("House",        Shortcut::F4,   Callbacks::OpenHouseEditor);
        editor.AddChild("Map Objects",  Shortcut::F5,   Callbacks::OpenMapObjectEditor);
        editor.AddChild("Map Height",   Shortcut::F6,   Callbacks::OpenMapHeightEditor);

        if (GetEditorMode() != EditorMode::MAP_HEIGHT_EDITOR) {
            EditorUI::FileMenuNode& insert = g_fileMenu.AddMenuNode("Insert", Shortcut::NONE);
            insert.AddChild("Reinsert last",   Shortcut::CTRL_T,   nullptr);

            EditorUI::FileMenuNode& bathroom = insert.AddChild("Bathroom", Shortcut::NONE);
            bathroom.AddChild("Basin", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::BATHROOM_BASIN);
            bathroom.AddChild("Cabinet", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::BATHROOM_CABINET);
            bathroom.AddChild("Toilet", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::TOILET);
            bathroom.AddChild("Towel", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::BATHROOM_TOWEL_RACK);

            EditorUI::FileMenuNode& interior = insert.AddChild("Interior", Shortcut::NONE);
            interior.AddChild("Door",          Shortcut::NONE, Callbacks::BeginAddingDoor);
            interior.AddChild("Couch",         Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::COUCH);
            interior.AddChild("Drawers Small", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::DRAWERS_SMALL);
            interior.AddChild("Drawers Large", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::DRAWERS_LARGE);

			interior.AddChild("Fireplace (Open)", Shortcut::NONE, Editor::PlaceFireplace, FireplaceType::DEFAULT);
			interior.AddChild("Fireplace (WoodStove)", Shortcut::NONE, Editor::PlaceFireplace, FireplaceType::WOOD_STOVE);

            interior.AddChild("Window", Shortcut::NONE, Callbacks::BeginAddingWindow);

            EditorUI::FileMenuNode& nature = insert.AddChild("Nature", Shortcut::NONE);
            nature.AddChild("BlackBerries", Shortcut::NONE,     Callbacks::BeginAddingBlackBerries);
            nature.AddChild("Tree",         Shortcut::NONE,     Callbacks::BeginAddingTree);
            
            EditorUI::FileMenuNode& pickups = insert.AddChild("Pick Ups", Shortcut::NONE);

            EditorUI::FileMenuNode& testModels = insert.AddChild("Test Models", Shortcut::NONE);
            testModels.AddChild("Test Model", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::TEST_MODEL);
            testModels.AddChild("Test Model 2", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::TEST_MODEL2);
            testModels.AddChild("Test Model 3", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::TEST_MODEL3);
            testModels.AddChild("Test Model 4", Shortcut::NONE, Editor::PlaceGenericObject, GenericObjectType::TEST_MODEL4);
            
            EditorUI::FileMenuNode& weapons = pickups.AddChild("Weapons", Shortcut::NONE);
            weapons.AddChild("AKS74U",          Shortcut::NONE,     nullptr);
            weapons.AddChild("FN-P90",          Shortcut::NONE,     nullptr);
            weapons.AddChild("Glock",           Shortcut::NONE,     nullptr);
            weapons.AddChild("Golden Glock",    Shortcut::NONE,     nullptr);
            weapons.AddChild("Remington 870",   Shortcut::NONE,     nullptr);
            weapons.AddChild("SPAS",            Shortcut::NONE,     nullptr);
            weapons.AddChild("Tokarev",         Shortcut::NONE,     nullptr);
            
            EditorUI::FileMenuNode& ammo = pickups.AddChild("Ammo", Shortcut::NONE);
            ammo.AddChild("AKS74U",                     Shortcut::NONE,     nullptr);
            ammo.AddChild("FN-P90",                     Shortcut::NONE,     nullptr);
            ammo.AddChild("Glock",                      Shortcut::NONE,     nullptr);
            ammo.AddChild("Shotgun Shells Buckshot",    Shortcut::NONE,     nullptr);
            ammo.AddChild("Shotgun Shells Slug",        Shortcut::NONE,     nullptr);
            ammo.AddChild("Tokarev",                    Shortcut::NONE,     nullptr);

            if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
                EditorUI::FileMenuNode& locations = g_fileMenu.AddMenuNode("Locations", Shortcut::NONE, nullptr);
                locations.AddChild("House", Shortcut::NONE, Callbacks::BeginAddingHouse);
                locations.AddChild("Player Spawn (Campaign)",   Shortcut::NONE, Callbacks::BeginAddingPlayerCampaignSpawn);
                locations.AddChild("Player Spawn (Deathmatch)", Shortcut::NONE, Callbacks::BeginAddingPlayerDeathMatchSpawn);
            }

            if (GetEditorMode() == EditorMode::HOUSE_EDITOR) {
                EditorUI::FileMenuNode& build = g_fileMenu.AddMenuNode("Build", Shortcut::NONE, nullptr);
                build.AddChild("Floor", Shortcut::NONE, Editor::PlaceHousePlane, HousePlaneType::FLOOR);
                build.AddChild("Wall", Shortcut::NONE, Editor::SetEditorState, EditorState::PLACE_WALL);
            }
        }

        //EditorUI::FileMenuNode& run = g_fileMenu.AddMenuNode("Run");
        //run.AddChild("New Run", nullptr, "F1");
    }

    void CreateFileMenuImGuiElements() {
        g_fileMenu.CreateImguiElements();
    }
}
