#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Bible/Bible.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Renderer/Renderer.h"

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
    if (m_state == InventoryState::EXAMINE_ITEM) UpdateExamineScreen(deltaTime);

    if (Input::KeyPressed(HELL_KEY_SPACE)) {
        PrintGridOccupiedStateToConsole();
    }
}

void Inventory::UpdateItemViewScreen(float deltaTime) {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;

    // WASD cell input
    if (player->PressedLeft(true))  StepDirection(-1, 0);
    if (player->PressedRight(true)) StepDirection(1, 0);
    if (player->PressedUp(true))    StepDirection(0, -1);
    if (player->PressedDown(true))  StepDirection(0, 1);

    // Buttons
    if (player->PressedInventoryExamine() && GetSelectedItemIndex() != -1) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        SetState(InventoryState::EXAMINE_ITEM);
    }
}

void Inventory::UpdateExamineScreen(float deltaTime) {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;

    float rotateSensitivity = 0.25f;
    float zoomSensitivity = 10.0f;

    float verticalRotationLimit = HELL_PI * 0.75f;
    float zoomMinLimit = 1.0f;
    float zoomMaxLimit = 2.0f;

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

    Transform scale;
    scale.scale = glm::vec3(m_examineZoom);

    InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(GetSelectedItemName());
    if (itemInfo) {
        Model* model = AssetManager::GetModelByName(itemInfo->m_examineModelName);
        if (!model) {
            m_examineItemMeshNodes.CleanUp();
            return;
        }

        // Move me to the bible when u find the values!
        Transform initialTransform = itemInfo->m_initialExamineItemTransform;
        // initialRotTransform.rotation.y = -0.2f;
        // initialRotTransform.rotation.z = -0.6f;

        m_examineModelMatrix = rotX.to_mat4() * rotY.to_mat4() * initialTransform.to_mat4() * scale.to_mat4();

        // Update the mesh nodes from the model. WARNNG: YOu dont wanna do this every frame like you are currently!

        std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;
        m_examineItemMeshNodes.Init(NO_ID, itemInfo->m_examineModelName, emptyMeshNodeCreateInfoSet);

        // Calculate matrix to scale the model to fit on screen nicely
        float maxXZ = 2.0f; // Desired max footprint on XZ in meters
        float maxY = 1.0f;  // Desired max height in meters
        glm::vec3 aabbMin = model->GetAABBMin();
        glm::vec3 aabbMax = model->GetAABBMax();
        glm::vec3 extent = aabbMax - aabbMin;
        float exXZ = std::max(extent.x, extent.z);
        float exY = extent.y;
        float sXZ = exXZ > 0.0f ? (maxXZ / exXZ) : 1e9f;
        float sY = exY > 0.0f ? (maxY / exY) : 1e9f;
        float s = std::min(sXZ, sY);
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(s));

        // Calculate matrix to center the model on screen
        glm::vec3 center = 0.5f * (aabbMin + aabbMax);
        glm::mat4 centeringMatrix = glm::translate(glm::mat4(1.0f), -center);

        // Mash all the matrices together
        glm::mat4 finalScaledAndCenteredModelMatrix = m_examineModelMatrix * scalingMatrix * centeringMatrix;

        // Iterate through the mesh materials as defined in the bible and update the meshNodes them with them
        for (const auto& pair : itemInfo->m_meshMaterialNames) {
            const std::string& meshName = pair.first;
            const std::string& materialName = pair.second;
            m_examineItemMeshNodes.SetMaterialByMeshName(meshName, materialName);
        }

        // Actually create the render items for each mesh
        m_examineItemMeshNodes.Update(finalScaledAndCenteredModelMatrix);
    }
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
