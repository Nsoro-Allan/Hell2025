// Openable.h
#pragma once
#include "Audio/Audio.h"
#include "CreateInfo.h"
#include "HellConstants.h"
#include "HellEnums.h"
#include "Util.h"
#include "UniqueID.h"
#include "Input/Input.h"

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
    std::string m_prerequisiteOpenMeshName = UNDEFINED_STRING;
    std::string m_prerequisiteClosedMeshName = UNDEFINED_STRING;
    bool m_dirty = true;
    bool m_firstFrame = true;
    bool m_locked = false;
    float m_currentOpenValue = 1;
    float m_minOpenValue = 0;
    float m_maxOpenValue = HELL_PI * 0.5f;
    float m_openSpeed = 1.0f;
    float m_closeSpeed = 1.0f;
    float m_audioVolume = 2.0f;

    void Init(const OpenableCreateInfo& createInfo, uint64_t parentObjectId);
    void SetParentObjectId(uint64_t parentObjectId);
    bool IsInteractable(const glm::vec3& viewPos);
    void Interact();
    void Update(float deltaTime);
    bool IsDirty();
    bool IsOpen();
    bool IsClosed();
};
