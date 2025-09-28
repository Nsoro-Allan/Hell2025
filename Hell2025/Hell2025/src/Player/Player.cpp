#include "Player.h"
#include "HellDefines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Audio/Audio.h"
#include "BackEnd/BackEnd.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Ocean/Ocean.h"
#include "Viewport/ViewportManager.h"
#include "UniqueID.h"

// Get me out of here
#include "World/World.h"
// Get me out of here

void Player::Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex) {
    m_playerId = UniqueID::GetNext();

    m_camera.SetPosition(position + glm::vec3(0.0f, m_viewHeightStanding, 0.0f));
    m_camera.SetEulerRotation(rotation);
    m_viewportIndex = viewportIndex;

    m_inventory.SetLocalPlayerIndex(m_viewportIndex);

    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    viewWeapon->SetExclusiveViewportIndex(viewportIndex);

    AnimatedGameObject* characterModel = GetCharacterModelAnimatedGameObject();

    SpriteSheetObjectCreateInfo createInfo;
    createInfo.textureName = "MuzzleFlash_4x5";
    createInfo.loop = false;
    createInfo.billboard = true;
    createInfo.renderingEnabled = false;
    m_muzzleFlash.Init(createInfo);

    CreateCharacterController(position);
    InitCharacterModel();
    InitRagdoll();
}

void Player::BeginFrame() {
    m_interactFound = false;
    m_interactObjectId = 0;
    m_interactObjectType = ObjectType::NONE;
}

void Player::Update(float deltaTime) {
    m_moving = false;

    if (Editor::IsOpen()) {
        return;
    }

    // Toggle inventory
    if (PressedToggleInventory()) {

        // Was the inventory closed? Then open it
        if (m_inventory.IsClosed()) {
            m_inventory.OpenInventory();
        }
        else {
            // Viewing items in the main screen? well close it
            if (GetInvetoryState() == InventoryState::MAIN_SCREEN) {
                m_inventory.CloseInventory();
            }
            // Examining an item? Well return to main screen
            if (GetInvetoryState() == InventoryState::EXAMINE_ITEM) {
                m_inventory.GoToMainScreen();
            }
        }
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
    }

    // This may break code elsewhere in the player logic like anywhere
    if (m_inventory.IsOpen()) {
        DisableControl();
        m_inventory.Update(deltaTime);
    }

    // Inside or outside?
    glm::vec3 rayOrigin = GetCameraPosition();
    glm::vec3 rayDirection = glm::vec3(0.0f, -1.0f, 0.0f);
    float rayLength = 100.0f;
    PhysXRayResult rayResult = Physics::CastPhysXRayStaticEnvironment(rayOrigin, rayDirection, rayLength);
    m_feetAboveHeightField = (rayResult.hitFound && rayResult.userData.physicsType == PhysicsType::HEIGHT_FIELD);

    // Running
    m_running = PressingRun() && !m_crouching;

    m_runningSpeed = 20;

    // Respawn
    if (IsAwaitingSpawn()) Respawn();
    if (IsDead() && m_timeSinceDeath > 3.25) {
        if (PressedFire() ||
            PressedReload() ||
            PressedCrouch() ||
            PressedInteract() ||
            PressingJump() ||
            PressedNextWeapon()) {
            Respawn();
            Audio::PlayAudio("RE_Beep.wav", 0.5);
        }
    }
 
    UpdateMovement(deltaTime);
    UpdateHeadBob(deltaTime);
    UpdateBreatheBob(deltaTime);
    UpdateCamera(deltaTime);
    UpdateCursorRays();
    UpdateInteract();
    UpdateWeaponLogic(deltaTime);
    UpdateViewWeapon(deltaTime);
    UpdateAnimatedGameObjects(deltaTime);
    UpdateWeaponSlide();
    UpdateSpriteSheets(deltaTime);
    UpdateAudio();
    UpdateUI();
    UpdateFlashlight(deltaTime);
    UpdateFlashlightFrustum();
    UpdatePlayingPiano(deltaTime);
    UpdateCharacterModelHacks();
    UpdateMelleBulletWave(deltaTime);


    if (World::HasOcean()) {
        float feetHeight = GetFootPosition().y;
        float waterHeight = Ocean::GetWaterHeightAtPlayer(m_viewportIndex);
        m_waterState.feetUnderWaterPrevious = m_waterState.feetUnderWater;
        m_waterState.cameraUnderWaterPrevious = m_waterState.cameraUnderWater;
        m_waterState.wadingPrevious = m_waterState.wading;
        m_waterState.swimmingPrevious = m_waterState.swimming;
        m_waterState.cameraUnderWater = GetCameraPosition().y < waterHeight;
        m_waterState.feetUnderWater = GetFootPosition().y < waterHeight;
        m_waterState.heightAboveWater = (GetFootPosition().y > waterHeight) ? (GetFootPosition().y - waterHeight) : 0.0f;
        m_waterState.heightBeneathWater = (GetFootPosition().y < waterHeight) ? (waterHeight - GetFootPosition().y) : 0.0f;
        m_waterState.swimming = m_waterState.cameraUnderWater && IsMoving();
        m_waterState.wading = !m_waterState.cameraUnderWater && m_waterState.feetUnderWater && IsMoving() && feetHeight < waterHeight - 0.5f;
    }
    else {
        m_waterState.feetUnderWaterPrevious = false;
        m_waterState.cameraUnderWaterPrevious = false;
        m_waterState.wadingPrevious = false;
        m_waterState.swimmingPrevious = false;
        m_waterState.cameraUnderWater = false;
        m_waterState.feetUnderWater = false;
        m_waterState.heightAboveWater = 0.0f;
        m_waterState.heightBeneathWater = 0.0f;
        m_waterState.swimming = false;
        m_waterState.wading = false;
    }

    // Weapon audio frequency (for under water)
    m_weaponAudioFrequency = CameraIsUnderwater() ? 0.4f : 1.0f;

    if (m_infoTextTimer > 0) {
        m_infoTextTimer -= deltaTime;
    }
    else {
        m_infoTextTimer = 0;
        m_infoText = "";
    }

    if (IsAlive()) {
        m_timeSinceDeath = 0.0f;
    }
    else {
        m_timeSinceDeath += deltaTime;
        SetFootPosition(glm::vec3(0, -10, 0));
    }

    //if (Input::KeyPressed(HELL_KEY_Q)) {
    //    std::cout << GetFootPosition() << "\n";
    //    std::cout << GetCamera().GetEulerRotation() << "\n\n";
    //}

    if (Input::KeyPressed(HELL_KEY_N) && m_viewportIndex == 0) {
        auto* viewWeapon = GetViewWeaponAnimatedGameObject();
        viewWeapon->PrintNodeNames();
    }

    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (viewport->IsVisible()) {
        if (Input::KeyPressed(HELL_KEY_8)) {
            //std::cout << "\nPlayer " << m_viewportIndex << " inventory:\n";
            //m_inventory.PrintGridOccupiedStateToConsole();

            std::cout << "glm::(" << GetCameraPosition().x << ", " << GetCameraPosition().z << "\n";
        }
    }
}

void Player::Respawn() {
    WeaponManager::Init();
    m_inventory.CloseInventory();
    m_inventory.ClearInventory();

    //World::GetKangaroos()[0].Respawn();

    Logging::Debug() << "Spawning player " << m_viewportIndex;
    SpawnPoint spawnPoint = World::GetRandomCampaignSpawnPoint();
    //Logging::Debug() << "Player " << m_viewportIndex << " spawn: " << spawnPoint.m_position;
    SetFootPosition(spawnPoint.GetPosition());

    //GetCamera().SetEulerRotation(spawnPoint.m_camEuler);

   // else {
   //     if (m_viewportIndex == 1) {
   //         SetFootPosition(glm::vec3(12.5f, 30.6f, 45.5f));
   //         m_camera.SetEulerRotation(glm::vec3(0, 0, 0));
   //     }
   //     if (m_viewportIndex == 2) {
   //         SetFootPosition(glm::vec3(12.5f, 30.6f, 55.5f));
   //         m_camera.SetEulerRotation(glm::vec3(0, 0, 0));
   //     }
   //     if (m_viewportIndex == 3) {
   //         SetFootPosition(glm::vec3(12.5f, 30.6f, 605.5f));
   //         m_camera.SetEulerRotation(glm::vec3(0, 0, 0));
   //     }
   // }

    m_alive = true;

   //if (m_viewportIndex == 0) {
   //    SetFootPosition(glm::vec3(17.0f, 30.7f, 41.5f));
   //    //SetFootPosition(glm::vec3(27.0f, 32.7f, 36.5f)); // roo
   //}
   //if (m_viewportIndex == 1) {
   //    SetFootPosition(glm::vec3(17.0f, 30.7f, 38.5f));
   //    //SetFootPosition(glm::vec3(12.5f, 30.6f, 45.5f));
   //    m_camera.SetEulerRotation(glm::vec3(0, 0, 0));
   //}

    m_weaponStates.clear();
    for (int i = 0; i < WeaponManager::GetWeaponCount(); i++) {
        WeaponState& state = m_weaponStates.emplace_back();
        state.name = WeaponManager::GetWeaponInfoByIndex(i)->name;
        state.has = false;
        state.ammoInMag = 0;
    }

    m_ammoStates.clear();
    for (int i = 0; i < WeaponManager::GetAmmoTypeCount(); i++) {
        AmmoState& state = m_ammoStates.emplace_back();
        state.name = WeaponManager::GetAmmoInfoByIndex(i)->name;
        state.ammoOnHand = 0;
    }

    GiveDefaultLoadout();
    SwitchWeapon("Glock", WeaponAction::DRAW_BEGIN);

    m_flashlightOn = false;
    m_awaitingSpawn = false; 

    m_camera.Update();
    m_flashlightDirection = m_camera.GetForward();


    // Are you inside? Turn flash light on
    float maxRayDistance = 2000;
    glm::vec3 rayOrigin = GetFootPosition() + glm::vec3(0, 2, 0);
    glm::vec3 rayDir = glm::vec3(0, 1, 0);
    PhysXRayResult physxRayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);
    if (!physxRayResult.hitFound) {
        m_flashlightOn = true;
    }

    // Make character model animated again (aka not ragdoll)
    AnimatedGameObject* characterModel = GetCharacterModelAnimatedGameObject();
    if (characterModel) {
        characterModel->SetAnimationModeToAnimated();
    }


    m_respawnCount++;
}



void Player::EnableControl() {
    m_controlEnabled = true;
}
void Player::DisableControl() {
    m_controlEnabled = false;
}

const bool Player::IsAwaitingSpawn() {
    return m_awaitingSpawn;
}

const bool Player::HasControl() {
    return m_controlEnabled;
}

const bool Player::IsLocal() const {
    return m_viewportIndex != -1;
}

const bool Player::IsOnline() const {
    return m_viewportIndex == -1;
}

const glm::mat4& Player::GetViewMatrix() const {
    return m_camera.GetViewMatrix();
}

const glm::mat4& Player::GetInverseViewMatrix() const {
    return m_camera.GetInverseViewMatrix();
}

const glm::vec3& Player::GetCameraPosition() const {
    return m_camera.GetPosition();
}

const glm::vec3& Player::GetCameraRotation() const {
    return m_camera.GetEulerRotation();
}

const glm::vec3& Player::GetCameraForward() const {
    return m_camera.GetForward();
}

const glm::vec3& Player::GetCameraRight() const {
    return m_camera.GetRight();
}

const glm::vec3& Player::GetCameraUp() const {
    return m_camera.GetUp();
}

const int32_t Player::GetViewportIndex() const {
    return m_viewportIndex;
}

const glm::vec3 Player::GetFootPosition() const {
    // FIND ME
    PxController* m_characterController = nullptr;
    CharacterController* characterControler = Physics::GetCharacterControllerById(m_characterControllerId);
    if (characterControler) {
        m_characterController = characterControler->GetPxController();
        PxExtendedVec3 pxPos = m_characterController->getFootPosition();
        return glm::vec3(
            static_cast<float>(pxPos.x),
            static_cast<float>(pxPos.y),
            static_cast<float>(pxPos.z)
        );
    }
    else {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

Camera& Player::GetCamera() {
    return m_camera;
}

AnimatedGameObject* Player::GetCharacterModelAnimatedGameObject() {
    return &m_characterModelAnimatedGameObject;
}

AnimatedGameObject* Player::GetViewWeaponAnimatedGameObject() {
    return &m_viewWeaponAnimatedGameObject;
}

bool Player::ViewportIsVisible() {
    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport) {
        return false;
    }
    else {
        return viewport->IsVisible();
    }
}

bool Player::ViewModelAnimationsCompleted() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (!viewWeapon) {
        std::cout << "WARNING!!! Player::ViewModelAnimationsCompleted() failed coz viewWeapon was nullptr\n";
        return true;
    }
    return viewWeapon->IsAllAnimationsComplete();
}

float Player::GetWeaponAudioFrequency() {
    return m_weaponAudioFrequency;
}

glm::mat4& Player::GetViewWeaponCameraMatrix() {
    return m_viewWeaponCameraMatrix;
}

glm::mat4& Player::GetCSMViewMatrix() {
    return m_csmViewMatrix;
}

void Player::DisplayInfoText(const std::string& text) {
    m_infoTextTimer = 2.0f;
    m_infoText = text;
}

void Player::UpdateAnimatedGameObjects(float deltaTime) {
    m_viewWeaponAnimatedGameObject.Update(deltaTime);
    m_viewWeaponAnimatedGameObject.SetExclusiveViewportIndex(m_viewportIndex);

    m_characterModelAnimatedGameObject.Update(deltaTime);
    m_characterModelAnimatedGameObject.SetIgnoredViewportIndex(m_viewportIndex);
}

const float Player::GetFov() {
    return m_cameraZoom;
}

void Player::Kill() {
    if (m_alive) {
        m_deathCount++;
        m_alive = false;
        m_characterModelAnimatedGameObject.SetAnimationModeToRagdoll();
        m_inventory.CloseInventory();
        Audio::PlayAudio("Death0.wav", 1.0f);
        DropWeapons();
    }
}

glm::vec3 Player::GetViewportColorTint() {
    glm::vec3 colorTint = glm::vec3(1, 1, 1);

    if (InventoryIsOpen() && m_inventory.GetInventoryState() == InventoryState::EXAMINE_ITEM) {
        colorTint = glm::vec3(0.325);
    }

    if (IsDead()) {
        colorTint.r = 2.0;
        colorTint.g = 0.2f;
        colorTint.b = 0.2f;

        float waitTime = 3;
        if (m_timeSinceDeath > waitTime) {
            float val = (m_timeSinceDeath - waitTime) * 10;
            colorTint.r -= val;
        }
    }

    //if (m_viewportIndex == 0) {
    //    std::cout << colorTint << "\n";
    //}

    return colorTint;
}

const void Player::SetName(const std::string& name) {
    m_name = name;
}

bool Player::RespawnAllowed() {
    return IsDead() && m_timeSinceDeath > 3.25f;
}


float Player::GetViewportContrast() {
    if (IsAlive()) {
        return 1.0f;
    }
    else {
        return 1.1f;
    }
}

Ragdoll* Player::GetRagdoll() {
    return Physics::GetRagdollById(m_characterModelAnimatedGameObject.GetRagdollId());
}


bool Player::InventoryIsOpen() {
    return m_inventory.IsOpen();
}

bool Player::InventoryIsClosed() {
    return m_inventory.IsClosed();
}

