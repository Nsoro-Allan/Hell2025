#include "SkinnedModel.h"
#include "AssetManagement/AssetManager.h"
#include "Util/Util.h"
#include <mutex>

void SkinnedModel::BakeToAssetManager() {
    m_vertexCount = m_skinnedModelData.vertexCount;
    m_indexCount = m_skinnedModelData.indexCount;
    m_boneOffsets = m_skinnedModelData.boneOffsets;
    m_nodes = m_skinnedModelData.nodes;
    m_boneMapping = m_skinnedModelData.boneMapping;

    for (SkinnedMeshData& skinnedMeshData : m_skinnedModelData.meshes) {
        const std::string& meshName = skinnedMeshData.name;
        const glm::vec3 aabbMin = skinnedMeshData.aabbMin;
        const glm::vec3 aabbMax = skinnedMeshData.aabbMax;
        const uint32_t baseVertexLocal = skinnedMeshData.localBaseVertex;
        std::vector<WeightedVertex>& vertices = skinnedMeshData.vertices;
        std::vector<uint32_t>& indices = skinnedMeshData.indices;
    
        AddMeshIndex(AssetManager::CreateSkinnedMesh(meshName, vertices, indices, baseVertexLocal, aabbMin, aabbMax, skinnedMeshData.requiresSkinning));
    }

    // Store bone node indices
    m_boneNodeIndices.assign(m_boneMapping.size(), -1);
    for (int nodeIdx = 0; nodeIdx < GetNodeCount(); ++nodeIdx) {
        const auto& name = m_nodes[nodeIdx].name;
        auto it = m_boneMapping.find(name);
        if (it != m_boneMapping.end()) {
            m_boneNodeIndices[it->second] = nodeIdx;
        }
        m_nodeMapping[name] = nodeIdx;
    }
}

void SkinnedModel::SetFileInfo(FileInfo fileInfo) {
    m_fileInfo = fileInfo;
}

bool SkinnedModel::BoneExists(const std::string& boneName) {
    return m_boneMapping.find(boneName) != m_boneMapping.end();
}

const FileInfo& SkinnedModel::GetFileInfo() {
    return m_fileInfo;
}

void SkinnedModel::SetVertexCount(uint32_t vertexCount) {
    m_vertexCount = vertexCount;
}

const std::string& SkinnedModel::GetName() {
    return m_fileInfo.name;
}

void SkinnedModel::AddMeshIndex(uint32_t index) {
    m_meshIndices.push_back(index);
}

uint32_t SkinnedModel::GetMeshCount() {
    return m_meshIndices.size();
}

std::vector<uint32_t>& SkinnedModel::GetMeshIndices() {
    return m_meshIndices;
}

uint32_t SkinnedModel::GetVertexCount() { 
    return m_vertexCount; 
}

uint32_t SkinnedModel::GetBoneCount() {
    return m_boneMapping.size();
}

uint32_t SkinnedModel::GetNodeCount() {
    return m_nodes.size();
}

void SkinnedModel::SetLoadingState(LoadingState loadingState) {
    m_loadingState = loadingState;
}

LoadingState SkinnedModel::GetLoadingState() const {
    return m_loadingState;
}