#pragma once
#include "HellFunctionTypes.h"

namespace Callbacks {
    void NewMap(const std::string& filename);
    void OpenMap(const std::string& filename);
    void SaveMap(const std::string& filename);

    void NewHouse(const std::string& filename);
    void OpenHouse(const std::string& filename);
    void SaveHouse();

    void OpenHouseEditor();
    void OpenMapHeightEditor();
    void OpenMapObjectEditor();

    void NewRun();
    void QuitProgram();

    void BeginAddingDoor();
    void BeginAddingHouse();
    void BeginAddingPictureFrame();
    void BeginAddingTree(); 
    void BeginAddingBlackBerries();
    void BeginAddingWall();
    void BeginAddingWindow();
}