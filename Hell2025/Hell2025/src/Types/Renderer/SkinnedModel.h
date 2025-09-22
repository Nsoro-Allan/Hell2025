#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "LoadingState.h"
#include "File/FileFormats.h"

#include <map>
#include <vector>

struct SkinnedModel {
    SkinnedModel() = default;

    void BakeToAssetManager();
    void AddMeshIndex(uint32_t index);
    void SetFileInfo(FileInfo fileInfo);
    void SetVertexCount(uint32_t vertexCount);

    void SetLoadingState(LoadingState loadingState);
    LoadingState GetLoadingState() const;

    bool BoneExists(const std::string& boneName);
    const FileInfo& GetFileInfo();
    const std::string& GetName();
    uint32_t GetMeshCount();
    uint32_t GetVertexCount();
    uint32_t GetBoneCount();
    uint32_t GetNodeCount();
    std::vector<uint32_t>& GetMeshIndices();

public:
    std::vector<Node> m_nodes;
    std::vector<glm::mat4> m_boneOffsets;
    std::map<std::string, unsigned int> m_boneMapping;
    std::map<std::string, unsigned int> m_nodeMapping;
    std::vector<int> m_boneNodeIndices;
    SkinnedModelData m_skinnedModelData;

private:
    FileInfo m_fileInfo;
    LoadingState m_loadingState { LoadingState::Value::AWAITING_LOADING_FROM_DISK };
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    std::vector<uint32_t> m_meshIndices;
};
