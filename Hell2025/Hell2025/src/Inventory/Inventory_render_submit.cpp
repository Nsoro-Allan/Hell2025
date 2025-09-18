#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Bible.h"
#include "Input/Input.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"


void Inventory::SubmitRenderItems() {
    if (m_state == InventoryState::MAIN_SCREEN) {
        int invOriginX = 100;
        int invOriginY = 100;
        int gridMargin = 49;
        int gridOriginX = invOriginX + gridMargin;
        int gridOriginY = invOriginY + gridMargin;
        int invWidth = (GetCellSizeInPixels() * m_gridCountX) + gridMargin * 2;
        int invHeight = 880;

        BlitInventoryBackground(invOriginX, invOriginY, invWidth, invHeight);
        BlitGrid(gridOriginX, gridOriginY);
        SubmitItemViewScreenRenderItems();
    }
    if (m_state == InventoryState::EXAMINE_ITEM) SubmitItemExamineRenderItems();
}

void Inventory::SubmitItemExamineRenderItems() {
    // Render item inspect item
}

void Inventory::BlitInventoryBackground(int originX, int originY, int width, int height) {
    Texture* bgTexture = AssetManager::GetTextureByName("inv_background");
    Texture* borderTexture = AssetManager::GetTextureByName("inv_border");
    Texture* cornerTexture = AssetManager::GetTextureByName("inv_border_corner");

    if (!bgTexture) return;
    if (!borderTexture) return;
    if (!cornerTexture) return;

    // Clip rect in top left origin pixels
    int clipMinX = originX;
    int clipMinY = originY;
    int clipMaxX = originX + width;
    int clipMaxY = originY + height;

    // Blit background
    const int bgWidth = std::max(1, bgTexture->GetWidth());
    const int bgHeight = std::max(1, bgTexture->GetHeight());
    const int bgCellCountX = (width + bgWidth - 1) / bgWidth;
    const int bgCellCountY = (height + bgHeight - 1) / bgHeight;

    for (int x = 0; x < bgCellCountX; ++x) {
        for (int y = 0; y < bgCellCountY; ++y) {
            BlitTextureInfo info{};
            info.textureName = bgTexture->GetFileName();
            info.location = glm::ivec2(originX + (x * bgWidth), originY + (y * bgHeight));
            info.alignment = Alignment::TOP_LEFT;
            info.textureFilter = TextureFilter::LINEAR;
            info.clipMinX = clipMinX;
            info.clipMinY = clipMinY;
            info.clipMaxX = clipMaxX;
            info.clipMaxY = clipMaxY;
            UIBackEnd::BlitTexture(info);
        }
    }

    // Blit border
    const int borderWidth = std::max(1, borderTexture->GetWidth());
    const int borderHeight = std::max(1, borderTexture->GetHeight());
    const int borderCellCountX = (width + borderWidth - 1) / borderWidth;
    const int borderCellCountY = (height + borderHeight - 1) / borderHeight;

    for (int x = 0; x < borderCellCountX; ++x) {
        BlitTextureInfo info{};
        info.textureName = borderTexture->GetFileName();
        info.textureFilter = TextureFilter::LINEAR;
        info.clipMinX = clipMinX;
        info.clipMinY = clipMinY;
        info.clipMaxX = clipMaxX;
        info.clipMaxY = clipMaxY;

        // Top
        info.location = glm::ivec2(originX + (x * borderWidth), originY);
        info.alignment = Alignment::TOP_LEFT;
        UIBackEnd::BlitTexture(info);

        // Bottom
        info.location = glm::ivec2(originX + (x * borderWidth) + width, originY + height);
        info.alignment = Alignment::TOP_LEFT;
        info.rotation = HELL_PI;
        UIBackEnd::BlitTexture(info);
    }

    for (int y = 0; y < borderCellCountY; ++y) {
        BlitTextureInfo info{};
        info.textureName = borderTexture->GetFileName();
        info.textureFilter = TextureFilter::LINEAR;
        info.clipMinX = clipMinX;
        info.clipMinY = clipMinY;
        info.clipMaxX = clipMaxX;
        info.clipMaxY = clipMaxY;

        // Left
        info.location = glm::ivec2(originX, originY - (y * borderWidth) + height);
        info.alignment = Alignment::TOP_LEFT;
        info.rotation = HELL_PI * -0.5f;
        UIBackEnd::BlitTexture(info);

        // Right
        info.location = glm::ivec2(originX + width, originY + (y * borderWidth));
        info.alignment = Alignment::TOP_LEFT;
        info.rotation = HELL_PI * 0.5f;
        UIBackEnd::BlitTexture(info);
    }

    // Top left corner
    BlitTextureInfo info{};
    info.textureName = cornerTexture->GetFileName();
    info.textureFilter = TextureFilter::LINEAR;
    info.clipMinX = clipMinX;
    info.clipMinY = clipMinY;
    info.clipMaxX = clipMaxX;
    info.clipMaxY = clipMaxY;
    info.location = glm::ivec2(originX, originY);
    info.alignment = Alignment::TOP_LEFT;
    info.rotation = 0.0f;
    UIBackEnd::BlitTexture(info);

    // Top right
    info.location = glm::ivec2(originX + width, originY);
    info.alignment = Alignment::TOP_LEFT;
    info.rotation = HELL_PI * 0.5f;
    UIBackEnd::BlitTexture(info);

    // Bottom left
    info.location = glm::ivec2(originX, originY + height);
    info.alignment = Alignment::TOP_LEFT;
    info.rotation = HELL_PI * 1.5f;
    UIBackEnd::BlitTexture(info);

    // Bottom right
    info.location = glm::ivec2(originX + width, originY + height);
    info.alignment = Alignment::TOP_LEFT;
    info.rotation = HELL_PI;
    UIBackEnd::BlitTexture(info);
}

void Inventory::BlitGrid(int originX, int originY) {
    Texture* squareSize1Texture = AssetManager::GetTextureByName("InvSquare_Size1");
    Texture* squareSize2Texture = AssetManager::GetTextureByName("InvSquare_Size2");
    Texture* squareSize3Texture = AssetManager::GetTextureByName("InvSquare_Size3");
    Texture* squareSize1SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size1Selected");
    Texture* squareSize2SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size2Selected");
    Texture* squareSize3SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size3Selected");

    if (!squareSize1Texture) return; 
    if (!squareSize2Texture) return; 
    if (!squareSize3Texture) return; 
    if (!squareSize1SelectedTexture) return; 
    if (!squareSize2SelectedTexture) return; 
    if (!squareSize3SelectedTexture) return;

    int cellSizeInPixels = GetCellSizeInPixels();

    // Render background squares (size 1)
    for (int x = 0; x < m_gridCountX; x++) {
        for (int y = 0; y < m_gridCountY; y++) {
            BlitTextureInfo blitTextureInfo;
            blitTextureInfo.textureName = squareSize1Texture->GetFileName();
            blitTextureInfo.location.x = originX + (cellSizeInPixels * x);
            blitTextureInfo.location.y = originY + (cellSizeInPixels * y);
            blitTextureInfo.alignment = Alignment::TOP_LEFT;
            blitTextureInfo.textureFilter = TextureFilter::LINEAR;
            UIBackEnd::BlitTexture(blitTextureInfo);
        }
    }

    // Render bigger background squares for items that need it (size 2 & 3)
    for (InventoryItem& item : m_items) {
        int itemSize = Bible::GetInventoryItemSizeByName(item.m_name);
        if (itemSize <= 1) continue;

        BlitTextureInfo blitTextureInfo;
        blitTextureInfo.textureName = (itemSize == 2 ? squareSize2Texture->GetFileName() : squareSize3Texture->GetFileName());
        blitTextureInfo.location.x = originX + (cellSizeInPixels * item.m_gridLocation.x);
        blitTextureInfo.location.y = originY + (cellSizeInPixels * item.m_gridLocation.y);
        blitTextureInfo.alignment = Alignment::TOP_LEFT;
        blitTextureInfo.textureFilter = TextureFilter::LINEAR;
        blitTextureInfo.rotation = 0.0f;

        if (item.m_rotatedInGrid) {
            blitTextureInfo.rotation = HELL_PI * -0.5f;
            blitTextureInfo.location.y += ((itemSize - 1) * cellSizeInPixels) - cellSizeInPixels;
        }

        UIBackEnd::BlitTexture(blitTextureInfo);
    }

    // Render the selected cell
    if (InBounds(m_selectedCellX, m_selectedCellY)) {
        int itemIndex = m_itemIndex2DArray[m_selectedCellX][m_selectedCellY];

        BlitTextureInfo blitTextureInfo;
        blitTextureInfo.alignment = Alignment::TOP_LEFT;
        blitTextureInfo.textureFilter = TextureFilter::LINEAR;

        if (itemIndex != -1 && itemIndex < (int)m_items.size()) {
            // Selection sits on an item, so snap to the item's anchor and use matching selected texture
            InventoryItem& item = m_items[itemIndex];
            int itemSize = Bible::GetInventoryItemSizeByName(item.m_name);

            blitTextureInfo.textureName = (itemSize == 1) ? squareSize1SelectedTexture->GetFileName() :(itemSize == 2) ? squareSize2SelectedTexture->GetFileName() : squareSize3SelectedTexture->GetFileName();
            blitTextureInfo.location.x = originX + (cellSizeInPixels * item.m_gridLocation.x);
            blitTextureInfo.location.y = originY + (cellSizeInPixels * item.m_gridLocation.y);
            blitTextureInfo.rotation = 0.0f;

            if (item.m_rotatedInGrid) {
                blitTextureInfo.rotation = HELL_PI * -0.5f;
                blitTextureInfo.location.y += ((itemSize - 1) * cellSizeInPixels) - cellSizeInPixels;
            }
        }
        else {
            // For empty cells, use the single size 1 selected highlight
            blitTextureInfo.textureName = squareSize1SelectedTexture->GetFileName();
            blitTextureInfo.location.x = originX + (cellSizeInPixels * m_selectedCellX);
            blitTextureInfo.location.y = originY + (cellSizeInPixels * m_selectedCellY);
        }

        UIBackEnd::BlitTexture(blitTextureInfo);
    }
}

void Inventory::SubmitItemViewScreenRenderItems() {

    Texture* squareSize1Texture = AssetManager::GetTextureByName("InvSquare_Size1");
    Texture* squareSize2Texture = AssetManager::GetTextureByName("InvSquare_Size2");
    Texture* squareSize3Texture = AssetManager::GetTextureByName("InvSquare_Size3");
    Texture* squareSize1SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size1Selected");
    Texture* squareSize2SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size2Selected");
    Texture* squareSize3SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size3Selected");

    if (!squareSize1Texture) return;
    if (!squareSize2Texture) return;
    if (!squareSize3Texture) return;
    if (!squareSize1SelectedTexture) return;
    if (!squareSize2SelectedTexture) return;
    if (!squareSize3SelectedTexture) return;


    int iconWidth = squareSize1Texture->GetWidth();

    int iconMarin = 49;
    int invOriginX = 100;
    int invOriginY = 100;
    int invWidth = (iconWidth * m_gridCountX) + iconMarin * 2;
    int invHeight = 880;

    int cellWidth = squareSize1Texture->GetWidth();
    int cellHeight = squareSize1Texture->GetHeight();

    glm::ivec2 origin = glm::ivec2(100, 100);
    glm::ivec2 size = glm::ivec2(474, 855);


    // Squares
    glm::ivec2 squaresOrigin = origin + glm::ivec2(iconMarin, iconMarin);

    // Icons
    for (InventoryItem& item : m_items) {
        // Lookup texture name based on item name + prefix
        std::string textureName = "InvItem_" + item.m_name;
        Texture* texture = AssetManager::GetTextureByName(textureName);
        if (!texture) {
            std::cout << "Could not render inventory icon for '" << item.m_name << "' coz ;" << textureName << "' was not found\n";
            continue;
        }

        InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(item.m_name);
        if (!itemInfo) continue;

        glm::ivec2 itemLocation;
        itemLocation.x = squaresOrigin.x + (cellWidth * item.m_gridLocation.x);
        itemLocation.y = squaresOrigin.y + (cellHeight * item.m_gridLocation.y);

        // Render icon
        BlitTextureInfo blitTextureInfo;
        blitTextureInfo.textureName = textureName;
        blitTextureInfo.location = itemLocation;
        blitTextureInfo.alignment = Alignment::TOP_LEFT;
        blitTextureInfo.textureFilter = TextureFilter::LINEAR;

        if (item.m_rotatedInGrid) {
            blitTextureInfo.rotation = HELL_PI * -0.5f;
            blitTextureInfo.location.y += GetCellSizeInPixels();
        }

        UIBackEnd::BlitTexture(blitTextureInfo);
    }

    // Text
    int marginX = squaresOrigin.x;
    int headingPadding = 30;
    int theLineY = squaresOrigin.y + (cellHeight * m_gridCountY) + headingPadding;
    int headingY = theLineY + 32;
    int descriptionY = headingY + 50;



    // The line
    BlitTextureInfo lineInfo;
    lineInfo.textureName = "inv_the_line";
    lineInfo.location = { marginX, theLineY };
    lineInfo.alignment = Alignment::TOP_LEFT;
    lineInfo.textureFilter = TextureFilter::LINEAR;
    UIBackEnd::BlitTexture(lineInfo);

    InventoryItemInfo* itemInfo = GetSelectedItemInfo();

    // If you have an item selected, then render the name and description and buttons
    if (itemInfo) {
        std::string name = itemInfo->m_itemHeading;
        std::string description = itemInfo->m_description;


        UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + name, "BebasNeue", marginX, headingY, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);
        UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + description, "RobotoCondensed", marginX, descriptionY, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);

        glm::ivec2 descriptonSize = TextBlitter::GetBlitTextSize(description, "RobotoCondensed", 1.0f);

        int thePlaceholderY = descriptionY + descriptonSize.y;

        int buttonPaddingY = 35;
        int buttonMarginX = marginX; // maybe you wanna indent this?
        int buttonOriginY = descriptionY + descriptonSize.y + buttonPaddingY;
        int buttonLineHeight = 31;

        if (itemInfo->m_equipable) {
            RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "E", "Equip");
            buttonOriginY += buttonLineHeight;
        }

        if (itemInfo->m_combineable) {
            RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "C", "Combine");
            buttonOriginY += buttonLineHeight;
        }

        if (true) { // Everything is examinable
            RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "F", "Examine");
            buttonOriginY += buttonLineHeight;
        }

        if (true) { // Everything is examinable
            RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "M", "Move");
            buttonOriginY += buttonLineHeight;
        }

        if (itemInfo->m_discardable) {
            RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "G", "Discard");
            buttonOriginY += buttonLineHeight;
        }
    }
    
    //if (false) {
    //    UIBackEnd::BlitTexture("White", origin, Alignment::TOP_LEFT, ORANGE, glm::ivec2(1, size.y), TextureFilter::LINEAR);
    //    UIBackEnd::BlitTexture("White", origin, Alignment::TOP_LEFT, ORANGE, glm::ivec2(size.x, 1), TextureFilter::LINEAR);
    //    UIBackEnd::BlitTexture("White", origin + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(1, size.y), TextureFilter::LINEAR);
    //    UIBackEnd::BlitTexture("White", origin + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(size.x, 1), TextureFilter::LINEAR);
    //}
}


void Inventory::RenderButton(glm::ivec2 location, const std::string& letter, const std::string& description) {
    Texture* buttonTexture = AssetManager::GetTextureByName("inventory_green_button");
    if (!buttonTexture) return;

    int buttonCenterX = location.x + (buttonTexture->GetWidth() * 0.5f);
    int descriptionLeftY = buttonCenterX + 19;

    UIBackEnd::BlitTexture("inventory_green_button", glm::ivec2(buttonCenterX, location.y), Alignment::CENTERED, WHITE, glm::ivec2(-1, -1), TextureFilter::LINEAR);
    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + letter, "RobotoCondensed", buttonCenterX, location.y + 2, Alignment::CENTERED, 0.75f, TextureFilter::LINEAR);
    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + description, "RobotoCondensed", descriptionLeftY, location.y + 2, Alignment::CENTERED_VERTICAL, 1.0f, TextureFilter::LINEAR);

}
