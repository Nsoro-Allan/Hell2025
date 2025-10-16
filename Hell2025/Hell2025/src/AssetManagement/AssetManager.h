#pragma once
#include "HellTypes.h"
#include "File/File.h"
#include "Types/Animation/Animation.h"
#include "Types/Renderer/Mesh.h"
#include "Types/Renderer/Model.h"
#include "Types/Renderer/SkinnedMesh.hpp"
#include "Types/Renderer/SkinnedModel.h"
#include "Types/Renderer/SpriteSheetTexture.h"
#include "Types/Renderer/Texture.h"
#include <span>
#include <string>
#include <vector>
#include <unordered_map>

namespace AssetManager {
    void Init(); 
    void UpdateLoading();
    bool LoadingComplete();
    void AddItemToLoadLog(std::string text);
    std::vector<std::string>& GetLoadLog();

    // Animations
    std::vector<Animation>& GetAnimations();
    Animation* GetAnimationByName(const std::string& name);
    Animation* GetAnimationByIndex(int index, bool printError = true);
    int GetAnimationIndexByName(const std::string& name);

    // Materials
    std::vector<Material>& GetMaterials();
    Material* GetDefaultMaterial();
    Material* GetMaterialByIndex(int index);
    Material* GetMaterialByName(const std::string& name);
    int GetMaterialIndexByName(const std::string& name);
    std::string GetMaterialNameByIndex(int index);

    // Mesh
    std::vector<Mesh>& GetMeshes();
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax, int parentIndex, glm::mat4 localTransform, glm::mat4 inverseBindTransform);
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    int GetMeshIndexByName(const std::string& name);
    int GetMeshIndexByName(const std::string& name);
    int GetQuadZFacingMeshIndex();
    Mesh* GetMeshByName(const std::string& name);
    Mesh* GetMeshByIndex(int index);
    Mesh* GetCubeMesh();
    Mesh* GetQuadZFacingMesh();
    Mesh* GetMeshByModelNameMeshName(const std::string& modelName, const std::string& meshName);
    Mesh* GetMeshByModelNameMeshIndex(const std::string& modelName, uint32_t meshIndex);
    int GetMeshIndexByModelNameMeshName(const std::string& modelName, const std::string& meshName);
    std::vector<Vertex> GetMeshVertices(Mesh* mesh);
    std::span<Vertex> GetMeshVerticesSpan(Mesh* mesh);
    std::span<uint32_t> GetMeshIndicesSpan(Mesh* mesh);
    void CreateMeshBvhs();
    const std::string& GetMeshNameByMeshIndex(int index);

    // Models
    std::vector<Model>& GetModels();
    Model* CreateModel(const std::string& name);
    Model* GetModelByName(const std::string& name);
    Model* GetModelByIndex(int index);
    int GetModelIndexByName(const std::string& name);
    void PrintModelMeshNames(Model* model);

    // Skinned Mesh
    std::vector<SkinnedMesh>& GetSkinnedMeshes();
    SkinnedMesh* GetSkinnedMeshByIndex(int index);
    int CreateSkinnedMesh(const std::string& name, std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices, uint32_t baseVertexLocal, glm::vec3 aabbMin, glm::vec3 aabbMax);
    int GetSkinnedMeshIndexByName(const std::string& name);

    // Textures
    std::vector<Texture>& GetTextures();
    Texture* GetTextureByName(const std::string& name);
    Texture* GetTextureByIndex(int index);
    int GetTextureIndexByName(const std::string& name, bool ignoreWarning = true);
    int GetTextureCount();

    // Spritesheet Textures
    std::vector<SpriteSheetTexture>& GetSpriteSheetTextures();
    SpriteSheetTexture* GetSpriteSheetTextureByName(const std::string& textureName);
    void BuildSpriteSheetTextures();

    // Skinned Model
    std::vector<SkinnedModel>& GetSkinnedModels();
    SkinnedModel* GetSkinnedModelByName(const std::string& name);
    SkinnedModel* GetSkinnedModelByIndex(int index);
    int GetSkinnedModelIndexByName(const std::string& name);

    // Vertex Data
    std::vector<Vertex>& GetVertices();
    std::span<Vertex> GetVerticesSpan(uint32_t baseVertex, uint32_t vertexCount);
    std::vector<uint32_t>& GetIndices();
    std::span<uint32_t> GetIndicesSpan(uint32_t baseIndex, uint32_t indexCount);

    // Building
    void BuildPrimitives();
    void BuildIndexMaps();
    void BuildMaterials();

    // Loading 
    void LoadPendingTexturesAsync();
    void LoadPendingModelsAsync();
    void LoadPendingSkinnedModelsAsync();
    void LoadPendingAnimationsAsync();
    void LoadAnimation(Animation* animation);
    void LoadModel(Model* model);
    void LoadSkinnedModel(SkinnedModel* skinnedModel);
    void LoadTexture(Texture* texture);

    // Baking
    void BakeModels();
    void BakeSkinnedModels();

    // Import/Export
    void ExportMissingModels();
    void ExportMissingSkinnedModels();
    void ExportMissingModelBvhs();

    // BVH
    void CopyInAllLoadedModelBvhData();

    // Vertex data
    std::vector<Vertex>& GetVertices();
    std::vector<uint32_t>& GetIndies();
    std::vector<WeightedVertex>& GetWeightedVertices();
    std::vector<uint32_t>& GetWeightedIndies();

    // Index maps
    std::unordered_map<std::string, int>& GetTextureIndexMap();
    std::unordered_map<std::string, int>& GetMaterialIndexMap();
    std::unordered_map<std::string, int>& GetModelIndexMap();
}