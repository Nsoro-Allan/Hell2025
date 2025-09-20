#pragma once
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Types/Renderer/MeshNodes.h"
#include <string>
#include <vector>

struct InventoryItem {
    std::string m_name = UNDEFINED_STRING;
    glm::ivec2 m_gridLocation = glm::ivec2(-1, -1);
    std::vector<std::string> m_attachments;
    int m_ammoInMag = 0;
    bool m_rotatedInGrid = false;
};

struct InventoryStyle {
    int invOriginX = 100;
    int invOriginY = 100;
    int gridMargin = 49;
    int theLinePadding = 30;
    int itemHeadingTopPadding = 32;
    int itemDescriptionTopPadding = 18;
    int itemButtonsTopPadding = 35;
    int itemButtonLineHeight = 31;
    std::string itemHeadingFont = "BebasNeue";
    std::string itemDescriptionFont = "RobotoCondensed";
};

struct InventoryLocations {
    glm::ivec2 background = {};
    glm::ivec2 itemGrid = {};
    glm::ivec2 itemHeading = {};
    glm::ivec2 itemDescription = {};
    glm::ivec2 itemButtons = {};
    glm::ivec2 theLine = {};
};

struct Inventory {
    Inventory();
    void Update(float deltaTime);
    void AddItem(const std::string& name);
    void ClearInventory();
    void OpenInventory();
    void CloseInventory();
    void SubmitRenderItems();
    void PrintGridOccupiedStateToConsole();
    void GoToMainScreen();
    void SetLocalPlayerIndex(int localPlayerIndex);
    void SetGridCountX(int count);
    void SetGridCountY(int count);

    const int GetGridCountX()                       { return m_gridCountX; }
    const int GetGridCountY()                       { return m_gridCountY; }
    const glm::mat4 GetItemExamineModelMatrix()     { return m_examineModelMatrix; }
    const InventoryState& GetInventoryState()       { return m_state; }
    const std::vector<InventoryItem>& GetItems()    { return m_items; }
    const std::vector<RenderItem>& GetRenderItems() { return m_examineItemMeshNodes.GetRenderItems(); }
    const bool IsClosed()                           { return m_state == InventoryState::CLOSED; }
    const bool IsOpen()                             { return m_state != InventoryState::CLOSED; }

private:
    glm::ivec2 GetNextFreeLocation(int itemCellSize);
    void UpdateOccupiedSlotsArray(); // rename this to reflect the actual name of the array: m_itemIndex2DArray
    void RenderButton(glm::ivec2 location, const std::string& letter, const std::string& description);
    void SetState(InventoryState state);
    bool MoveItem(int itemIndex, int cellX, int cellY, bool rotated);

    // Render submits
    void BlitTheLine(glm::ivec2 origin);
    void BlitItemGrid(glm::ivec2 origin);
    void BlitItemHeading(glm::ivec2 origin);
    void BlitItemDescription(glm::ivec2 origin);
    void BlitItemButtons(glm::ivec2 origin);

    void SubmitItemExamineRenderItems();

    // Util
    void StepDirection(int dx, int dy);
    void BlitInventoryBackground(glm::ivec2 origin, int width, int height);
    
    // Updates
    void UpdateItemViewScreen(float deltaTime);
    void UpdateExamineScreen(float deltaTime);

    // Getters
    InventoryItemInfo* GetSelectedItemInfo();
    InventoryItem* GetItemAtIndex(int index);
    glm::ivec2 GetSelectedItemHeadingSize();
    glm::ivec2 GetSelectedItemDescriptionSize();
    glm::ivec2 GetItemGridSize();
    int GetCellSizeInPixels();
    int GetSelectedItemIndex();
    int GetItemSizeAtLocation(int x, int y);
    bool IsCellOccupied(int x, int y);
    bool InBounds(int x, int y);
    bool ItemSelected();
    const std::string& GetSelectedItemName();
    const std::string& GetItemNameAtLocation(int x, int y);
    const std::string& GetSelectedItemHeading();
    const std::string& GetSelectedItemDescription();

    int m_itemIndex2DArray[MAX_INVENTORY_X_SIZE][MAX_INVENTORY_Y_SIZE]; // Updated on insertion and move
    int m_gridCountX = 4;
    int m_gridCountY = 4;
    int m_selectedCellX = 0;
    int m_selectedCellY = 0;
    int m_localPlayerIndex = 0;
    std::vector<InventoryItem> m_items;
    InventoryState m_state = InventoryState::CLOSED;
    float m_examineRotationX = 0.0f;
    float m_examineRotationY = 0.0f;
    float m_examineZoom = 0.0f;
    glm::mat4 m_examineModelMatrix;
    MeshNodes m_examineItemMeshNodes;
    InventoryStyle m_style;
    InventoryLocations m_locations;
};
