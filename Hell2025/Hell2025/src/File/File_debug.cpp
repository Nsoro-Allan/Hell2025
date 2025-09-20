#include "File.h"
#include "Util.h"

namespace File {
    //void File::PrintModelHeader(ModelHeader header, const std::string& identifier) {
    //    std::cout << identifier << "\n";
    //    std::cout << " Version: " << header.version << "\n";
    //    std::cout << " Mesh Count: " << header.meshCount << "\n";
    //    std::cout << " Name Length: " << header.nameLength << "\n";
    //    std::cout << " Timestamp: " << header.timestamp << "\n\n";
    //    std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
    //    std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
    //}

    void File::PrintModelHeader(ModelHeaderV2 header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Version: " << header.version << "\n";
        std::cout << " Mesh Count: " << header.meshCount << "\n";
        std::cout << " Timestamp: " << header.timestamp << "\n\n";
        std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
        std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
    }

    //void File::PrintMeshHeader(MeshHeader header, const std::string& identifier) {
    //    std::cout << identifier << "\n";
    //    std::cout << " Name Length: " << header.nameLength << "\n";
    //    std::cout << " Vertex Count: " << header.vertexCount << "\n";
    //    std::cout << " Index Count: " << header.indexCount << "\n";
    //    std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
    //    std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
    //}

    void File::PrintMeshHeader(MeshHeaderV2 header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Signature:     " << header.signature << "\n";
        std::cout << " Name:          " << header.name << "\n";
        std::cout << " Parent index:  " << header.parentIndex << "\n";
        std::cout << " Vertex Count:  " << header.vertexCount << "\n";
        std::cout << " Index Count:   " << header.indexCount << "\n";
        std::cout << " AABB min:      " << Util::Vec3ToString(header.aabbMin) << "\n";
        std::cout << " AABB max:      " << Util::Vec3ToString(header.aabbMax) << "\n";
        std::cout << " Local Transform: \n";
        std::cout << Util::Mat4ToString(header.localTransform) << "\n";
        std::cout << " Inverse Bind Tranform: \n";
        std::cout << Util::Mat4ToString(header.inverseBindTransform) << "\n";
        std::cout << "\n";
    }

    void File::PrintSkinnedModelHeader(SkinnedModelHeader header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Version: " << header.version << "\n";
        std::cout << " Name Length: " << header.nameLength << "\n";
        std::cout << " Vertex Count: " << header.vertexCount << "\n";
        std::cout << " Index Count: " << header.indexCount << "\n";
        std::cout << " Mesh Count: " << header.meshCount << "\n";
        std::cout << " Mesh Node Count: " << header.nodeCount << "\n";
        std::cout << " Mesh Bone Count: " << header.boneCount << "\n";
        std::cout << " Timestamp: " << header.timestamp << "\n\n";
    }

    void File::PrintSkinnedMeshHeader(SkinnedMeshHeader header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Name Length: " << header.nameLength << "\n";
        std::cout << " Vertex Count: " << header.vertexCount << "\n";
        std::cout << " Index Count: " << header.indexCount << "\n";
        std::cout << " Local Base Vertex: " << header.localBaseVertex << "\n";
        std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
        std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
    }

    void PrintModelBvhHeader(ModelBvhHeader header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Signature:  " << header.signature << "\n";
        std::cout << " Version:    " << header.version << "\n";
        std::cout << " Mesh Count: " << header.meshCount << "\n";
        std::cout << " Time Stamp: " << Util::TimestampToString(header.timestamp) << "\n\n";
    }

    void PrintMeshBvhHeader(MeshBvhHeader header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Signature:   " << header.signature << "\n";
        std::cout << " Node Count:  " << header.floatCount << "\n";
        std::cout << " Float Count: " << header.nodeCount << "\n\n";
    }

    void PrintArmatureHeader(ArmatureHeader header, const std::string& identifier) {
        std::cout << identifier << "\n";
        std::cout << " Signature:   " << header.signature << "\n";
        std::cout << " Name:  " << header.name << "\n";
        std::cout << " Bone Count: " << header.boneCount << "\n";
        std::cout << "\n";
    }
}