#pragma once
#include "Audio/Audio.h"
#include "CreateInfo.h"
#include "HellDefines.h"
#include "HellEnums.h"

struct Openable {
    OpenState m_currentOpenState = OpenState::CLOSED;
    OpenAxis m_openAxis = OpenAxis::TRANSLATE_Z;
    Transform m_transform;
    uint64_t m_parentObjectId = 0;
    std::string m_openingAudio = UNDEFINED_STRING;
    std::string m_closingAudio = UNDEFINED_STRING;
    std::string m_openedAudio = UNDEFINED_STRING;
    std::string m_closedAudio = UNDEFINED_STRING;
    std::string m_lockedAudio = UNDEFINED_STRING;
    bool m_dirty = true;
    bool m_locked = false;
    float m_currentOpenValue = 1;
    float m_minOpenValue = 0;
    float m_maxOpenValue = HELL_PI * 0.5f;
    float m_openSpeed = 1.0f;
    float m_closeSpeed = 1.0f;
    float m_audioVolume = 2.0f;

    void Init(const OpenableCreateInfo& createInfo, uint64_t parentObjectId) {
        m_currentOpenState = createInfo.initialOpenState;
        m_openAxis = createInfo.openAxis;
        m_minOpenValue = createInfo.minOpenValue;
        m_maxOpenValue = createInfo.maxOpenValue;
        m_currentOpenValue = glm::clamp(createInfo.initialOpenValue, m_minOpenValue, m_maxOpenValue);
        m_openSpeed = createInfo.openSpeed;
        m_closeSpeed = createInfo.closeSpeed;
        m_openingAudio = createInfo.openingAudio;
        m_closingAudio = createInfo.closingAudio;
        m_openedAudio = createInfo.openedAudio;
        m_closedAudio = createInfo.closedAudio;
        m_lockedAudio = createInfo.lockedAudio;
        m_audioVolume = createInfo.audioVolume;
        m_parentObjectId = parentObjectId;
    }

    void SetParentObjectId(uint64_t parentObjectId) {
        m_parentObjectId = parentObjectId;
    }

    void Interact() {
        if (m_locked && m_lockedAudio != UNDEFINED_STRING) {
            Audio::PlayAudio(m_lockedAudio, m_audioVolume);
            return;
        }

        if (m_currentOpenState == OpenState::OPEN) {
            m_currentOpenState = OpenState::CLOSING;
            if (m_closingAudio != UNDEFINED_STRING) {
                Audio::PlayAudio(m_closingAudio, m_audioVolume);
            }
        }

        if (m_currentOpenState == OpenState::CLOSED) {
            m_currentOpenState = OpenState::OPENING;
            if (m_openingAudio != UNDEFINED_STRING) {
                Audio::PlayAudio(m_openingAudio, m_audioVolume);
            }
        }
    }

    void Update(float deltaTime) {
        m_dirty = false;

        if (m_currentOpenState == OpenState::CLOSING) {
            m_dirty = true;
            m_currentOpenValue -= m_closeSpeed * deltaTime;

            if (m_currentOpenValue < m_minOpenValue) {
                m_currentOpenValue = m_minOpenValue;
                m_currentOpenState = OpenState::CLOSED;

                if (m_closedAudio != UNDEFINED_STRING) {
                    Audio::PlayAudio(m_closedAudio, m_audioVolume);
                }
            }
        }
        if (m_currentOpenState == OpenState::OPENING) {
            m_dirty = true;
            m_currentOpenValue += m_openSpeed * deltaTime;

            if (m_currentOpenValue > m_maxOpenValue) {
                m_currentOpenValue = m_maxOpenValue;
                m_currentOpenState = OpenState::OPEN;

                if (m_openedAudio != UNDEFINED_STRING) {
                    Audio::PlayAudio(m_openedAudio, m_audioVolume);
                }
            }
        }
        if (m_currentOpenState == OpenState::OPEN) {
            m_currentOpenValue = m_maxOpenValue;
        }
        if (m_currentOpenState == OpenState::CLOSED) {
            m_currentOpenValue = m_minOpenValue;
        }

        m_transform.position.x = 0.0f;
        m_transform.position.y = 0.0f;
        m_transform.position.z = 0.0f;
        m_transform.rotation.x = 0.0f;
        m_transform.rotation.y = 0.0f;
        m_transform.rotation.z = 0.0f;

        // TODO: Add rotate
        switch (m_openAxis) {
            case OpenAxis::TRANSLATE_X:     m_transform.position.x = m_currentOpenValue;  break;
            case OpenAxis::TRANSLATE_Y:     m_transform.position.y = m_currentOpenValue;  break;
            case OpenAxis::TRANSLATE_Z:     m_transform.position.z = m_currentOpenValue;  break;
            case OpenAxis::TRANSLATE_Y_NEG: m_transform.position.y = -m_currentOpenValue; break;
            case OpenAxis::TRANSLATE_X_NEG: m_transform.position.x = -m_currentOpenValue; break;
            case OpenAxis::TRANSLATE_Z_NEG: m_transform.position.z = -m_currentOpenValue; break;
            case OpenAxis::ROTATE_X:        m_transform.rotation.x = m_currentOpenValue;  break;
            case OpenAxis::ROTATE_Y:        m_transform.rotation.y = m_currentOpenValue;  break;
            case OpenAxis::ROTATE_Z:        m_transform.rotation.z = m_currentOpenValue;  break;
            case OpenAxis::ROTATE_X_NEG:    m_transform.rotation.x = -m_currentOpenValue; break;
            case OpenAxis::ROTATE_Y_NEG:    m_transform.rotation.y = -m_currentOpenValue; break;
            case OpenAxis::ROTATE_Z_NEG:    m_transform.rotation.z = -m_currentOpenValue; break;
            default: break;
        }
    }

    bool IsDirty() {
        return m_dirty;
    }
};