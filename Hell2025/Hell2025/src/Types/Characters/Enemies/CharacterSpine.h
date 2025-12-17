#pragma once
#include "HellEnums.h"
#include "HellTypes.h"

#define SHARK_SPINE_SEGMENT_COUNT 11

struct CharacterSpine {

    void Init(const std::string& skinnedModelName, const glm::vec3& position);

    const glm::vec3& GetSpinePositionByIndex(int32_t index);

    int32_t m_segmentCount = 0;
    glm::vec3 m_forward = glm::vec3(0.0);

    glm::vec3 m_spinePositions[SHARK_SPINE_SEGMENT_COUNT];
    std::string m_spineBoneNames[SHARK_SPINE_SEGMENT_COUNT];
    float m_spineSegmentLengths[SHARK_SPINE_SEGMENT_COUNT - 1];
};