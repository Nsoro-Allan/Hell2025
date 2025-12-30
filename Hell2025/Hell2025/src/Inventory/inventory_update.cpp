#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Bible/Bible.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Renderer/Renderer.h"
#include "Util.h"

void Inventory::Update(float deltaTime) {
    //if (Input::KeyPressed(HELL_KEY_M)) {
    //    SetGridCountX(6);
    //}
    //if (Input::KeyPressed(HELL_KEY_7)) {
    //    MoveItem(5, 4, 2, true);
    //}

    //SetGridCountX(6);
    ////SetGridCountY(4);
    //MoveItem(5, 4, 2, true);

    if (m_state == InventoryState::MAIN_SCREEN) UpdateItemViewScreen(deltaTime);
    if (m_state == InventoryState::SHOP) UpdateItemViewScreen(deltaTime);
    if (m_state == InventoryState::EXAMINE_ITEM) UpdateExamineScreen(deltaTime);
}

void Inventory::UpdateItemViewScreen(float deltaTime) {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;

    // WASD cell input
    if (player->PressedLeft(true))  StepDirection(-1, 0);
    if (player->PressedRight(true)) StepDirection(1, 0);
    if (player->PressedUp(true))    StepDirection(0, -1);
    if (player->PressedDown(true))  StepDirection(0, 1);

    //InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(GetSelectedItemName());

    // Buttons
    if (player->PressedInventoryExamine() && GetSelectedItemIndex() != -1) {
        InitMeshNodesFromSelectedItem();
        SetState(InventoryState::EXAMINE_ITEM);
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_examineRotationX = 0.0F;
        m_examineRotationY = 0.0f;
        m_examineZoom = 0.0f;
    }

    int cash = player->GetCash();
    int akPrice = 100;
    int spasPrice = 200;
    int goldenGlockPrice = 300;
    if (player->PressedInteract()) {

        if (GetSelectedItemName() == "AKS74U") {
            if (cash >= akPrice) {
                player->m_inventory.GiveWeapon("AKS74U");
                player->m_inventory.GiveAmmo("AKS74U", 300);
                player->SubtractCash(akPrice);
                player->LeaveShop();
            }
            else {
                player->m_typeWriter.DisplayText("Hold your seahorses");
            }
            InputMulti::ClearKeyStates();
        }

        if (GetSelectedItemName() == "SPAS") {
            if (cash >= spasPrice) {
                player->m_inventory.GiveWeapon("SPAS");
                player->m_inventory.GiveAmmo("SPAS", 8);
                player->SubtractCash(spasPrice);
                player->LeaveShop();
            }
            else {
                player->m_typeWriter.DisplayText("Hold your seahorses");
            }
            InputMulti::ClearKeyStates();
        }

        if (GetSelectedItemName() == "GoldenGlock") {
            if (cash >= goldenGlockPrice) {
                player->m_inventory.GiveWeapon("GoldenGlock");
                player->m_inventory.GiveAmmo("GoldenGlock", 200);
                player->SubtractCash(goldenGlockPrice);
                player->LeaveShop();
            }
            else {
                player->m_typeWriter.DisplayText("Hold your seahorses");
            }
            InputMulti::ClearKeyStates();
        }
    }
}

void Inventory::InitMeshNodesFromSelectedItem() {
    InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(GetSelectedItemName());
    if (!itemInfo) return;

    Bible::ConfigureMeshNodesByPickUpName(NO_ID, itemInfo->m_name, m_examineItemMeshNodes, false);

    //std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;
    //m_examineItemMeshNodes.Init(NO_ID, itemInfo->m_examineModelName, emptyMeshNodeCreateInfoSet);
    //
    //std::cout << "You initialized the inventory mesh nodes object to: " << itemInfo->m_examineModelName << "\n";
    //
    //
    //// Iterate through the mesh materials as defined in the bible and update the meshNodes them with them
    //for (const auto& pair : itemInfo->m_meshMaterialNames) {
    //    const std::string& meshName = pair.first;
    //    const std::string& materialName = pair.second;
    //    m_examineItemMeshNodes.SetMaterialByMeshName(meshName, materialName);
    //}

    // TODO: Use the bible properly for this!!!!!!!
    // TODO: Use the bible properly for this!!!!!!!
    // TODO: Use the bible properly for this!!!!!!!
    // TODO: Use the bible properly for this!!!!!!!
}

void Inventory::UpdateExamineScreen(float deltaTime) {
    InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(GetSelectedItemName());
    PickUpInfo* pickUpInfo = Bible::GetPickUpInfoByName(GetSelectedItemName());
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);

    if (!itemInfo) return;
    if (!pickUpInfo) return;
    if (!player) return;

    Model* model = AssetManager::GetModelByName(pickUpInfo->modelName);
    if (!model) {
        m_examineItemMeshNodes.CleanUp();
        return;
    }

    if (Input::KeyPressed(HELL_KEY_ENTER)) {
        Bible::Init();
    }

    float rotateSensitivity = 0.25f;
    float zoomSensitivity = 10.0f;
    float verticalRotationLimit = HELL_PI * 0.75f;

    float zoomMinLimit = 1.0f;
    float zoomMaxLimit = itemInfo->m_maxExamineZoom;

    // Rotate
    if (player->PressingFire()) {
        m_examineRotationX += InputMulti::GetMouseYOffset(player->m_mouseIndex) * deltaTime * rotateSensitivity;
        m_examineRotationY -= InputMulti::GetMouseXOffset(player->m_mouseIndex) * deltaTime * rotateSensitivity;
    }

    // Zoom
    if (Input::MouseWheelUp()) {
        m_examineZoom += deltaTime * zoomSensitivity;
    }
    if (Input::MouseWheelDown()) {
        m_examineZoom -= deltaTime * zoomSensitivity;
    }

    // Clamp vertical rotation
    m_examineRotationX = glm::clamp(m_examineRotationX, -verticalRotationLimit, verticalRotationLimit);

    // Clamp zoom rotation
    m_examineZoom = glm::clamp(m_examineZoom, zoomMinLimit, zoomMaxLimit);

    Transform rotX;
    rotX.rotation.x = m_examineRotationX;

    Transform rotY;
    rotY.rotation.y = m_examineRotationY;

    Transform zoomTransform;
    zoomTransform.scale = glm::vec3(m_examineZoom);

    const glm::vec3 aabbMin = model->GetAABBMin();
    const glm::vec3 aabbMax = model->GetAABBMax();
    const glm::vec3 center = 0.5f * (aabbMin + aabbMax);
    const glm::mat4 centeringMatrix = glm::translate(glm::mat4(1.0f), -center);

    Transform initialTransform;
    initialTransform.position = itemInfo->m_examineModelTranslation;
    initialTransform.rotation = itemInfo->m_examineModelRotation;
    initialTransform.scale = itemInfo->m_examineModelScale;

    glm::mat4 modelMatrix = rotX.to_mat4() * rotY.to_mat4() * initialTransform.to_mat4() * zoomTransform.to_mat4() * centeringMatrix;
    
    //Renderer::DrawItemExamineAABB(AABB(aabbMin, aabbMax), YELLOW);
    m_examineItemMeshNodes.Update(modelMatrix);
}

void Inventory::StepDirection(int dx, int dy) {
    Audio::PlayAudio(AUDIO_SELECT, 1.0f);

    int startX = m_selectedCellX;
    int startY = m_selectedCellY;
    int startIndex = InBounds(startX, startY) ? m_itemIndex2DArray[startX][startY] : -1;

    int x = (startX + dx + m_gridCountX) % m_gridCountX;
    int y = (startY + dy + m_gridCountY) % m_gridCountY;

    // Only skip contiguous cells if we STARTED on an item
    if (startIndex != -1) {
        // Skip all cells that belong to the same item
        while (m_itemIndex2DArray[x][y] == startIndex) {
            if (x == startX && y == startY) break; // safety
            x = (x + dx + m_gridCountX) % m_gridCountX;
            y = (y + dy + m_gridCountY) % m_gridCountY;
        }
    }

    m_selectedCellX = x;
    m_selectedCellY = y;
}
