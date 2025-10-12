#pragma once
#include "Audio/Audio.h"
#include "HellDefines.h"
#include "HellEnums.h"

enum OpenAxis {
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    TRANSLATE_X_NEG,
    TRANSLATE_Y_NEG,
    TRANSLATE_Z_NEG,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
};

struct Openable {
    OpenState openState = OpenState::CLOSED;
    OpenAxis openAxis = OpenAxis::TRANSLATE_Z;
    std::string meshName = UNDEFINED_STRING;
    float currentOpenValue = 1;
    float minOpenValue = 0;
    float maxOpenValue = HELL_PI * 0.5f;
    float openSpeed = 1.0f;
    float closeSpeed = 1.0f;
    bool movedThisFrame = false;
    bool locked = false;
    std::string openingAudio = UNDEFINED_STRING;
    std::string closingAudio = UNDEFINED_STRING;
    std::string openedAudio = UNDEFINED_STRING;
    std::string closedAudio = UNDEFINED_STRING;
    std::string lockedAudio = "Locked.wav";
    Transform m_transform;
    float audioVolume = 2.0f;

    void Interact() {
        if (locked) {
            Audio::PlayAudio(lockedAudio, audioVolume);
            return;
        }

        if (openState == OpenState::OPEN) {
            openState = OpenState::CLOSING;
            if (openingAudio != UNDEFINED_STRING) {
                Audio::PlayAudio(openingAudio, audioVolume);
            }
        }

        if (openState == OpenState::CLOSED) {
            openState = OpenState::OPENING;
            if (closingAudio != UNDEFINED_STRING) {
                Audio::PlayAudio(closingAudio, audioVolume);
            }
        }
    }

    void Update(float deltaTime) {
        movedThisFrame = false;

        if (openState == OpenState::CLOSING) {
            movedThisFrame = true;
            currentOpenValue -= closeSpeed * deltaTime;

            if (currentOpenValue < minOpenValue) {
                currentOpenValue = minOpenValue;
                openState = OpenState::CLOSED;

                if (closedAudio != UNDEFINED_STRING) {
                    Audio::PlayAudio(closedAudio, audioVolume);
                }
            }
        }
        if (openState == OpenState::OPENING) {
            movedThisFrame = true;
            currentOpenValue += openSpeed * deltaTime;

            if (currentOpenValue > maxOpenValue) {
                currentOpenValue = maxOpenValue;
                openState = OpenState::OPEN;

                if (openedAudio != UNDEFINED_STRING) {
                    Audio::PlayAudio(openedAudio, audioVolume);
                }
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

        // TODO: Add rotate

        switch (openAxis) {
            case TRANSLATE_X_NEG: m_transform.position.x = -currentOpenValue; break;
            case TRANSLATE_X:     m_transform.position.x = currentOpenValue; break;
            case TRANSLATE_Y_NEG: m_transform.position.y = -currentOpenValue; break;
            case TRANSLATE_Y:     m_transform.position.y = currentOpenValue; break;
            case TRANSLATE_Z_NEG: m_transform.position.z = -currentOpenValue; break;
            case TRANSLATE_Z:     m_transform.position.z = currentOpenValue; break;
            default: break;
        }
    }
};