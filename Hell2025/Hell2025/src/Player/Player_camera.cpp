#include "Player.h"
#include "Core/Debug.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include <glm/gtc/noise.hpp> 
#include "Util.h"

void Player::UpdateHeadBob(float deltaTime) {
    if (!HasControl()) return;
  
    bool pressingMoveKey = PressingWalkLeft() || PressingWalkRight() || PressingWalkForward() || PressingWalkBackward();

    if (!CameraIsUnderwater() && !IsWading() && pressingMoveKey) {

        m_headBobTime += deltaTime;

        float frequency = 14.4;
        if (IsCrouching()) {
            frequency *= 0.75f;
        }
        else if (IsRunning()) {
            frequency *= 1.1f;
        }

        float bobIntensity = 0.05f;
        float noiseIntensity = 0.02f;
        m_bobOffsetY = glm::sin(m_headBobTime * frequency) * bobIntensity;
        m_bobOffsetX = glm::sin(m_headBobTime * frequency * 0.5f) * (bobIntensity * 0.5f);
        float noiseOffsetY = glm::perlin(glm::vec2(m_headBobTime * 0.1f, 0.0f)) * noiseIntensity;
        float noiseOffsetX = glm::perlin(glm::vec2(0.0f, m_headBobTime * 0.1f)) * noiseIntensity;
        m_headBob = glm::vec3(m_bobOffsetX + noiseOffsetX, m_bobOffsetY + noiseOffsetY, 0.0f);
    }
    else {
        // TODO: find the values that make a footstep always play on the first step u take
        m_headBobTime = Util::FInterpTo(m_headBobTime, 0.2f, deltaTime, 10);
        m_headBob.x = Util::FInterpTo(m_headBob.x, 0.0f, deltaTime, 10);
        m_headBob.y = Util::FInterpTo(m_headBob.y, 0.0f, deltaTime, 10);
        m_headBob.z = Util::FInterpTo(m_headBob.z, 0.0f, deltaTime, 10);
        m_bobOffsetX = Util::FInterpTo(m_bobOffsetX, 0.0f, deltaTime, 10);
        m_bobOffsetY = Util::FInterpTo(m_bobOffsetY, 0.0f, deltaTime, 10);
    }
}

void Player::UpdateBreatheBob(float deltaTime) {
    if (Util::IsNan(GetCameraUp()) || Util::IsNan(GetCameraRight())) return;

    m_breatheBobTime += deltaTime;

    float breathSpeed = 0.5f;
    float horizontalBreathIntensity = 0.00025f;
    float verticalBreathIntensity = 0.002f;
    float noiseIntensity = 0.0005f;

    float breathOffsetX = glm::sin(m_breatheBobTime * breathSpeed * glm::two_pi<float>()) * horizontalBreathIntensity;
    float breathOffsetY = glm::sin(m_breatheBobTime * breathSpeed * glm::two_pi<float>() * 0.5f) * verticalBreathIntensity;

    float noiseOffsetX = glm::perlin(glm::vec2(m_breatheBobTime * 0.05f, 0.0f)) * noiseIntensity;
    float noiseOffsetY = glm::perlin(glm::vec2(0.0f, m_breatheBobTime * 0.05f)) * noiseIntensity;

    m_breatheBob = GetCameraUp() * (breathOffsetY + noiseOffsetY);
    m_breatheBob += GetCameraRight() * glm::vec3(breathOffsetX + noiseOffsetX);
}

void Player::UpdateCamera(float deltaTime) {
    // Mouselook
    if (!Editor::IsOpen() && m_controlEnabled) {
        float xOffset = (float)InputMulti::GetMouseXOffset(m_mouseIndex);
        float yOffset = (float)InputMulti::GetMouseYOffset(m_mouseIndex);
        m_camera.AddPitch(-yOffset * m_mouseSensitivity);
        m_camera.AddYaw(xOffset * m_mouseSensitivity);        
    }

    // Height
    float crouchDownSpeed = 17.5f;
    float viewHeightTarget = m_crouching ? m_viewHeightCrouching : m_viewHeightStanding;
    m_currentViewHeight = Util::FInterpTo(m_currentViewHeight, viewHeightTarget, deltaTime, crouchDownSpeed);

    static float viewHeightModifer = 0.0f;
    if (!IsPlayingPiano()) {
        if (Input::KeyDown(HELL_KEY_EQUAL)) {
            viewHeightModifer += 1.0f * deltaTime;
        }
        if (Input::KeyDown(HELL_KEY_MINUS)) {
            viewHeightModifer -= 1.0f * deltaTime;
        }
        if (Input::KeyDown(HELL_KEY_BACKSPACE)) {
            viewHeightModifer = 0.0f;
        }
    }

    // Position
    m_camera.SetPosition(GetFootPosition() + glm::vec3(0, m_currentViewHeight + viewHeightModifer, 0) + m_headBob + m_breatheBob);
   
    // Get view weapon camera matrix
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    SkinnedModel* model = viewWeapon->GetSkinnedModel();

    glm::mat4 cameraMatrix = viewWeapon->GetAnimatedTransformByBoneName("camera");
    glm::mat4 cameraBindMatrix = glm::mat4(1);

    for (int i = 0; i < model->m_nodes.size(); i++) {
        if (model->m_nodes[i].name == "camera") {
            cameraBindMatrix = model->m_nodes[i].inverseBindTransform;
        }
    }
    m_viewWeaponCameraMatrix = inverse(cameraBindMatrix) * cameraMatrix;

    // HACK!!!!!!!!!!!!!
    if (GetCurrentWeaponInfo()->name == "AKS74U") {
        glm::mat4 dmMaster = viewWeapon->GetAnimatedTransformByBoneName("Dm-Master");
        m_viewWeaponCameraMatrix = inverse(cameraBindMatrix) * inverse(dmMaster) * cameraMatrix;
    }

    if (!IsAlive()) {
        Ragdoll* ragdoll = GetRagdoll();

        if (ragdoll) {
            m_characterModelAnimatedGameObject.SetAnimationModeToRagdoll();
            ragdoll->ActivatePhysics();
            glm::mat4 headMatrix = ragdoll->GetRigidWorlTransform("CC_Base_Head");
            m_deathCamViewMatrix = glm::inverse(headMatrix);
        }
    }
}
