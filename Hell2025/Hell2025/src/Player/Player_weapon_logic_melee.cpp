#include "Player.h"
#include "Audio/Audio.h"

void Player::UpdateMeleeLogic(float deltaTime) {
    if (InventoryIsClosed()) {
        if (PressingFire() && CanFireMelee()) {
            FireMelee();
        }
    }
}

void Player::FireMelee() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();

    if (weaponInfo->audioFiles.fire.size()) {
        int rand = std::rand() % weaponInfo->audioFiles.fire.size();
        Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f);
    }

    if (weaponInfo->animationNames.fire.size()) {
        viewWeapon->PlayAnimation("MainLayer", weaponInfo->animationNames.fire, weaponInfo->animationSpeeds.fire);
    }
    BeginMeleeBulletWave();
    m_weaponAction = WeaponAction::FIRE;
}

bool Player::CanFireMelee() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponAction weaponAction = GetCurrentWeaponAction();
    return (
        weaponAction == IDLE ||
        weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber("MainLayer", 5)
    );
}