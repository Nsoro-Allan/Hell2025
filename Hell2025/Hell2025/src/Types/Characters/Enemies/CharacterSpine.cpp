#include "CharacterSpine.h"
#include "AssetManagement/AssetManager.h"

void CharacterSpine::Init(const std::string& skinnedModelName, const glm::vec3& position) {
    m_segmentCount = SHARK_SPINE_SEGMENT_COUNT;

    SkinnedModel* skinnedModel = AssetManager::GetSkinnedModelByName(skinnedModelName);
    if (!skinnedModel) return;

    for (int i = 0; i < skinnedModel->m_nodes.size(); i++) {
        std::string& nodeName = skinnedModel->m_nodes[i].name;

        if (nodeName == "BN_Head_00") {
            m_spinePositions[0] = position;
            m_spineBoneNames[0] = nodeName;
        }
        else if (nodeName == "BN_Neck_01") {
            m_spinePositions[1] = position;
            m_spineBoneNames[1] = nodeName;
        }
        else if (nodeName == "BN_Neck_00") {
            m_spinePositions[2] = position;
            m_spineBoneNames[2] = nodeName;
        }
        else if (nodeName == "Spine_00") {
            m_spinePositions[3] = position;
            m_spineBoneNames[3] = nodeName;
        }
        else if (nodeName == "BN_Spine_01") {
            m_spinePositions[4] = position;
            m_spineBoneNames[4] = nodeName;
        }
        else if (nodeName == "BN_Spine_02") {
            m_spinePositions[5] = position;
            m_spineBoneNames[5] = nodeName;
        }
        else if (nodeName == "BN_Spine_03") {
            m_spinePositions[6] = position;
            m_spineBoneNames[6] = nodeName;
        }
        else if (nodeName == "BN_Spine_04") {
            m_spinePositions[7] = position;
            m_spineBoneNames[7] = nodeName;
        }
        else if (nodeName == "BN_Spine_05") {
            m_spinePositions[8] = position;
            m_spineBoneNames[8] = nodeName;
        }
        else if (nodeName == "BN_Spine_06") {
            m_spinePositions[9] = position;
            m_spineBoneNames[9] = nodeName;
        }
        else if (nodeName == "BN_Spine_07") {
            m_spinePositions[10] = position;
            m_spineBoneNames[10] = nodeName;
        }
    }

    // Compute distances between spine segments
    m_spinePositions[0].y = 0.0f;

    // Reset height
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        m_spinePositions[i].y = m_spinePositions[0].y;
    }
    // Print names
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        //std::cout << i << ": " << m_spineBoneNames[i] << "\n";
    }
    // Calculate distances
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT - 1; i++) {
        m_spineSegmentLengths[i] = glm::distance(m_spinePositions[i], m_spinePositions[i + 1]);
    }

    m_forward = glm::normalize(m_spinePositions[0] - m_spinePositions[1]);
}

const glm::vec3& CharacterSpine::GetSpinePositionByIndex(int32_t index) {
    static glm::vec3 invalid = glm::vec3(0.0f, 0.0f, 0.0f);

    if (index >= 0 && index < m_segmentCount) {
        return m_spinePositions[index];
    }
    return invalid;
}
