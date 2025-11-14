#pragma once
#include "File/FileFormats.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "LoadingState.h"
#include "Mesh.h"

#include <limits>
#include <string>
#include <vector>

struct Model {
    Model() = default;

    void SetFileInfo(FileInfo fileInfo);
    void AddMeshIndex(uint32_t index);
    void SetName(std::string modelName);
    void SetAABB(glm::vec3 aabbMin, glm::vec3 aabbMax);
    void SetLoadingState(LoadingState loadingState);
    uint32_t GetGlobalMeshIndexByMeshName(const std::string& meshName);

    LoadingState GetLoadingState() const;
    const FileInfo& GetFileInfo() const                       { return m_fileInfo; }
    const size_t GetMeshCount()  const                        { return m_meshIndices.size(); }
    const glm::vec3& GetAABBMin() const                       { return m_aabbMin; }
    const glm::vec3& GetAABBMax() const                       { return m_aabbMax; }
    const glm::vec3& GetExtents() const                       { return m_aabbMax - m_aabbMin; }
    const std::string GetName() const                         { return m_name; }
    const std::vector<uint32_t>& GetMeshIndices() const       { return m_meshIndices; }

    bool m_awaitingLoadingFromDisk = true;
    std::string m_fullPath = "";
    ModelData m_modelData;
    ModelBvhData m_modelBvhData;


private:
    FileInfo m_fileInfo;
    LoadingState m_loadingState{ LoadingState::Value::AWAITING_LOADING_FROM_DISK };
    glm::vec3 m_aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_aabbMax = glm::vec3(-std::numeric_limits<float>::max());
    std::string m_name = "undefined";
    std::vector<uint32_t> m_meshIndices;
    std::unordered_map<std::string, uint32_t> m_meshNameToGlobalMeshIndexMap;
};
