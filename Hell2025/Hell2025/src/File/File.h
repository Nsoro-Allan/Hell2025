#pragma once
#include "FileFormats.h"
#include <string>

namespace File {

    // Heightmaps
    HeightMapData LoadHeightMap(const std::string filename);
    void SaveHeightMap(const HeightMapData& heightmapData);
    void PrintHeightMapHeader(HeightMapHeader header);

    // Models
    //void ExportModel(const ModelData& modelData);
    void ExportModelV2(const ModelData& modelData);
    ModelData ImportModel(const std::string& filepath);
    ModelHeaderV2 ReadModelHeaderV2(const std::string& filepath);
    ModelHeaderV3 ReadModelHeaderV3(const std::string& filepath);

    // Skinned Models
    void ExportSkinnedModel(const SkinnedModelData& modelData);
    SkinnedModelData ImportSkinnedModel(const std::string& filepath);
    SkinnedModelHeader ReadSkinnedModelHeader(const std::string& filepath);
    
    // BVHs
    void ExportModelBvh(const ModelData& modelData);
    ModelBvhData ImportModelBvh(const std::string& filepath);
    ModelBvhHeader ReadModelBvhHeader(const std::string& filepath);

    // I/O
    void DeleteFile(const std::string& filePath);

    // Time
    uint64_t GetCurrentTimestamp();
    std::string TimestampToString(uint64_t timestamp);
    uint64_t GetLastModifiedTime(const std::string& filePath);

    // Header Util
    uint32_t ReadFileHeaderVersion(const std::string& filepath);
    std::string ReadFileHeaderSignature(const std::string& filepath);

    // Signatures
    void MemCopyName(char* nameBuffer, const std::string& name);
    void MemCopyFileSignature(char* signatureBuffer, const std::string& signatureName);
    bool CompareFileSignature(char* signatureBuffer, const std::string& signatureName);
    bool ValidateSignature(const std::string& signatureQuery, const std::string& signatureName);

    // Debug
    void ExportMeshDataToOBJ(const std::string& filepath, const MeshData& mesh);
    void ExportSkinnedMeshDataToOBJ(const std::string& filepath, const SkinnedMeshData& mesh);
    void PrintSkinnedModelHeader(SkinnedModelHeader header, const std::string& identifier);
    void PrintSkinnedMeshHeader(SkinnedMeshHeader header, const std::string& identifier);
    void PrintModelHeader(ModelHeaderV2 header, const std::string& identifier);
    void PrintMeshHeader(MeshHeaderV2 header, const std::string& identifier);
    void PrintModelBvhHeader(ModelBvhHeader header, const std::string& identifier);
    void PrintMeshBvhHeader(MeshBvhHeader header, const std::string& identifier);
    void PrintArmatureHeader(ArmatureHeader header, const std::string& identifier);
}