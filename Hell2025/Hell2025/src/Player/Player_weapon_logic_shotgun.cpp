#include "Player.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "World/World.h"

void Player::UpdateShotgunGunLogic(float deltaTime) {
    if (InventoryIsClosed()) {
        if (PressingFire() && CanFireShotgun()) {
            FireShotgun();
        }
        if (PressingFire() && CanDryFireShotgun()) {
            DryFireShotgun();
        }
        if (PressedReload() && CanReloadShotgun()) {
            ReloadShotgun();
        }
        if (PressedWeaponMiscFunction() && CanUnloadShotgun()) {
            UnloadShotgun();
        }
        if (PressedADS() && CanToggleShotgunAuto()) {
            ToggleAutoShotgun();
        }
        if (PressedMelee()) {
            ShotgunMelee();
        }
    }

    UpdateShotgunReloadLogic();
    UpdateShotgunUnloadLogic();
    UpdatePumpAudio();

    if (Input::KeyPressed(HELL_KEY_I)) {
        AmmoState* ammoState = GetCurrentAmmoState();
        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        ammoState->ammoOnHand--;
    }
    if (Input::KeyPressed(HELL_KEY_O)) {
        AmmoState* ammoState = GetCurrentAmmoState();
        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        ammoState->ammoOnHand++;
    }

    // Green shell hack
    static float delayCounter = 0.0f;
    if (delayCounter > 0.0f) {
        delayCounter -= Game::GetDeltaTime();
    }
    if (delayCounter <= 0.0f && (Input::MouseWheelDown() || Input::MouseWheelUp())) {
        WeaponState* weaponState = GetCurrentWeaponState();
        weaponState->shotgunSlug = !weaponState->shotgunSlug;
        Audio::PlayAudio("SPAS_AutoToggle_STOLEN_FROM_HALFLIFE.wav", 1.0f, GetWeaponAudioFrequency());
        delayCounter = 0.1f; // cooldown
    }

    WeaponState* weaponState = GetCurrentWeaponState();
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (weaponState->shotgunSlug) {
        viewWeapon->SetMeshMaterialByMeshName("Shells", "ShellGreen");
        viewWeapon->SetMeshMaterialByMeshName("Shells.001", "ShellGreen");
    }
    else {
        viewWeapon->SetMeshMaterialByMeshName("Shells", "Shell");
        viewWeapon->SetMeshMaterialByMeshName("Shells.001", "Shell");
    }
}

void Player::ToggleAutoShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.toggleAutoShotgun, 1.0f);

    m_weaponAction = WeaponAction::TOGGLING_AUTO;
    weaponState->shotgunInAutoMode = !weaponState->shotgunInAutoMode;
    Audio::PlayAudio("SPAS_AutoToggle_STOLEN_FROM_HALFLIFE.wav", 1.0f, GetWeaponAudioFrequency());

    //if (weaponState->shotgunInAutoMode) {
    //    DisplayInfoText("AUTO SHOTGUN ENABLED");
    //}
    //else {
    //    DisplayInfoText("AUTO SHOTGUN DISABLED");
    //}    
}

void Player::FireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoInfo* ammoInfo = GetCurrentAmmoInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    m_weaponAction = WeaponAction::FIRE;
    weaponState->ammoInMag--;
    weaponState->shotgunShellChambered = weaponState->shotgunInAutoMode && weaponState->ammoInMag > 0;

    // Needs to rack pump?
    if (!weaponState->shotgunInAutoMode && weaponState->ammoInMag > 0) {
        weaponState->shotgunAwaitingPumpAudio = true;
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.fire[0], weaponInfo->animationSpeeds.fire);
    }
    // No pump rack
    else {
        weaponState->shotgunAwaitingPumpAudio = false;
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunFireNoPump, 1.0f);
    }
    SpawnMuzzleFlash(55.0f, 0.3f);
    SpawnCasing(ammoInfo, weaponState->shotgunSlug);
    Audio::PlayAudio(weaponInfo->audioFiles.fire[0], 1.0f, GetWeaponAudioFrequency());

    for (int i = 0; i < 12; i++) {
        SpawnBullet(0.1f);
    }
}


void Player::ShotgunMelee() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();


    m_weaponAction = WeaponAction::SHOTGUN_MELEE;

    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.melee, weaponInfo->animationSpeeds.melee);

    bool meleeHitSomething = false;

    BeginMeleeBulletWave();

    if (!meleeHitSomething) {
        Audio::PlayAudio("Shotgun_Melee_Miss.wav", 1.0f, GetWeaponAudioFrequency());        
    }
    else {
        // TODO
    }

}

void Player::DryFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.dryFire, 1.0f);

    m_weaponAction = WeaponAction::DRY_FIRE;
    Audio::PlayAudio("Dry_Fire_HalfLife.wav", 1.0f, GetWeaponAudioFrequency());
}

void Player::ReloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadStart, 1.0f);
    m_weaponAction = SHOTGUN_RELOAD_BEGIN;
    weaponState->shotgunAwaitingFirstShellReload = true;
    weaponState->shotgunAwaitingSecondShellReload = true;
}

void Player::UnloadShotgun() {
    std::cout << "UnloadShotgun()\n";
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunUnloadStart, weaponInfo->animationSpeeds.shotgunUnloadStart);
    m_weaponAction = SHOTGUN_UNLOAD_BEGIN;
    weaponState->shotgunAwaitingFirstShellReload = true;
    weaponState->shotgunAwaitingSecondShellReload = true;

    Audio::PlayAudio("SPAS_RackIn.wav", 1.0f);
}

void Player::UpdateShotgunUnloadLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponAction weaponAction = GetCurrentWeaponAction();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);

    if (ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_UNLOAD_BEGIN ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_UNLOAD_SINGLE_SHELL ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_UNLOAD_DOUBLE_SHELL) {

        if (weaponState->ammoInMag >= 2) {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunUnloadTwoShells, weaponInfo->animationSpeeds.shotgunUnloadTwoShells);
            m_weaponAction = WeaponAction::SHOTGUN_UNLOAD_DOUBLE_SHELL;
            weaponState->shotgunAwaitingFirstShellReload = true;
            weaponState->shotgunAwaitingSecondShellReload = true;
        }
        else if (weaponState->ammoInMag == 1) {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunUnloadOneShell, weaponInfo->animationSpeeds.shotgunUnloadOneShell);
            m_weaponAction = WeaponAction::SHOTGUN_UNLOAD_SINGLE_SHELL;
            weaponState->shotgunAwaitingFirstShellReload = true;
        }
        else {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunUnloadEnd, weaponInfo->animationSpeeds.shotgunUnloadEnd);
            m_weaponAction = WeaponAction::SHOTGUN_UNLOAD_END;
            Audio::PlayAudio("SPAS_RackOut.wav", 1.0f);
            std::cout << "Audio::PlayAudio(SPAS_RackOut.wav, 1.0f);\n";
        }
    }


    // Audio
    if (weaponAction == WeaponAction::SHOTGUN_UNLOAD_SINGLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 3)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag--;
            ammoState->ammoOnHand++;
        }
    }

    if (weaponAction == WeaponAction::SHOTGUN_UNLOAD_DOUBLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 3)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag--;
            ammoState->ammoOnHand++;
        }
        if (weaponState->shotgunAwaitingSecondShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 13)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingSecondShellReload = false;
            weaponState->ammoInMag--;
            ammoState->ammoOnHand++;
        }
    }

    // This needs reconsidering
    // Specifically, considering the chambering logic, 
    // Coz it's already incorrect before you get to this point
    if (PressedFire() || PressedADS()) {
        if (weaponAction == SHOTGUN_UNLOAD_BEGIN ||
            weaponAction == SHOTGUN_UNLOAD_SINGLE_SHELL ||
            weaponAction == SHOTGUN_UNLOAD_DOUBLE_SHELL ||
            weaponAction == SHOTGUN_UNLOAD_BEGIN ||
            weaponAction == SHOTGUN_UNLOAD_BEGIN ||
            weaponAction == SHOTGUN_UNLOAD_BEGIN) {
            weaponState->shotgunAwaitingPumpAudio = true;
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunUnloadEnd, weaponInfo->animationSpeeds.shotgunUnloadEnd);
            m_weaponAction = SHOTGUN_UNLOAD_END;
        }
    }
}


void Player::UpdateShotgunReloadLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponAction weaponAction = GetCurrentWeaponAction();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);

    // Begin loading anim has complete, time to reload!
    if (ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_RELOAD_BEGIN ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_RELOAD_SINGLE_SHELL ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::SHOTGUN_RELOAD_DOUBLE_SHELL) {

        // Reload 1 shell ONLY if the shotty can only take 1, or you only have 1
        if (weaponState->ammoInMag == 7 && ammoState->ammoOnHand > 0 || ammoState->ammoOnHand == 1) {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadOneShell, weaponInfo->animationSpeeds.shotgunReloadOneShell);
            weaponState->shotgunAwaitingFirstShellReload = true;

            m_weaponAction = SHOTGUN_RELOAD_SINGLE_SHELL;
        }
        // Otherwise do the double reload
        else if (weaponState->ammoInMag <= 6 && ammoState->ammoOnHand >= 2) {
            viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadTwoShells, weaponInfo->animationSpeeds.shotgunReloadTwoShells);
            weaponState->shotgunAwaitingFirstShellReload = true;
            weaponState->shotgunAwaitingSecondShellReload = true;

            m_weaponAction = SHOTGUN_RELOAD_DOUBLE_SHELL;
        }
        // Otherwise reload is done, time to flip the gun back to idle
        else {
            if (IsShellInShotgunChamber()) {
                m_weaponAction = SHOTGUN_RELOAD_END;
                weaponState->shotgunAwaitingPumpAudio = false;
                viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadEnd, weaponInfo->animationSpeeds.shotgunReloadEnd);
            }
            else {
                m_weaponAction = SHOTGUN_RELOAD_END_WITH_PUMP;
                weaponState->shotgunAwaitingPumpAudio = true;
                viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadEndPump, weaponInfo->animationSpeeds.shotgunReloadEndPump);
            }
        }
    }

    // Cancel reload with no shell in chamber ?
    if (PressedFire() || PressedADS()) {
        if (weaponAction == SHOTGUN_RELOAD_BEGIN ||
            weaponAction == SHOTGUN_RELOAD_SINGLE_SHELL ||
            weaponAction == SHOTGUN_RELOAD_DOUBLE_SHELL ||
            weaponAction == SHOTGUN_RELOAD_BEGIN ||
            weaponAction == SHOTGUN_RELOAD_BEGIN ||
            weaponAction == SHOTGUN_RELOAD_BEGIN) {
            weaponState->shotgunAwaitingPumpAudio = true;

            if (IsShellInShotgunChamber()) {
                viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadEnd, weaponInfo->animationSpeeds.shotgunReloadEnd);
                m_weaponAction = SHOTGUN_RELOAD_END;
            }
            else {
                viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.shotgunReloadEndPump, weaponInfo->animationSpeeds.shotgunReloadEndPump);
                m_weaponAction = SHOTGUN_RELOAD_END_WITH_PUMP;
            }
        }
    }

    // Audio
    if (weaponAction == WeaponAction::SHOTGUN_RELOAD_SINGLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 7)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
    }

    if (weaponAction == WeaponAction::SHOTGUN_RELOAD_DOUBLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 7)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
        if (weaponState->shotgunAwaitingSecondShellReload && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 19)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingSecondShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
    }
}

void Player::UpdatePumpAudio() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    auto PlayPumpAudioIfNeeded = [&](WeaponAction action, int frame) {
        if (GetCurrentWeaponAction() == action &&
            viewWeapon->AnimationIsPastFrameNumber("MainLayer", frame) &&
            weaponState->shotgunAwaitingPumpAudio) {

            Audio::PlayAudio(weaponInfo->audioFiles.shotgunPump, 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingPumpAudio = false;
            weaponState->shotgunShellChambered = true;
        }
    };

    PlayPumpAudioIfNeeded(WeaponAction::FIRE, 3);
    PlayPumpAudioIfNeeded(WeaponAction::SHOTGUN_RELOAD_END_WITH_PUMP, 3);
    PlayPumpAudioIfNeeded(WeaponAction::DRAWING_WITH_SHOTGUN_PUMP, 6);
}

bool Player::CanFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag > 0 && weaponState->shotgunShellChambered) {
        return (
            weaponAction == IDLE ||
            weaponAction == FIRE && !weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 21) ||
            weaponAction == FIRE && weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 4));
    }
    else {
        return false;
    }
}

bool Player::CanDryFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag == 0) {
        return (
            weaponAction == IDLE ||
            weaponAction == DRY_FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5) ||
            weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 17)
        );
    }
    else {
        return false;
    }
}

bool Player::CanToggleShotgunAuto() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!weaponInfo->hasAutoSwitch) {
        return false;
    }

    WeaponAction weaponAction = GetCurrentWeaponAction();
    return (
        weaponAction == IDLE ||
        weaponAction == FIRE && !weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 21) ||
        weaponAction == FIRE && weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 4) ||
        weaponAction == TOGGLING_AUTO && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 2)
    );
}

bool Player::CanReloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    WeaponAction weaponAction = GetCurrentWeaponAction();
    
    // If shotty aint full, and you have enough ammo
    if (weaponState->ammoInMag < weaponInfo->magSize && ammoState->ammoOnHand > 0) {
        return (
            // And you are playing an acceptable animation to cancel
            weaponAction == IDLE ||
            weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 22) ||
            weaponAction == DRY_FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5) ||
            weaponAction == SHOTGUN_RELOAD_END && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5) ||
            weaponAction == SHOTGUN_RELOAD_END_WITH_PUMP && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5)
        );
    }
    return false;
}


bool Player::CanUnloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    WeaponAction weaponAction = GetCurrentWeaponAction();

    // If shotty aint full, and you have enough ammo
    if (weaponState->ammoInMag > 0) {
        return (
            // And you are playing an acceptable animation to cancel
            weaponAction == IDLE ||
            weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 22) ||
            weaponAction == DRY_FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5) ||
            weaponAction == SHOTGUN_RELOAD_END && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5) ||
            weaponAction == SHOTGUN_RELOAD_END_WITH_PUMP && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5)
            );
    }
    return false;
}

bool Player::IsShellInShotgunChamber() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shotgunShellChambered;
    }
}

bool Player::ShotgunRequiresPump() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shotgunRequiresPump;
    }
}
