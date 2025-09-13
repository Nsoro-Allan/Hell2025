#pragma once
#include "Audio/Audio.h"
#include "HellDefines.h"
#include "HellEnums.h"

enum OpenAxis {
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
};

struct OpenStateHandler {
    OpenState openState;
    float currentOpenValue = 1;
    float minOpenValue = 0;
    float maxOpenValue = HELL_PI * 0.5f;
    float openSpeed = 1.0f;
    float closeSpeed = 1.0f;
    bool movedThisFrame = false;
    bool m_locked = false;
    std::string openingAudio = "";
    std::string closingAudio = "";
    std::string openedAudio = "";
    std::string closedAudio = "";
    Transform m_transform;
    OpenAxis m_openAxis = OpenAxis::TRANSLATE_Z;
    float audioVolume = 2.0f;

    void Interact() {
        if (m_locked) {
            Audio::PlayAudio("Locked.wav", audioVolume);
            return;
        }

        if (openState == OpenState::OPEN) {
            Audio::PlayAudio(openingAudio, audioVolume);
            openState = OpenState::CLOSING;
        }
        if (openState == OpenState::CLOSED) {
            Audio::PlayAudio(closingAudio, audioVolume);
            openState = OpenState::OPENING;
        }
    }

    void Update(float deltaTime) {
        movedThisFrame = false;

        if (openState == OpenState::CLOSING) {
            movedThisFrame = true;
            currentOpenValue -= closeSpeed * deltaTime;
            if (currentOpenValue < minOpenValue) {
                openState = OpenState::CLOSED;
                Audio::PlayAudio(closedAudio, audioVolume);
                currentOpenValue = minOpenValue;
            }
        }
        if (openState == OpenState::OPENING) {
            movedThisFrame = true;
            currentOpenValue += openSpeed * deltaTime;
            if (currentOpenValue > maxOpenValue) {
                openState = OpenState::OPEN;
                Audio::PlayAudio(openedAudio, audioVolume);
                currentOpenValue = maxOpenValue;
            }
        }
        if (openState == OpenState::OPEN) {
            currentOpenValue = maxOpenValue;
        }
        if (openState == OpenState::CLOSED) {
            currentOpenValue = minOpenValue;
        }

        m_transform.position.x = 0.0f;
        m_transform.position.y = 0.0f;
        m_transform.position.z = 0.0f;
        m_transform.rotation.x = 0.0f;
        m_transform.rotation.y = 0.0f;
        m_transform.rotation.z = 0.0f;

        if (m_openAxis == TRANSLATE_Z) {
            m_transform.position.z = -currentOpenValue;
        }
    }
};