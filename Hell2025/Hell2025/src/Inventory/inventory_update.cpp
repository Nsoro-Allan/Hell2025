#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Core/Bible.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"


void Inventory::Update(float deltaTime) {
    if (m_state == InventoryState::MAIN_SCREEN) UpdateItemViewScreen(deltaTime);
    if (m_state == InventoryState::EXAMINE_ITEM) UpdateExamineScreen(deltaTime);
}


void Inventory::UpdateItemViewScreen(float deltaTime) {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;

    // WASD cell input
    if (player->PressedLeft()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellX--;
    }
    if (player->PressedRight()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellX++;
    }
    if (player->PressedUp()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellY--;
    }
    if (player->PressedDown()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellY++;
    }

    // Wrap check Left
    if (m_selectedCellX >= m_gridCountX) {
        m_selectedCellX = 0;
    }
    // Wrap check Right
    if (m_selectedCellX < 0) {
        m_selectedCellX = m_gridCountX - 1;
    }
    // Wrap check bottom
    if (m_selectedCellY >= MAX_INVENTORY_Y_SIZE) {
        m_selectedCellY = 0;
    }
    // Wrap check top
    if (m_selectedCellY < 0) {
        m_selectedCellY = MAX_INVENTORY_Y_SIZE - 1;
    }

    // Buttons
    if (player->PressedInventoryExamine()) {
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
        m_examineItemMeshNodes.InitFromModel(itemInfo->m_examineModelName);

        // Calculate matrix to scale the model to fit on screen nicely
        float maxXZ = 2.0f;         // Desired max footprint on XZ in meters
        float maxY = maxXZ * 0.5f;  // Desired max height in meters
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
        m_examineItemMeshNodes.SetGoldFlag(itemInfo->m_isGold);
        m_examineItemMeshNodes.UpdateRenderItems(finalScaledAndCenteredModelMatrix);
    }
}