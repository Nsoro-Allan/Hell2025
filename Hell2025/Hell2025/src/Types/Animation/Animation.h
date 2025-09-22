#pragma once
#include "HellTypes.h"
#include "LoadingState.h"
#include <map>

struct SQT {
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 positon = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1.0f);
    float timeStamp = -1;
    const char* jointName;
};

struct AnimatedNode {
    AnimatedNode(const char* name) {
        m_nodeName = name;
    }
    std::vector<SQT> m_nodeKeys;
    const char* m_nodeName;
};

struct Animation {
    Animation() = default;
    float m_duration;
    float m_ticksPerSecond;
    float m_finalTimeStamp;
    std::vector<AnimatedNode> m_animatedNodes;
    std::map<const char*, unsigned int> m_NodeMapping;

    void SetFileInfo(FileInfo fileInfo);
    void SetLoadingState(LoadingState value);

    FileInfo GetFileInfo();
    LoadingState GetLoadingState() const;
    float GetTicksPerSecond();
    const std::string& GetName();

private:
    FileInfo m_fileInfo;
    LoadingState m_loadingState { LoadingState::Value::AWAITING_LOADING_FROM_DISK };
};
