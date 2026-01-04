#include "Player.h"
#include "Audio/Audio.h"
#include "Viewport/ViewportManager.h"
#include "HellLogging.h"

void Player::UpdateGunLogic(float deltaTime) {
    if (InventoryIsClosed()) {
        if (PressingADS() && CanEnterADS()) {
            EnterADS();
        }
        if (!PressingADS() && CanLeaveADS()) {
            LeaveADS();
        }
        if (PressingFire() && CanFireGun()) {
            FireGun();
        }
        if (PressedReload() && CanReloadGun()) {
            ReloadGun();
        }
    }
    UpdateGunReloadLogic();
    UpdateSlideLogic();
    UpdateADSLogic(deltaTime);
}

void Player::FireGun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->itemInfoName);

    if (weaponInfo->hasADS && IsInADS()) {
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.adsFire, weaponInfo->animationSpeeds.adsFire);
        m_weaponAction = WeaponAction::ADS_FIRE;
    }
    else {
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.fire, weaponInfo->animationSpeeds.fire);
        m_weaponAction = WeaponAction::FIRE;
    }

    SpawnMuzzleFlash(55.0f, 0.2f);
    SpawnCasing();
    SpawnBullet(0.05f);

    weaponState->ammoInMag--;

    int rand = std::rand() % weaponInfo->audioFiles.fire.size();
    if (weaponState->hasSilencer) {
        Audio::PlayAudio("Silenced.wav", 1.0f, GetWeaponAudioFrequency());
    }
    else {
        Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f, GetWeaponAudioFrequency());
    }
}

void Player::ReloadGun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (GetCurrentWeaponMagAmmo() == 0) {
        Audio::PlayAudio(weaponInfo->audioFiles.reloadEmpty, 0.7f);
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.reloadempty, weaponInfo->animationSpeeds.reloadempty);
        m_weaponAction = RELOAD_FROM_EMPTY;
    }
    else {
        Audio::PlayAudio(weaponInfo->audioFiles.reload, 0.8f);
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.reload, weaponInfo->animationSpeeds.reload);
        m_weaponAction = RELOAD;
    }
    weaponState->awaitingMagReload = true;
}

void Player::UpdateGunReloadLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetCurrentAmmoState();

    if (m_weaponAction != RELOAD && m_weaponAction != RELOAD_FROM_EMPTY) {
        return;
    }

    int frameNumber = 0;
    switch (m_weaponAction) {
        case RELOAD:            frameNumber = weaponInfo->reloadMagInFrameNumber;       break;
        case RELOAD_FROM_EMPTY: frameNumber = weaponInfo->reloadEmptyMagInFrameNumber;  break;
        default: return;
    }

    if (weaponState->awaitingMagReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", frameNumber)) {
        int ammoToGive = std::min(weaponInfo->magSize - weaponState->ammoInMag, ammoState->ammoOnHand);
        weaponState->ammoInMag += ammoToGive;
        ammoState->ammoOnHand -= ammoToGive;
        weaponState->awaitingMagReload = false;
    }
}

void Player::UpdateSlideLogic() {
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetCurrentAmmoState();

    if (!weaponState) {
        Logging::Error() << "weaponState was nullptr, failed calling GetCurrentWeaponState()";
    }
    if (!weaponInfo) {
        Logging::Error() << "weaponState was nullptr, failed calling GetCurrentWeaponInfo()";
    }
    if (!ammoState) {
        Logging::Error() << "weaponState was nullptr, failed calling GetCurrentAmmoState()";
    }

    if (weaponInfo->emptyReloadRequiresSlideOffset && m_weaponAction != RELOAD_FROM_EMPTY && weaponState->ammoInMag == 0) {
        weaponState->requiresSlideOffset = true;
    }
    else {
        weaponState->requiresSlideOffset = false;
    }
}

void Player::UpdateADSLogic(float deltaTime) {
    // Zoom
    float zoomSpeed = 0.075f;
    if (m_weaponAction == ADS_IN || m_weaponAction == ADS_IDLE || m_weaponAction == ADS_FIRE) {
        m_cameraZoom -= zoomSpeed;
    }
    else {
        m_cameraZoom += zoomSpeed;
    }
    m_cameraZoom = std::max(0.575f, m_cameraZoom);
    m_cameraZoom = std::min(1.0f, m_cameraZoom);

    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    viewport->SetPerspective(m_cameraZoom, NEAR_PLANE, FAR_PLANE);
}

void Player::EnterADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.adsIn, weaponInfo->animationSpeeds.adsIn);
    m_weaponAction = ADS_IN;
}

void Player::LeaveADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.adsOut, weaponInfo->animationSpeeds.adsOut);
    m_weaponAction = ADS_OUT;
}

bool Player::CanReloadGun() {
    AmmoState* ammoState = GetCurrentAmmoState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();

    if (!ammoState) return false;
    if (!weaponInfo) return false;
    if (!weaponState) return false;

    return (weaponState->ammoInMag < weaponInfo->magSize && ammoState->ammoOnHand > 0);
}

bool Player::CanFireGun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag > 0) {
        return (
            weaponAction == IDLE              ||
            weaponAction == ADS_IDLE          ||
            weaponAction == ADS_FIRE          && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.adsFire) ||
            weaponAction == DRAWING           && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.draw) ||
            weaponAction == FIRE              && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.fire) ||
            weaponAction == RELOAD            && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.reload) ||
            weaponAction == RELOAD_FROM_EMPTY && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.reloadFromEmpty));
    }
    else {
        std::cout << "Cannot fire gun\n";
        return false;
    }
}

bool Player::CanEnterADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!weaponInfo->hasADS) return false;

    return (
        (m_weaponAction != RELOAD && m_weaponAction != RELOAD_FROM_EMPTY && !IsInADS()) ||

        (m_weaponAction == RELOAD && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.reload)) ||

        (m_weaponAction == RELOAD_FROM_EMPTY && viewWeapon->AnimationIsPastFrameNumber("MainLayer", weaponInfo->animationCancelFrames.reloadFromEmpty)));
}

bool Player::CanLeaveADS() {
    return (m_weaponAction == ADS_IN || m_weaponAction == ADS_IDLE || m_weaponAction == ADS_FIRE);
}

bool Player::IsInADS() {
    return (m_weaponAction == ADS_IN ||
            m_weaponAction == ADS_OUT ||
            m_weaponAction == ADS_IDLE ||
            m_weaponAction == ADS_FIRE);
}

glm::mat4 Player::GetViewWeaponBoneWorldMatrix(const std::string& boneName) {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (!viewWeapon) {
        return glm::mat4(1.0f);
    }

    glm::mat4 worldMatrix = viewWeapon->GetBoneWorldMatrix(boneName);
    
    return worldMatrix;
}