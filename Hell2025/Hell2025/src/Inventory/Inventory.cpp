#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Bible.h"
#include "Input/Input.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"

Inventory::Inventory() {
    //UpdateOccupiedSlotsArray();
}

void Inventory::OpenInventory() {
    GoToMainScreen();
}

void Inventory::GoToMainScreen() {
    SetState(InventoryState::MAIN_SCREEN);
    m_selectedCellX = 0;
    m_selectedCellY = 0;
    m_examineRotationX = 0.0f;
    m_examineRotationY = 0.0f;
    m_examineZoom = 0.0f;
}

void Inventory::CloseInventory() {
    SetState(InventoryState::CLOSED);
}

void Inventory::AddItem(const std::string& name) {
    InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(name);
    if (!itemInfo) return;

    //std::cout << "\n";
    //std::cout << "Adding: " << name << "\n";
    //std::cout << "- cell size: " << itemInfo->m_cellSize << "\n";
;   //PrintGridOccupiedStateToConsole();

    // Find the next free location
    glm::ivec2 nextFreeLocation = GetNextFreeLocation(itemInfo->m_cellSize);
    
    // Oh there wasn't one?
    if (nextFreeLocation == glm::ivec2(-1, -1)) {
        std::cout << "NO FREE SPACE FOR ITEM '" << name << "'\n";
    }

    //std::cout << "- next free location: " << nextFreeLocation.x << ", " << nextFreeLocation.y << "\n";


    // Yay, there was, insert it!
    InventoryItem item;
    item.m_name = name;
    item.m_gridLocation = nextFreeLocation;
    m_items.push_back(item);

    UpdateOccupiedSlotsArray();
}

void Inventory::SetLocalPlayerIndex(int localPlayerIndex) {
    m_localPlayerIndex = localPlayerIndex;
}

void Inventory::SetState(InventoryState state) {
    m_state = state;
}

void Inventory::ClearInventory() {
    m_items.clear();
    UpdateOccupiedSlotsArray();
}

int Inventory::GetSelectedItemIndex() {
    return m_itemIndex2DArray[m_selectedCellX][m_selectedCellY];
}


const std::string& Inventory::GetSelectedItemName() {
    static std::string noItem = "NO_ITEM";

    InventoryItem* selectedItem = GetItemAtIndex(GetSelectedItemIndex());
    if (!selectedItem) return noItem;

    return selectedItem->m_name;
}

InventoryItem* Inventory::GetItemAtIndex(int index) {
    if (index < 0 || index >= m_items.size()) return nullptr;
    else return &m_items[index];
}

InventoryItemInfo* Inventory::GetSelectedItemInfo() {
    int selectedItemIndex = GetSelectedItemIndex();
    if (selectedItemIndex == -1) return nullptr; // nothing at this slot

    InventoryItem* selectedItem = GetItemAtIndex(selectedItemIndex);
    if (!selectedItem) return nullptr; // This should never happen

    return Bible::GetInventoryItemInfoByName(selectedItem->m_name);
}

const std::string& Inventory::GetItemNameAtLocation(int x, int y) {
    static std::string noItem = "NO_ITEM";
    int itemIndex = m_itemIndex2DArray[x][y];
    if (itemIndex == -1) {
        return noItem;
    }
    else {
        return m_items[itemIndex].m_name;
    }
}

int Inventory::GetItemSizeAtLocation(int x, int y) {
    const std::string& itemName = GetItemNameAtLocation(x, y);
    int itemSize = 1;
    if (itemName != "NO_ITEM") {
        itemSize = Bible::GetInventoryItemSizeByName(itemName);
    }
    return itemSize;
}

bool Inventory::IsCellOccupied(int x, int y) {
    return m_itemIndex2DArray[x][y] != -1;
}

void Inventory::UpdateOccupiedSlotsArray() {
    // Initilize to empty
    for (int x = 0; x < MAX_INVENTORY_X_SIZE; x++) {
        for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
            m_itemIndex2DArray[x][y] = -1;
        }
    }

    // Iterate your inventory and mark which slots are occupied
    for (int i = 0; i < m_items.size(); i++) {
        InventoryItem& item = m_items[i];
        InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(item.m_name);
        if (!itemInfo) continue;

        m_itemIndex2DArray[item.m_gridLocation.x][item.m_gridLocation.y] = i;

        if (itemInfo->m_cellSize > 1) {
            m_itemIndex2DArray[item.m_gridLocation.x + 1][item.m_gridLocation.y] = i;
        }
        if (itemInfo->m_cellSize > 2) {
            m_itemIndex2DArray[item.m_gridLocation.x + 2][item.m_gridLocation.y] = i;
        }
    }
}

glm::ivec2 Inventory::GetNextFreeLocation(int itemCellSize) {
    UpdateOccupiedSlotsArray();

    // First scan for a non-rotated slot
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {

            //if (x == 3 && y == 3) continue;

            // Skip fat items
            if (itemCellSize > 1) {
                if (x + 1 >= m_gridCountX)     continue; // range check
                if (IsCellOccupied(x + 1, y))  continue; // cell check
            }
            if (itemCellSize > 2) {
                if (x + 2 >= m_gridCountX)     continue; // range check
                if (IsCellOccupied(x + 2, y))  continue; // cell check
            }
            // Free slot found
            if (!IsCellOccupied(x,y)) {
                return glm::ivec2(x, y);
            }
        }
    }
    // Now scan for a rotated slot
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {
            // TODO
        }
    }

    // No space available
    return glm::ivec2(-1, -1);
}

void Inventory::PrintGridOccupiedStateToConsole() { 
    std::cout << " ";
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {
            std::cout << "[";
            if (m_itemIndex2DArray[x][y]) {
                std::cout << "X";
            }
            else {
                std::cout << " ";
            }
            std::cout << "]";
        }
        std::cout << "\n ";
    }
}

//InventoryItem* Inventory::GetSelectedItem() {
//    if (m_selectedItem)
//}