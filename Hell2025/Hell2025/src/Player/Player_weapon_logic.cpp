#include "Player.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Util.h"
#include "World/World.h"

// remove me
#include "Renderer/Renderer.h"

void Player::UpdateWeaponLogic(float deltaTime) {
    if (IsDead()) return;

    if (InventoryIsOpen()) {
        if (m_weaponAction == ADS_IN ||
            m_weaponAction == ADS_IDLE ||
            m_weaponAction == ADS_FIRE) {
            LeaveADS();
        }
    }
    if (InventoryIsClosed()) {
        if (PressedNextWeapon()) {
            NextWeapon();
        }
    }

    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();

    if (!viewWeapon) return;
    if (!weaponInfo) return;

    switch (GetCurrentWeaponType()) {
        case WeaponType::MELEE:     UpdateMeleeLogic(deltaTime);        break;
        case WeaponType::PISTOL:    UpdateGunLogic(deltaTime);          break;
        case WeaponType::AUTOMATIC: UpdateGunLogic(deltaTime);          break;
        case WeaponType::SHOTGUN:   UpdateShotgunGunLogic(deltaTime);   break;
    }

    // Need to initiate draw animation?
    if (GetCurrentWeaponAction() == WeaponAction::DRAW_BEGIN) {

        // Drawing a shotgun when it needs a pump
        if (GetCurrentWeaponType() == WeaponType::SHOTGUN && !IsShellInShotgunChamber() && weaponState->ammoInMag > 0) {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunDrawPump, weaponInfo->animationSpeeds.shotgunDrawPump);
            weaponState->shotgunAwaitingPumpAudio = true;
            weaponState->shotgunRequiresPump = true;
            m_weaponAction = DRAWING_WITH_SHOTGUN_PUMP;
        }
        // First draw
        else if (weaponState->awaitingDrawFirst && weaponInfo->animationNames.drawFirst != "") {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.drawFirst, weaponInfo->animationSpeeds.drawFirst);
            weaponState->awaitingDrawFirst = false;
            m_weaponAction = DRAWING_FIRST;
            Audio::PlayAudio(weaponInfo->audioFiles.drawFirst, 1.0f);
        }
        // Regular draw
        else {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.draw, weaponInfo->animationSpeeds.draw);
            m_weaponAction = DRAWING;
        }
    }

    // Finished ADS in? Return to ADS idle
    if (GetWeaponAction() == WeaponAction::ADS_IN && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.adsIn) ||
        GetWeaponAction() == WeaponAction::ADS_FIRE && ViewModelAnimationsCompleted()) {
        m_weaponAction = WeaponAction::ADS_IDLE;
    }

    // Finished drawing weapon? Return to idle
    if (GetCurrentWeaponAction() == WeaponAction::DRAWING && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.draw) ||
        GetCurrentWeaponAction() == WeaponAction::DRAWING_FIRST && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.drawFirst) ||
        GetCurrentWeaponAction() == WeaponAction::DRAWING_WITH_SHOTGUN_PUMP && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.shotgunDrawPump)) {
        m_weaponAction = WeaponAction::IDLE;
    }

    // In ADS idle?
    if (GetCurrentWeaponAction() == WeaponAction::ADS_IDLE) {
        if (IsMoving()) {
            viewWeapon->PlayAndLoopAnimation("MainLayer", weaponInfo->animationNames.adsWalk, weaponInfo->animationSpeeds.adsWalk);
        }
        else {
            viewWeapon->PlayAndLoopAnimation("MainLayer", weaponInfo->animationNames.adsIdle, weaponInfo->animationSpeeds.adsIdle);
        }
    }

    // In idle? Then play idle or walk if moving
    if (GetCurrentWeaponAction() == WeaponAction::IDLE) {
        const std::string& animName = IsMoving() ? weaponInfo->animationNames.walk : weaponInfo->animationNames.idle;
        viewWeapon->PlayAndLoopAnimation("MainLayer", animName, 1.0f);
    }

    // Everything done? Go to idle
    if (ViewModelAnimationsCompleted()) {
        m_weaponAction = WeaponAction::IDLE;
    }
}

void Player::GiveDefaultLoadout() {
    // Always give knife
    m_inventory.AddItem("Knife");
    m_inventory.AddItem("Glock");
    m_inventory.AddItem("GoldenGlock");
    m_inventory.AddItem("Tokarev");
    m_inventory.AddItem("Remington870");
    m_inventory.AddItem("SPAS");
    m_inventory.AddItem("BlackSkull");
    m_inventory.AddItem("SmallKey");


    GiveWeapon("Knife");
    GiveWeapon("SPAS");
    GiveWeapon("Remington870");
    GiveWeapon("Glock");
    GiveWeapon("Tokarev");
    GiveWeapon("GoldenGlock");

    GiveAmmo("Shotgun", 80);
    GiveAmmo("Glock", 200);
    GiveAmmo("Tokarev", 200);

    GiveSilencer("Glock");
    GiveSight("GoldenGlock");    
}

void Player::NextWeapon() {
    m_currentWeaponIndex++;
    if (m_currentWeaponIndex == m_weaponStates.size()) {
        m_currentWeaponIndex = 0;
    }
    while (!m_weaponStates[m_currentWeaponIndex].has) {
        m_currentWeaponIndex++;
        if (m_currentWeaponIndex == m_weaponStates.size()) {
            m_currentWeaponIndex = 0;
        }
    }
    Audio::PlayAudio("NextWeapon.wav", 0.5f);
    SwitchWeapon(m_weaponStates[m_currentWeaponIndex].name, DRAW_BEGIN);
}

void Player::SwitchWeapon(const std::string& name, WeaponAction weaponAction) {
    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();

    if (!state) return;
    if (!weaponInfo) return;
    if (!viewWeapon) return;

    for (int i = 0; i < m_weaponStates.size(); i++) {
        if (m_weaponStates[i].name == name) {
            m_currentWeaponIndex = i;
        }
    }
    viewWeapon->SetName(weaponInfo->name);
    viewWeapon->SetSkinnedModel(weaponInfo->modelName);
    viewWeapon->EnableDrawingForAllMesh();
    viewWeapon->SetGoldFlag(weaponInfo->isGold);

    // Set materials
    for (auto& it : weaponInfo->meshMaterials) {
        viewWeapon->SetMeshMaterialByMeshName(it.first, it.second);
    }
    // Set materials by index
    for (auto& it : weaponInfo->meshMaterialsByIndex) {
        viewWeapon->SetMeshMaterialByMeshIndex(it.first, it.second);
    }
    // Hide mesh
    for (auto& meshName : weaponInfo->hiddenMeshAtStart) {
        viewWeapon->DisableDrawingForMeshByMeshName(meshName);
    }
    m_weaponAction = weaponAction;

    Audio::PlayAudio("NextWeapon.wav", 0.5f);
}

WeaponType Player::GetCurrentWeaponType() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        return weaponInfo->type;
    } 
    else {
        return WeaponType::UNDEFINED;
    }
}

WeaponAction Player::GetCurrentWeaponAction() {
    return m_weaponAction;
}

WeaponAction& Player::GetWeaponAction() {
    return m_weaponAction;
}

WeaponInfo* Player::GetCurrentWeaponInfo() {
    return WeaponManager::GetWeaponInfoByName(m_weaponStates[m_currentWeaponIndex].name);;
}

void Player::GiveWeapon(const std::string& name) {
    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    if (state && weaponInfo) {
        state->has = true;
        state->ammoInMag = weaponInfo->magSize;

    }
}

void Player::GiveAmmo(const std::string& name, int amount) {
    AmmoState* state = GetAmmoStateByName(name);
    if (state) {
        state->ammoOnHand += amount;
    }
}

void Player::GiveSight(const std::string& weaponName) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(weaponName);
    WeaponState* state = GetWeaponStateByName(weaponName);
    if (state && weaponInfo) {
        state->hasSight = true;
    }
}

void Player::GiveSilencer(const std::string& weaponName) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(weaponName);
    WeaponState* state = GetWeaponStateByName(weaponName);
    if (state && weaponInfo) {
        state->hasSilencer = true;
    }
}

WeaponState* Player::GetWeaponStateByName(const std::string& name) {
    for (int i = 0; i < m_weaponStates.size(); i++) {
        if (m_weaponStates[i].name == name) {
            return &m_weaponStates[i];
        }
    }
    return nullptr;
}

AmmoState* Player::GetAmmoStateByName(const std::string& name) {
    for (int i = 0; i < m_ammoStates.size(); i++) {
        if (m_ammoStates[i].name == name) {
            return &m_ammoStates[i];
        }
    }
    return nullptr;
}

AmmoState* Player::GetCurrentAmmoState() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return GetAmmoStateByName(weaponInfo->ammoType);
}

AmmoInfo* Player::GetCurrentAmmoInfo() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return WeaponManager::GetAmmoInfoByName(weaponInfo->ammoType);
}

WeaponState* Player::GetCurrentWeaponState() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return GetWeaponStateByName(weaponInfo->name);
}

int Player::GetCurrentWeaponMagAmmo() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        if (weaponState) {
            return weaponState->ammoInMag;
        }
    }
    return 0;
}

int Player::GetCurrentWeaponTotalAmmo() {
    AmmoState* ammoState = GetCurrentAmmoState();
    if (!ammoState) return 0;

    return ammoState->ammoOnHand;
}

void Player::SpawnMuzzleFlash(float speed, float scale) {
    m_muzzleFlash.SetSpeed(speed);
    m_muzzleFlash.SetScale(glm::vec3(scale));
    m_muzzleFlash.SetTime(0.0f);
    m_muzzleFlash.EnableRendering();
    m_muzzleFlash.SetRotation(glm::vec3(0.0f, 0.0f, Util::RandomFloat(0, HELL_PI * 2)));
}

void Player::SpawnCasing(AmmoInfo* ammoInfo, bool alternateAmmo) {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return;

    if (!Util::StrCmp(ammoInfo->casingModelName, UNDEFINED_STRING)) {
        BulletCasingCreateInfo createInfo;
        createInfo.modelIndex = AssetManager::GetModelIndexByName(ammoInfo->casingModelName);
        createInfo.materialIndex = AssetManager::GetMaterialIndexByName(ammoInfo->casingMaterialName);
        createInfo.position = viewWeapon->GetBoneWorldPosition(weaponInfo->casingEjectionBoneName);
        createInfo.rotation.y = m_camera.GetYaw() + (HELL_PI * 0.5f);
        createInfo.force = glm::normalize(GetCameraRight() + glm::vec3(0.0f, Util::RandomFloat(0.7f, 0.9f), 0.0f)) * glm::vec3(weaponInfo->casingEjectionImpulse);
    // createInfo.force = glm::normalize(GetCameraRight() + glm::vec3(0.0f, Util::RandomFloat(0.7f, 0.9f), 0.0f)) * glm::vec3(0.0175);
    // std::cout << "warning: you have hardcoded casing ejection impulse!\n";

        createInfo.position += GetCameraForward() * glm::vec3(0.15f);
        createInfo.position += GetCameraRight() * glm::vec3(0.05f);
        createInfo.position += GetCameraUp() * glm::vec3(-0.025f);

        if (alternateAmmo) {
            createInfo.materialIndex = AssetManager::GetMaterialIndexByName("ShellGreen");
        }

        createInfo.mass = 0.008f;

        World::AddBulletCasing(createInfo);


    }
    else {
        std::cout << "Player::SpawnCasing(AmmoInfo* ammoInfo) failed to spawn a casing coz invalid casing model name in weapon info\n";
    }
}

void Player::SpawnBullet(float variance) {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    glm::vec3 bulletDirection = GetCameraForward();
    bulletDirection.x += Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
    bulletDirection.y += Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
    bulletDirection.z += Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
    bulletDirection = glm::normalize(bulletDirection);

    BulletCreateInfo createInfo;
    createInfo.origin = GetCameraPosition();
    createInfo.direction = bulletDirection;
    createInfo.damage = weaponInfo->damage;
    createInfo.weaponIndex = WeaponManager::GetWeaponIndexFromWeaponName(weaponInfo->name);
    createInfo.ownerObjectId = m_playerId;

    World::AddBullet(createInfo);
}

void Player::UpdateWeaponSlide() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();

    std::string& boneName = weaponInfo->pistolSlideBoneName;

    if (weaponState->requiresSlideOffset) {
        Transform transform;
        transform.position.z = -weaponInfo->pistolSlideOffset;
        viewWeapon->SetAdditiveTransform(boneName, transform.to_mat4());
    }
    else {
        viewWeapon->SetAdditiveTransform(boneName, glm::mat4(1.0f));
    }
}

void Player::DropWeapons() {

    for (WeaponState& weaponState : m_weaponStates) {
        if (weaponState.has) {
            
            WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(weaponState.name);
            if (!weaponInfo) {
                std::cout << "You tried to drop a weapon with an invalid name somehow...\n";
                continue;
            }

            if (weaponInfo->pickupName != "") {
                PickUpCreateInfo createInfo;
                createInfo.position = GetCameraPosition();
                createInfo.rotation.x = Util::RandomFloat(-HELL_PI, HELL_PI);
                createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
                createInfo.rotation.z = Util::RandomFloat(-HELL_PI, HELL_PI);
                createInfo.pickUpType = weaponInfo->pickupName;
                createInfo.isGold = weaponInfo->isGold;

                createInfo.intitialForce.x = Util::RandomFloat(-HELL_PI * 0.5f, HELL_PI * 0.5f);
                createInfo.intitialForce.y = 1.0f;
                createInfo.intitialForce.z = Util::RandomFloat(-HELL_PI * 0.5f, HELL_PI * 0.5f);
                createInfo.intitialForce = glm::normalize(createInfo.intitialForce);
                createInfo.intitialForce *= 200.0f;     

                World::AddPickUp(createInfo);
                //std::cout << "Dropped weapon!  weapon name: '" << weaponState.name << "' pickup name: '" << weaponInfo->pickupName << "'\n";
            }
        }
    }
}

void Player::UpdateMelleBulletWave(float deltaTime) {
    if (!m_meleeBulletWaveState.active) return;

    std::cout << "Time: " << m_meleeBulletWaveState.time << "\n";

    m_meleeBulletWaveState.time += deltaTime;

    // Have we started yet? Then increment the internal counter
    if (m_meleeBulletWaveState.time > m_meleeBulletWaveState.startTime) {
        m_meleeBulletWaveState.intervalCounter += deltaTime;
    }

    // Are we done?
    if (m_meleeBulletWaveState.time >= m_meleeBulletWaveState.maxTime) {
        m_meleeBulletWaveState.active = false;
    }

    // Time to spawn a bullet?
    if (m_meleeBulletWaveState.intervalCounter >= m_meleeBulletWaveState.intervalDuration) {
        m_meleeBulletWaveState.intervalCounter = 0.0f;
        m_meleeBulletWaveState.spawnCountThisWave++;

        for (int i = -2; i < 3; i++) {

            glm::vec3 bulletOrigin = GetCameraPosition() + (GetCameraRight() * 0.1f);
            bulletOrigin -= (GetCameraRight() * 0.05f) * glm::vec3(m_meleeBulletWaveState.spawnCountThisWave);
            bulletOrigin += (GetCameraUp() * 0.05f) * glm::vec3(i);

            BulletCreateInfo createInfo;
            createInfo.origin = bulletOrigin;
            createInfo.direction = GetCameraForward();
            createInfo.weaponIndex = -1;
            createInfo.damage = 100;
            createInfo.ownerObjectId = m_playerId;
            createInfo.rayLength = 1.5f;
            createInfo.createsDecals = false;
            createInfo.createsFollowThroughBulletOnGlassHit = false;
            createInfo.playsPiano = false;
            createInfo.createsDecalTexturePaintedWounds = false;

            World::AddBullet(createInfo);
            //Renderer::DrawLine(createInfo.origin, createInfo.origin + createInfo.direction * 0.5f, GREEN);
        }
    }
}

void Player::BeginMeleeBulletWave() {
    std::cout << "Begin Melee Bullet Wave\n";
    m_meleeBulletWaveState.active = true;
    m_meleeBulletWaveState.time = 0.0f;
    m_meleeBulletWaveState.intervalDuration = 0.01f;
    m_meleeBulletWaveState.startTime = 0.05;
    m_meleeBulletWaveState.maxTime = 0.2f;
    m_meleeBulletWaveState.intervalCounter = 0.0f; 
    m_meleeBulletWaveState.spawnCountThisWave = 0;
}