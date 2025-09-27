#pragma once
#include "Callbacks.h"
#include "BackEnd/BackEnd.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Managers/MapManager.h"
#include "World/SectorManager.h"
#include "World/World.h"

#include <iostream>

namespace Callbacks {
    void NewHeightMap(const std::string& filename) {
        int defaultChunkWidth = 8;
        int defaultChunkDepth = 16;
        float initialHeight = 30.0f;
        MapManager::NewMap(filename, defaultChunkWidth, defaultChunkDepth, initialHeight);
    }

    void NewHouse(const std::string& filename) {
        Logging::ToDo() << "TODO: NewHouse() callback: " << filename << "\n";
    }

    void NewMap(const std::string& filename) {
        Logging::ToDo() << "TODO: NewMap() callback: " << filename << "\n";
    }

    void OpenHouse(const std::string& filename) {
        Logging::ToDo() << "TODO: OpenHouse() callback: " << filename << "\n";
    }

    void SaveHouse() {
        World::SaveHouse();
    }

    void OpenMap(const std::string& filename) {
        MapManager::LoadMap(filename);
    }

    void SaveMap(const std::string& filename) {
        MapManager::SaveMap(filename);
    }

    void OpenHouseEditor() {
        Editor::OpenHouseEditor();
    }

    void OpenMapHeightEditor() {
        Editor::OpenMapHeightEditor();
        Logging::Debug() << "Opened map height editor";
    }

    void OpenMapObjectEditor() {
        Editor::OpenMapObjectEditor();
        Logging::Debug() << "Opened map object editor";
    }

    void NewRun() {
        World::NewRun();
    }

    void BeginAddingDoor() {
        Editor::SetEditorState(EditorState::PLACE_DOOR);
    }

    void BeginAddingHouse() {
        Editor::SetEditorState(EditorState::PLACE_HOUSE);
    }

    void BeginAddingPlayerCampaignSpawn() {
        Editor::SetEditorState(EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN);
    }

    void BeginAddingPlayerDeathMatchSpawn() {
        Editor::SetEditorState(EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN);
    }

    void BeginAddingPictureFrame() {
        Editor::SetEditorState(EditorState::PLACE_PICTURE_FRAME);
    }

    void BeginAddingTree() {
        Editor::SetPlantType(TreeType::TREE_LARGE_0);
        Editor::SetEditorState(EditorState::PLACE_TREE);
    }

    void BeginAddingBlackBerries() {
        Editor::SetPlantType(TreeType::BLACK_BERRIES);
        Editor::SetEditorState(EditorState::PLACE_TREE);
    }

    void BeginAddingWall() {
        Editor::SetEditorState(EditorState::PLACE_WALL);
    }

    void BeginAddingWindow() {
        Editor::SetEditorState(EditorState::PLACE_WINDOW);
    }

    void QuitProgram() {
        BackEnd::ForceCloseWindow();
    }
}