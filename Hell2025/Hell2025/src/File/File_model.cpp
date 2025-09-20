#include "File.h"
#include "Util.h"

#include <fstream>
#include <string>
#include <cstdint>
#include <chrono>
#include <cstring> // For std::memset, std::memcpy
#include <string>
#include <cstddef> // For size_t

#define PRINT_MODEL_HEADERS_ON_READ 0
#define PRINT_MODEL_HEADERS_ON_WRITE 0
#define PRINT_MESH_HEADERS_ON_READ 0
#define PRINT_MESH_HEADERS_ON_WRITE 0
#define PRINT_ARMATURE_HEADERS_ON_READ 0

namespace File {

    ModelHeaderV2 ReadModelHeaderV2(const std::string& filepath) {
        ModelHeaderV2 header{};
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cout << "ReadModelHeaderV2(): open failed " << filepath << "\n";
            return {};
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() < std::streamoff(sizeof(ModelHeaderV2))) {
            std::cout << "ReadModelHeaderV2(): file too small " << filepath << "\n";
            return {};
        }
        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            std::cout << "ReadModelHeaderV2(): read failed " << filepath << "\n";
            return {};
        }

        if (std::memcmp(header.signature, HELL_MODEL_SIGNATURE, sizeof(HELL_MODEL_SIGNATURE) - 1) != 0) {
            std::cout << "ReadModelHeaderV2(): bad signature\n";
            return {};
        }
        if (header.version != 2) {
            std::cout << "ReadModelHeaderV2(): wrong version " << header.version << "\n";
            return {};
        }

        return header;
    }

    ModelHeaderV3 ReadModelHeaderV3(const std::string& filepath) {
        ModelHeaderV3 header{};
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "ReadModelHeaderV3(): open failed " << filepath << "\n";
            return {};
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() < std::streamoff(sizeof(ModelHeaderV3))) {
            std::cerr << "ReadModelHeaderV3(): file too small " << filepath << "\n";
            return {};
        }
        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            std::cerr << "ReadModelHeaderV3(): read failed " << filepath << "\n";
            return {};
        }

        if (std::memcmp(header.signature, HELL_MODEL_SIGNATURE, sizeof(HELL_MODEL_SIGNATURE) - 1) != 0) {
            std::cerr << "ReadModelHeaderV3(): bad signature\n";
            return {};
        }
        if (header.version != 3) {
            std::cerr << "ReadModelHeaderV3(): wrong version " << header.version << "\n";
            return {};
        }

        return header;
    }

    void ExportModelV2(const ModelData& modelData) {
        std::string outputPath = "res/models/" + modelData.name + ".model";
        std::ofstream file(outputPath, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Failed to open file for writing: " << outputPath << "\n";
            return;
        }

        // Fill the header
        ModelHeaderV2 modelHeader;
        modelHeader.version = 2;
        modelHeader.meshCount = modelData.meshCount;
        modelHeader.timestamp = modelData.timestamp;
        modelHeader.aabbMin = modelData.aabbMin;
        modelHeader.aabbMax = modelData.aabbMax;

        MemCopyFileSignature(modelHeader.signature, HELL_MODEL_SIGNATURE);

        // Write the header
        file.write(reinterpret_cast<const char*>(&modelHeader), sizeof(ModelHeaderV2));

        //#if PRINT_MODEL_HEADERS_ON_WRITE
        PrintModelHeader(modelHeader, "Wrote model header: " + outputPath);
        //#endif

        // Write the mesh data
        for (const MeshData& meshData : modelData.meshes) {
            MeshHeaderV2 meshHeader;
            meshHeader.vertexCount = (uint32_t)meshData.vertices.size();
            meshHeader.indexCount = (uint32_t)meshData.indices.size();
            meshHeader.aabbMin = meshData.aabbMin;
            meshHeader.aabbMax = meshData.aabbMax;
            meshHeader.parentIndex = -1;
            meshHeader.localTransform = glm::mat4(1.0f);
            meshHeader.inverseBindTransform = glm::mat4(1.0f);
            MemCopyFileSignature(meshHeader.signature, HELL_MESH_SIGNATURE);
            MemCopyName(meshHeader.name, meshData.name);
            std::cout << meshData.name << " == " << meshHeader.name << "\n";

            // Write the mesh header
            file.write(reinterpret_cast<const char*>(&meshHeader), sizeof(MeshHeaderV2));

            // Write the data
            file.write(reinterpret_cast<const char*>(meshData.vertices.data()), meshData.vertices.size() * sizeof(Vertex));
            file.write(reinterpret_cast<const char*>(meshData.indices.data()), meshData.indices.size() * sizeof(uint32_t));

            //#if PRINT_MESH_HEADERS_ON_WRITE
            PrintMeshHeader(meshHeader, "Wrote mesh: " + meshData.name);
            //#endif
        }
        file.close();
        std::cout << "Exported: " << outputPath << "\n";
    }

    ModelData ImportModel(const std::string& filepath) {
        ModelData modelData;

        // Bail if file does not exist
        if (!Util::FileExists(filepath)) {
            std::cout << "File::ImportMovel() failed: " << filepath << " does not exist\n";
            return modelData;
        }

        // Validate signature
        std::string signature = ReadFileHeaderSignature(filepath);
        if (!ValidateSignature(signature, HELL_MODEL_SIGNATURE)) {
            std::cout << "File::ImportMovel() invalid ModelHeader signature '" << signature << "' for " << filepath << "\n";
            return modelData;
        }

        // Validate version
        uint32_t version = ReadFileHeaderVersion(filepath);
        if (version == 0) {
            std::cout << "File::ImportMovel() invalid ModelHeader version '" << version << "' for " << filepath << "\n";
            return modelData;
        }

        // Get FileInfo to get the model name
        FileInfo fileInfo = Util::GetFileInfoFromPath(filepath);

        // Attempt to load the file
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "File::ImportMovel() failed: could not open: " << filepath << "\n";
            return modelData;
        }

        // Read the header
        if (version == 2) {
            ModelHeaderV2 modelHeader = ReadModelHeaderV2(filepath);
            modelData.meshCount = modelHeader.meshCount;
            modelData.armatureCount = 0;
            modelData.meshes.resize(modelHeader.meshCount);
            modelData.name = fileInfo.name;
            modelData.aabbMin = modelHeader.aabbMin;
            modelData.aabbMax = modelHeader.aabbMax;
            modelData.timestamp = modelHeader.timestamp;
            
            file.seekg(sizeof(ModelHeaderV2), std::ios::beg);

            #if PRINT_MODEL_HEADERS_ON_READ
            PrintModelHeader(modelHeader, "Read model header in: " + filepath);
            #endif
        }
        else if (version == 3) {
            ModelHeaderV3 modelHeader = ReadModelHeaderV3(filepath);
            modelData.meshCount = modelHeader.meshCount;
            modelData.armatureCount = modelHeader.armatureCount;
            modelData.meshes.resize(modelHeader.meshCount);
            modelData.name = fileInfo.name;
            modelData.aabbMin = modelHeader.aabbMin;
            modelData.aabbMax = modelHeader.aabbMax;
            modelData.timestamp = modelHeader.timestamp;

            file.seekg(sizeof(ModelHeaderV3), std::ios::beg);

            #if PRINT_MODEL_HEADERS_ON_READ
            PrintModelHeader(modelHeader, "Read model header in: " + filepath);
            #endif
        }
        else {
            std::cout << "File::ImportMovel() failed to load '" << fileInfo.name << ".model, unsupported version '" << version << "'\n";
        }

        // Resize meshes vector ready for import
        modelData.meshes.resize(modelData.meshCount);

        // Load each mesh
        for (uint32_t i = 0; i < modelData.meshCount; ++i) {
            MeshData& meshData = modelData.meshes[i];

            // Read the header
            MeshHeaderV2 meshHeader = {};
            file.read(reinterpret_cast<char*>(&meshHeader), sizeof(MeshHeaderV2));

            #if PRINT_MESH_HEADERS_ON_READ
            PrintMeshHeader(meshHeader, "Read mesh: " + meshData.name);
            #endif

            meshData.name.assign(meshHeader.name, strnlen(meshHeader.name, sizeof(meshHeader.name)));
            meshData.vertexCount = meshHeader.vertexCount;
            meshData.indexCount = meshHeader.indexCount;
            meshData.vertices.resize(meshData.vertexCount);
            meshData.indices.resize(meshData.indexCount);
            meshData.aabbMin = meshHeader.aabbMin;
            meshData.aabbMax = meshHeader.aabbMax;
            meshData.parentIndex = meshHeader.parentIndex;
            meshData.localTransform = meshHeader.localTransform;
            meshData.inverseBindTransform = meshHeader.inverseBindTransform;
            file.read(reinterpret_cast<char*>(meshData.vertices.data()), meshHeader.vertexCount * sizeof(Vertex));
            file.read(reinterpret_cast<char*>(meshData.indices.data()), meshHeader.indexCount * sizeof(uint32_t));
        }

        // Load each armature
        modelData.armatures.resize(modelData.armatureCount);

        for (uint32_t i = 0; i < modelData.armatureCount; ++i) {
            ArmatureData& armatureData = modelData.armatures[i];

            ArmatureHeader armatureHeader = {};
            file.read(reinterpret_cast<char*>(&armatureHeader), sizeof(ArmatureHeader));
        
            #if PRINT_ARMATURE_HEADERS_ON_READ
            PrintArmatureHeader(armatureHeader, "Read armature [" + std::to_string(i) + "] " + fileInfo.name + ".model");
            #endif

            armatureData.name.assign(armatureHeader.name, strnlen(armatureHeader.name, sizeof(armatureHeader.name)));
            armatureData.boneCount = armatureHeader.boneCount;
            armatureData.bones.resize(armatureHeader.boneCount);
            file.read(reinterpret_cast<char*>(armatureData.bones.data()), armatureHeader.boneCount * sizeof(Bone));
        }

        if (version == 3 && true) {
            std::cout << "\n";
            std::cout << fileInfo.name << ".model\n";
            std::cout << "meshCount: " << modelData.meshCount << "\n";
            std::cout << "armatureCount: " << modelData.armatureCount << "\n";
            std::cout << "\n";

            for (MeshData& meshData : modelData.meshes) {
                std::cout << "- " << meshData.name << "\n";
                std::cout << "- " << meshData.vertexCount << " verts\n";
                std::cout << "- " << meshData.indexCount << " indices\n";
            }

            for (ArmatureData& armatureData : modelData.armatures) {
                std::cout << "- " << armatureData.name << "\n";
                std::cout << "- " << armatureData.boneCount << " bones\n";

                for (Bone& bone : armatureData.bones) {
                    std::cout << "Name: " << bone.name << "\n";
                    std::cout << "Parent: " << bone.parentIndex << "\n";
                    std::cout << "Local Rest\n";
                    std::cout << Util::Mat4ToString(bone.localRestPose);
                    std::cout << "\nInverse Bind Pose\n";
                    std::cout << Util::Mat4ToString(bone.localRestPose);
                    std::cout << "\n\n";
                }
            }
        }


        file.close();

        return modelData;
    }
}