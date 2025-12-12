#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"
#include "Camera/Frustum.h"
#include "Inventory/Inventory.h"
#include "Math/AABB.h"
#include "Physics/Physics.h"
#include "Types/Game/AnimatedGameObject.h"
#include "Types/Renderer/SpriteSheetObject.h"
#include "Weapon/WeaponManager.h"

struct Player {
    void Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex);
    void BeginFrame();
    void Update(float deltaTime);
    void UpdateCharacterModelHacks();
    void Respawn();
    void EnableControl();
    void DisableControl();
    void InitRagdoll();
    void InitCharacterModel();
    void CreateCharacterController(glm::vec3 position);
    //void MoveCharacterController(glm::vec3 displacement);
    const bool IsAwaitingSpawn();
    const bool HasControl();
    const bool IsLocal() const;
    const bool IsOnline() const;
    const glm::vec3 GetFootPosition() const;
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetInverseViewMatrix() const;
    const glm::vec3& GetCameraPosition() const;
    const glm::vec3& GetCameraRotation() const;
    const glm::vec3& GetCameraForward() const;
    const glm::vec3& GetCameraRight() const;
    const glm::vec3& GetCameraUp() const;
    const int32_t GetViewportIndex() const;
    AnimatedGameObject* GetCharacterModelAnimatedGameObject();
    AnimatedGameObject* GetViewWeaponAnimatedGameObject();
    Camera& GetCamera();

    glm::vec3 GetViewportColorTint();
    float GetViewportContrast();
    float m_timeSinceDeath = 0.0f;
    bool RespawnAllowed();

    void SubmitRenderItems();
    void SubmitAttachmentRenderItem(const std::string& weaponAttachmentName);


    void UpdateCursorRays();
    void UpdateInteract();
    void UpdateCamera(float deltaTime);
    void UpdateViewWeapon(float deltaTime);
    void UpdateMovement(float deltaTime);
    void UpdateUI();
    void UpdateSpriteSheets(float deltaTime);
    void UpdateHeadBob(float deltaTime);
    void UpdateBreatheBob(float deltaTime);
    void UpdateAudio();
    void UpdateFlashlight(float deltaTime);
    void UpdateFlashlightFrustum();
    void UpdateAnimatedGameObjects(float deltaTime);
    void UpdatePlayingPiano(float deltaTime);
    void UpdateWeaponSlide();

    // Remove me when you can
    void HideKnifeMesh();
    void HideGlockMesh();
    void HideShotgunMesh();
    void HideAKS74UMesh();
    // Remove me when you can

    void Kill();
    Ragdoll* GetRagdoll();
    glm::mat4 m_deathCamViewMatrix = glm::mat4(1.0f);


    // Weapon shit
    int GetCurrentWeaponMagAmmo();
    int GetCurrentWeaponTotalAmmo();
    bool IsShellInShotgunChamber();
    void UpdateWeaponLogic(float deltaTime);
    void GiveDefaultLoadout();
    void GiveWeapon(const std::string& name);
    void GiveAmmo(const std::string& name, int amount);
    void GiveSight(const std::string& weaponName);
    void GiveSilencer(const std::string& weaponName);
    void SwitchWeapon(const std::string& name, WeaponAction weaponAction);
    void DropWeapons();
    WeaponAction& GetWeaponAction();
    WeaponInfo* GetCurrentWeaponInfo();
    WeaponState* GetWeaponStateByName(const std::string &name);
    WeaponState* GetCurrentWeaponState();
    AmmoState* GetAmmoStateByName(const std::string& name);
    AmmoState* GetCurrentAmmoState();
    AmmoInfo* GetCurrentAmmoInfo();

    void AddHorizontalImpulse(glm::vec3 direction, float force);
    void AddVerticalImpulse(float force);
    void SimulateVelocityMovement(float deltaTime, const glm::vec3& inputDirection, float maxSpeed, float accelerationStrength, float damping, bool applyGravity, float gravity);
    //glm::vec3 m_velocity = glm::vec3(0.0f);
    glm::vec3 m_movementDirection = glm::vec3(0.0f); // can be zero
    float m_acceleration = 0.0f;

    glm::ivec2 GetViewportCenter();
    void CheckForMeleeHit();
    float GetTargetWalkingSpeed();

    // Water
    WaterState m_waterState;
    bool FeetEnteredUnderwater();
    bool FeetExitedUnderwater();
    bool CameraEnteredUnderwater();
    bool CameraExitedUnderwater();
    bool IsSwimming();
    bool IsWading();
    bool CameraIsUnderwater();
    bool FeetBelowWater();
    bool StartedSwimming();
    bool StoppedSwimming();
    bool StartingWading();
    bool StoppedWading();
    float GetFeetDistanceBeneathWater();

    // Piano
    uint64_t m_pianoId = 0;
    bool m_isPlayingPiano = false;
    void SitAtPiano(uint64_t pianoId);

    // State queries
    bool IsMoving();
    bool IsGrounded();
    bool IsCrouching();
    //bool IsAlive();
    //bool IsOverlappingLadder();
    //bool IsAtShop();
    bool IsPlayingPiano();
    //bool IsUnderWater();

    // Input
    int32_t GetKeyboardIndex();
    int32_t GetMouseIndex();
    void SetKeyboardIndex(int32_t index);
    void SetMouseIndex(int32_t index);
    bool PressingWalkForward();
    bool PressingWalkBackward();
    bool PressingWalkLeft();
    bool PressingWalkRight();
    bool PressingCrouch();
    bool PressedUp(bool allowKeyRepeat = false);
    bool PressedDown(bool allowKeyRepeat = false);
    bool PressedLeft(bool allowKeyRepeat = false);
    bool PressedRight(bool allowKeyRepeat = false);
    bool PressedWalkForward(bool allowKeyRepeat = false);
    bool PressedWalkBackward(bool allowKeyRepeat = false);
    bool PressedWalkLeft(bool allowKeyRepeat = false);
    bool PressedWalkRight(bool allowKeyRepeat = false);
    bool PressedInteract();
    bool PressingInteract();
    bool PressedReload();
    bool PressedFire();
    bool PressingFire();
    bool PressingJump();
    bool PressingRun();
    bool PressedRun();
    bool PressedCrouch();
    bool PressedWeaponMiscFunction();
    bool PressedNextWeapon();
    bool PressingADS();
    bool PressedADS();
    bool PressedEscape();
    bool PressedMelee();
    bool PressedFlashlight();
    bool PressedToggleInventory();
    bool PressedInventoryExamine();

    const InventoryState& GetInvetoryState() { return m_inventory.GetInventoryState(); }
    Inventory& GetInventory() { return m_inventory; }

    void DisplayInfoText(const std::string& text);
    std::string m_infoText = "";
    float m_infoTextTimer = 0;

    // Dev keys
    bool PressedFullscreen();
    bool PressedOne();
    bool PressedTwo();
    bool PressedThree();
    bool PressedFour();

    bool ShouldRenderViewWeapon();

    const float GetFov();
    const void SetName(const std::string& name);

    glm::mat4 GetViewWeaponBoneWorldMatrix(const std::string& boneName);

    ivecXZ GetChunkPos() { return m_chunkPos; }

    bool InventoryIsOpen();
    bool InventoryIsClosed();

    bool InteractFound()                { return m_interactFound; }
    uint64_t GetInteractObjectId()      { return m_interactObjectId; }
    Frustum& GetFlashlightFrustum()     { return m_flashlightFrustum; }
    const std::string& GetName() const  { return m_name; }

private:
    std::string m_name = "PLAYER_NAME";
    Inventory m_inventory;

    // Interact
    PhysXRayResult m_physXRayResult;
    BvhRayResult m_bvhRayResult;
    uint64_t m_interactObjectId = 0;
    uint32_t m_interactOpenableId = 0;
    uint32_t m_interactCustomId = 0;
    bool m_rayHitFound = false;
    bool m_interactFound = false;
    int32_t m_deathCount = 0;
    int32_t m_respawnCount = 0;

    //glm::vec3 m_position = glm::vec3(0.0f);
    ivecXZ m_chunkPos;

    // Interact

    Frustum m_flashlightFrustum;


    // Flashlight
    glm::vec3 m_flashlightPosition;
    glm::vec3 m_flashlightDirection;
    glm::mat4 m_flashlightProjectionView;
    bool m_flashlightOn = false;
    float m_flashLightModifier = 0.0f;

    // Headbob 
    float m_bobOffsetX = 0.0f;
    float m_bobOffsetY = 0.0f;
    float m_headBobTime = 0.0f;
    float m_breatheBobTime = 0.0f;
    glm::vec3 m_headBob = glm::vec3(0.0f);
    glm::vec3 m_breatheBob = glm::vec3(0.0f);

    // Audio
    bool m_footstepPlayed = false;

    // States
    bool m_moving = false;
    bool m_crouching = false;
    bool m_grounded = true;
    bool m_groundedLastFrame;
    bool m_feetAboveHeightField = false;
    bool m_running = false;

    // Speed
    float m_walkSpeed = 5.0f;
    float m_currentSpeed = 0.0f;
    float m_walkingSpeed = 4.85f;
    float m_crouchingSpeed = 2.325f;
    float m_runningSpeed = m_walkingSpeed * 1.1f;
    float m_swimmingSpeed = 3.25f;
    float m_crouchDownSpeed = 17.5f;

    // Heights
    float m_realViewHeightStanding = 1.65f;  // are these used?
    float m_realViewHeightCrouching = 1.15f; // are these used?
    float m_viewHeightStanding = m_realViewHeightStanding;
    float m_viewHeightCrouching = m_realViewHeightCrouching;
    float m_currentViewHeight = m_viewHeightStanding;

    // Misc
    int m_revolverReloadIterations = 0;
    int m_currentWeaponIndex = 0;
    bool m_controlEnabled = true;
    bool m_awaitingSpawn = true;
    bool m_firedThisFrame = false;
    bool _needsAmmoReloaded = false;
    bool m_revolverNeedsCocking = false;
    bool _glockSlideNeedsToBeOut = false;
    bool _needsShotgunFirstShellAdded = false;
    bool _needsShotgunSecondShellAdded = false;
    float m_mouseSensitivity = 0.002f;
    float m_cameraZoom = 1.0f; 
    float m_accuracyModifer = 0;
    int32_t m_viewportIndex = 0;
    Camera m_camera;
    InputType m_inputType = KEYBOARD_AND_MOUSE;
    PlayerControls m_controls;
    ShellEjectionState m_shellEjectionState;
    WeaponAction m_weaponAction = DRAW_BEGIN;
    
    std::vector<WeaponState> m_weaponStates;
    std::vector<AmmoState> m_ammoStates;

    float m_waterImpactVelocity = 0;



    float _muzzleFlashRotation = 0;
    glm::vec2 _weaponSwayFactor = glm::vec2(0);
    glm::vec3 _weaponSwayTargetPos = glm::vec3(0);
    float _muzzleFlashCounter = 0;

    //uint32_t _interactFlags;
    //uint32_t _bulletFlags;
    int m_playerIndex;


    public:
        float _muzzleFlashTimer = 0;

        Transform m_weaponSwayTransform;

        glm::mat4 m_weaponSwayMatrix = glm::mat4(1);
        int m_mouseIndex = -1;
        int m_keyboardIndex = -1;

        glm::mat4 GetItemExamineModelMatrix() { return m_inventory.GetItemExamineModelMatrix(); }


        float m_weaponAudioFrequency = 1.0f;

        float GetWeaponAudioFrequency();

        void NextWeapon();
        void UpdateMeleeLogic(float deltaTime);
        void UpdateGunLogic(float deltaTime);
        void UpdateShotgunGunLogic(float deltaTime);

        WeaponType GetCurrentWeaponType();
        WeaponAction GetCurrentWeaponAction();

        // Melee
        void FireMelee();
        bool CanFireMelee();
        
        // Gun
        void FireGun();
        void ReloadGun();
        void EnterADS();
        void LeaveADS();
        void UpdateGunReloadLogic();
        void UpdateSlideLogic();
        void UpdateADSLogic(float deltaTime);
        bool CanFireGun();
        bool CanReloadGun();
        bool CanEnterADS();
        bool CanLeaveADS();
        void SpawnBullet(float variance);
        bool IsInADS();

        // Shotgun
        void FireShotgun();
        void ShotgunMelee();
        void DryFireShotgun();
        void ReloadShotgun();
        void UnloadShotgun();
        void ToggleAutoShotgun();
        void UpdatePumpAudio();        
        void UpdateShotgunReloadLogic();
        void UpdateShotgunUnloadLogic();
        bool CanToggleShotgunAuto();
        bool CanFireShotgun();
        bool CanDryFireShotgun();
        bool CanReloadShotgun();
        bool CanUnloadShotgun();
        bool ShotgunRequiresPump();

        // Casings and Muzzle flash
        void SpawnMuzzleFlash(float speed, float scale);
        void SpawnCasing(AmmoInfo* ammoInfo, bool alternateAmmo);


        bool ViewModelAnimationsCompleted();
        bool ViewportIsVisible();

        // Physics 
        void SetFootPosition(glm::vec3 position);
        PxShape* GetCharacterControllerShape();
        PxRigidDynamic* GetCharacterControllerActor();

        glm::mat4& GetViewWeaponCameraMatrix();
        glm::mat4& GetCSMViewMatrix();

        glm::mat4 m_viewWeaponCameraMatrix;
        glm::mat4 m_csmViewMatrix;


        float m_yVelocity = 0;

        float m_weaponSwayX = 0;
        float m_weaponSwayY = 0;

        // Swimming
        float m_swimVerticalAcceleration = 0.0f;
        float m_smoothedWaterY;
        //bool m_underWater = false;

        struct MeleeBulletWaveState {
            bool active = false;
            float startTime = 0.0f;
            float maxTime = 0.0f;
            float time = 0.0f;
            float intervalDuration = 0.0f;
            float intervalCounter = 0.0f;
            int spawnCountThisWave = 0;
        } m_meleeBulletWaveState;

        void UpdateMelleBulletWave(float deltaTime);
        void BeginMeleeBulletWave();

    private:
        bool m_alive = true;

        AnimatedGameObject m_viewWeaponAnimatedGameObject;
        AnimatedGameObject m_characterModelAnimatedGameObject;
        SpriteSheetObject m_muzzleFlash;

      //  CharacterController

        //PxController* m_characterController = NULL;

        std::vector<SpriteSheetRenderItem> m_spriteSheetRenderItems;
        AABB m_characterControllerAABB;

        void UpdateWalkingMovement(float deltaTime);
        void UpdateSwimmingMovement(float deltaTime);

        uint64_t m_playerId = 0;
        uint64_t m_characterControllerId = 0;

    public:
        const uint64_t GetPlayerId()                                            { return m_playerId; }
        const uint64_t GetcharacterControllerId()                               { return m_characterControllerId; }
        const uint64_t GetRagdollId()                                           { return m_characterModelAnimatedGameObject.GetRagdollId(); }
        const bool IsAlive()                                                    { return m_alive; }
        const bool IsDead()                                                     { return !m_alive; }
        const bool IsRunning()                                                  { return m_running; }
        const bool AreFeetAboveHeightField()                                    { return m_feetAboveHeightField; }
        const std::vector<SpriteSheetRenderItem>& GetSpriteSheetRenderItems()   { return m_spriteSheetRenderItems; }
        const glm::vec3 GetFlashlightPosition()                                 { return m_flashlightPosition; }
        const glm::vec3 GetFlashlightDirection()                                { return m_flashlightDirection; };
        const glm::mat4 GetFlashlightProjectionView()                           { return m_flashlightProjectionView; };
        const float GetFlashLightModifer()                                      { return m_flashLightModifier; }
        const AABB& GetCharacterControllerAABB()                                { return m_characterControllerAABB; }
};