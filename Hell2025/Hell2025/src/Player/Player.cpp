#include "Player.h"
#include "HellConstants.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Audio/Audio.h"
#include "BackEnd/BackEnd.h"
#include "Bible/Bible.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Ocean/Ocean.h"
#include "Viewport/ViewportManager.h"
#include "UniqueID.h"
#include "Input/InputMulti.h"

// Get me out of here
#include "Renderer/Renderer.h"
#include "World/World.h"
// Get me out of here

void Player::Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex) {
    m_playerId = UniqueID::GetNextObjectId(ObjectType::PLAYER);

    m_camera.SetPosition(position + glm::vec3(0.0f, m_viewHeightStanding, 0.0f));
    m_camera.SetEulerRotation(rotation);
    m_viewportIndex = viewportIndex;

    m_inventory.SetLocalPlayerIndex(m_viewportIndex);
    m_shopInventory.SetLocalPlayerIndex(m_viewportIndex);

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
    m_interactOpenableId = 0;
}

void Player::EnterShop() {
    m_isInShop = true;
    m_shopInventory.OpenAsShop();
    m_inventory.CloseInventory();

    const std::string& text = Bible::RandomMermaidGreeting();
    m_typeWriter.DisplayText(text);

    m_flashlightOn = true;
    Audio::PlayAudio(AUDIO_SELECT, 1.00f);

    InputMulti::ClearKeyStates();
}

void Player::LeaveShop() {
    m_isInShop = false;
    m_inventory.CloseInventory();
    m_shopInventory.CloseInventory();
    m_typeWriter.ClearText();
}

static float WrapPi(float a) {
    const float twoPi = 6.28318530718f;
    a = std::fmod(a + 3.14159265359f, twoPi);
    if (a < 0.0f) a += twoPi;
    return a - 3.14159265359f;
}

static float LerpAngle(float a, float b, float t) {
    float delta = WrapPi(b - a);
    return a + delta * t;
}

void Player::UpdateShop(float deltaTime) {
    glm::vec3 targetPosition = glm::vec3(13.06f, 28.68f, 36.78);
    glm::vec3 targetCamEuler = glm::vec3(-0.08f, -1.65f, 0.0f);

    if (Game::GetSplitscreenMode() == SplitscreenMode::TWO_PLAYER) {
        targetPosition = glm::vec3(12.93f, 28.61f, 36.80);
        targetCamEuler = glm::vec3(-0.00f, -2.05f, 0.0f);
    }


    glm::vec3 currentPosition = GetFootPosition();
    glm::vec3 currentCamEuler = m_camera.GetEulerRotation();

    float positionInterpolationSpeed = 25.0f;
    float rotationInterpolationSpeed = 50.0f;

    float positionT = 1.0f - std::exp(-positionInterpolationSpeed * deltaTime);
    float rotationT = 1.0f - std::exp(-rotationInterpolationSpeed * deltaTime);

    glm::vec3 newPosition = glm::mix(currentPosition, targetPosition, positionT);

    glm::vec3 newCamEuler = currentCamEuler;
    newCamEuler.x = LerpAngle(currentCamEuler.x, targetCamEuler.x, rotationT); // pitch
    newCamEuler.y = LerpAngle(currentCamEuler.y, targetCamEuler.y, rotationT); // yaw
    newCamEuler.z = 0.0f;

    SetFootPosition(newPosition);
    m_camera.SetEulerRotation(newCamEuler);
}

void Player::Update(float deltaTime) {
    m_moving = false;

    if (Editor::IsOpen()) {
        return;
    }

    // Toggle inventory
    if (PressedToggleInventory() && m_shopInventory.IsClosed()) {

        // Was the inventory closed? Then open it
        if (m_inventory.IsClosed() ) {
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

        // Hack to also exit shop if the inventory is being used to display your items when in shop to SELL
        m_shopInventory.CloseInventory();
        m_isInShop = false;

        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
    }

    // Close the shop
    if (PressedToggleInventory() && IsInShop()) {
        LeaveShop();
    }

    // Shop hack test
    //if (m_viewportIndex == 0 && Input::KeyPressed(HELL_KEY_U)) {
    //    EnterShop();
    //}

    if (IsInShop()) {
        UpdateShop(deltaTime);
    }
    
    //if (ViewportIsVisible()) {
    //    std::cout << "Pos:" << GetFootPosition() << " cam: " << GetCameraRotation() << "\n";
    //    
    //    //std::cout << "Shop: " << m_shopInventory.IsOpen() << " Inv: " << m_inventory.IsOpen() << "\n";
    //}

    // This may break code elsewhere in the player logic like anywhere
    if (m_inventory.IsOpen()) {
        DisableControl();
        m_inventory.Update(deltaTime);
    }
    if (m_shopInventory.IsOpen()) {
        DisableControl();
        m_shopInventory.Update(deltaTime);
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
    m_running = false;

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
 
    UpdateLadderIds();
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
    UpdateAudio(deltaTime);
    UpdateUI(deltaTime);
    UpdateFlashlight(deltaTime);
    UpdateFlashlightFrustum();
    UpdatePlayingPiano(deltaTime);
    UpdateCharacterModelHacks();
    UpdateMelleBulletWave(deltaTime);

    float minimumMermaidInteractYHeight = 28.0f;
    if (PressedInteract() && GetFootPosition().y > minimumMermaidInteractYHeight && IsFacingClosestMermaid() && !IsInShop()) {
        EnterShop();
    }


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

   //if (Input::KeyPressed(HELL_KEY_N) && m_viewportIndex == 0) {
   //    auto* viewWeapon = GetViewWeaponAnimatedGameObject();
   //    viewWeapon->PrintNodeNames();
   //}

    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (viewport->IsVisible()) {

        //std::cout << "Facing: " << IsFacingClosestMermaid() << " " << "Dot: " << DotToClosestToMermaid() << "\n";

        if (Input::KeyPressed(HELL_KEY_8)) {
            //std::cout << "\nPlayer " << m_viewportIndex << " inventory:\n";
            //m_inventory.PrintGridOccupiedStateToConsole();

            //std::cout << "glm::(" << GetCameraPosition().x << ", " << GetCameraPosition().z << "\n";


            std::cout << "POS:" << GetCameraPosition() << " ROT: " << GetCameraRotation() << "\n";


        }
    }
}

void Player::Respawn() {
    m_inventory.Init();
    m_shopInventory.Init();
    m_health = 100;
    m_isInShop = false;

    //World::GetKangaroos()[0].Respawn();

    Logging::Debug() << "Spawning player " << m_viewportIndex;
    SpawnPoint spawnPoint = World::GetRandomCampaignSpawnPoint();


    //Logging::Debug() << "Player " << m_viewportIndex << " spawn: " << spawnPoint.m_position;
    SetFootPosition(spawnPoint.GetPosition() - glm::vec3(0.0f, 1.60, 0.0f));
    m_camera.SetEulerRotation(spawnPoint.GetCamEuler());

    //if (m_viewportIndex == 0) {
    //    SetFootPosition(glm::vec3(36.18, 31, 37.26));
    //    m_camera.SetEulerRotation(glm::vec3(-0.15, -0.02, 0));
    //}

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


    GiveDefaultLoadout();
    SwitchWeapon("Glock", WeaponAction::DRAW_BEGIN);

    m_flashlightOn = false;
    m_awaitingSpawn = false; 

    m_camera.Update();
    m_flashlightDirection = m_camera.GetForward();

    if (IsInShop()) {
        m_flashlightDirection += glm::vec3(0.0f, -0.1f, 0.0f);
        m_flashlightDirection = glm::normalize(m_flashlightDirection);
    }

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

void Player::GiveDamage(int damage, uint64_t enemyId) {
    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        Kill();
    }
}

float Player::DotToClosestToMermaid() {
    if (World::GetMermaids().empty()) return 0;

    Mermaid& mermaid = World::GetMermaids()[0];
    return glm::dot(mermaid.GetWorldForward(), GetCameraForward());
}

void Player::GiveCash(int amount) {
    m_cash += amount;
}

void Player::SubtractCash(int amount) {
    m_cash -= amount;
}

bool Player::IsFacingClosestMermaid() {
    if (World::GetMermaids().empty()) return false;

    Mermaid& mermaid = World::GetMermaids()[0];

    const glm::vec3& cameraPositon = GetCameraPosition();
    const glm::vec3& cameraForward = GetCameraForward();

    glm::vec3 toCamera = cameraPositon - mermaid.GetPosition();
    glm::vec3 toMermaid = mermaid.GetPosition() - cameraPositon;

    bool cameraOnFrontSide = glm::dot(mermaid.GetWorldForward(), toMermaid) > 0.0f;
    bool mermaidInFrontOfCamera = glm::dot(cameraForward, toMermaid) > 0.0f;

    // HACCCCCCCCCK because cameraOnFrontSide doesn't evaluate to what you think it does
    float distanceToMermaid = glm::distance(cameraPositon, mermaid.GetPosition());
    if (distanceToMermaid > 2.0f) {
        return false;
    }

    return !(cameraOnFrontSide && mermaidInFrontOfCamera);
}

void Player::Kill() {
    if (m_alive) {
        m_flashlightOn = false;
        m_deathCount++;
        m_alive = false;
        m_characterModelAnimatedGameObject.SetAnimationModeToRagdoll();
        m_inventory.CloseInventory();
        m_shopInventory.CloseInventory();
        Audio::PlayAudio("Death0.wav", 1.0f);
        DropWeapons();
        m_cash /= 2;

        // HACK
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            if (i != m_viewportIndex) {
                if (Player* player = Game::GetLocalPlayerByIndex(i)) {
                    player->GiveCash(100);
                    player->m_killCount++;
                }
            }
        }
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

bool Player::ShopInventoryIsOpen() {
    return m_shopInventory.IsOpen();
}

bool Player::ShopInventoryIsClosed() {
    return m_shopInventory.IsClosed();
}

