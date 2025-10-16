#include "MeshNodes.h"
#include "AssetManagement/AssetManager.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Managers/OpenableManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util.h"

MeshNode* MeshNodes::GetMeshNodeByLocalIndex(int32_t nodeIndex) {
    if (nodeIndex < 0 || nodeIndex >= (int32_t)m_meshNodes.size()) return nullptr;
    return &m_meshNodes[nodeIndex];
}

MeshNode* MeshNodes::GetMeshNodeByMeshName(const std::string& meshName) {
    int32_t nodeIndex = GetMeshNodeIndexByMeshName(meshName);
    if (nodeIndex == -1 || nodeIndex >= (int32_t)m_meshNodes.size()) return nullptr;
    return &m_meshNodes[(size_t)nodeIndex];
}

int32_t MeshNodes::GetMeshNodeIndexByMeshName(const std::string& meshName) {
    auto it = m_localIndexMap.find(meshName);
    if (it == m_localIndexMap.end()) return -1;
    return (int32_t)it->second;
}

void MeshNodes::Init(uint64_t parentId, const std::string& modelName, const std::vector<MeshNodeCreateInfo>& meshNodeCreateInfoSet) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (!model) {
        Logging::Error() << "MeshNodes::Init() failed because modelName was not found";
        return;
    }

    CleanUp();

    m_modelName = modelName;
    m_nodeCount = model->GetMeshCount();
    m_meshNodes.resize(m_nodeCount);

    // Defaults
    for (size_t i = 0; i < m_nodeCount; i++) {
        int globalMeshIndex = model->GetMeshIndices()[i];

        Mesh* mesh = AssetManager::GetMeshByIndex(globalMeshIndex);
        if (!mesh) {
            Logging::Error() << "MeshNodes::Init(...) failed to find mesh by global index " << i;
            continue;
        }

        // Map mesh name to local index for easy lookup
        m_localIndexMap[mesh->GetName()] = i;

        MeshNode& meshNode = m_meshNodes[i];
        meshNode.blendingMode = BlendingMode::BLEND_DISABLED;
        meshNode.materialIndex = AssetManager::GetMaterialIndexByName(DEFAULT_MATERIAL_NAME);
        meshNode.transform = Transform();
        meshNode.transformPreviousFrame = Transform(glm::vec3(666.0f)); // Forces dirty
        meshNode.localParentIndex = mesh->parentIndex;
        meshNode.localTransform = mesh->localTransform;
        meshNode.inverseBindTransform = mesh->inverseBindTransform;
        meshNode.modelMatrix = glm::mat4(1.0f);
        meshNode.worldspaceAabb = AABB();
        meshNode.objectId = parentId;
        meshNode.type = MeshNodeType::DEFAULT;
        meshNode.globalMeshIndex = globalMeshIndex;
    }

    // If the model contains armatures, store the first one (TODO: allow more maybe)
    if (model->m_modelData.armatures.size() > 0) {
        m_armatureData = model->m_modelData.armatures[0];
    }

    // Apply any mesh node create info
    for (const MeshNodeCreateInfo& createInfo : meshNodeCreateInfoSet) {

        MeshNode* meshNode = GetMeshNodeByMeshName(createInfo.meshName);
        if (!meshNode) {
            Logging::Error() << "MeshNodes::Init(...) failed to process meshNodeCreateInfoSet, mesh name '" << createInfo.meshName << "' not found";
            continue;
        }
        meshNode->materialIndex = AssetManager::GetMaterialIndexByName(createInfo.materialName);
        meshNode->blendingMode = createInfo.blendingMode;
        meshNode->isGold = createInfo.isGold;

        int nodeIndex = m_localIndexMap[createInfo.meshName];

        if (createInfo.openable.createMe) {
            uint64_t openableId = OpenableManager::CreateOpenable(createInfo.openable);
            Openable* openable = OpenableManager::GetOpenableByOpenableId(openableId);
            if (openable) {
                openable->SetParentObjectId(meshNode->objectId);
                openable->m_meshName = createInfo.meshName;
                meshNode->objectId = openableId;
            }
        }
    }
}

void MeshNodes::PrintMeshNames() {
    std::cout << m_modelName << "\n";

    for (size_t i = 0; i < m_meshNodes.size(); i++) {
        MeshNode& meshNode = m_meshNodes[i];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshNode.globalMeshIndex);
        if (!mesh) continue;
    
        std::cout << "-" << i << ": " << mesh->GetName() << "\n";
    }
}

void MeshNodes::SetGoldFlag(bool flag) {
    m_isGold = flag;
}

bool MeshNodes::NodeExists(const std::string& meshName) {
    const auto it = m_localIndexMap.find(meshName);
    return (it != m_localIndexMap.end());
}

bool MeshNodes::BoneExists(const std::string& boneName) {
    for (Bone& bone : m_armatureData.bones) {
        if (bone.name == boneName)
            return true;
    }
    return false;
}

bool MeshNodes::HasNodeWithObjectId(uint64_t objectId) const {
    for (const MeshNode& meshNode : m_meshNodes) {
        if (meshNode.objectId == objectId) {
            return true;
        }
    }
    return false;
}

void MeshNodes::CleanUp() {
    m_modelName = "";
    m_nodeCount = 0;
    m_meshNodes.clear();
    m_localIndexMap.clear();
    m_renderItems.clear();
}

void MeshNodes::SetTransformByMeshName(const std::string& meshName, Transform transform) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->transform = transform;
    }
}

void MeshNodes::SetMeshMaterials(const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        Logging::Error() << "MeshNodes::SetMeshMaterials() failed: '" << materialName << "' not found";
        return;
    }
    for (MeshNode& meshNode : m_meshNodes) {
        meshNode.materialIndex = materialIndex;
    }
}

void MeshNodes::SetMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        Logging::Error() << "MeshNodes::SetMaterialByMeshName() failed: '" << materialName << "' not found";
        return;
    }
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->materialIndex = materialIndex;
    }
}

void MeshNodes::SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->blendingMode = blendingMode;
    }
}

void MeshNodes::SetObjectIdByMeshName(const std::string& meshName, uint64_t id) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->objectId = id;
    }
}

void MeshNodes::SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId) {
    Openable* openable = OpenableManager::GetOpenableByOpenableId(openableId);
    if (!openable) {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed: openableId " << openableId << " not found\n";
        return;
    }

    // A little messy. Maybe rethink me??? It overwrites objectId
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        openable->SetParentObjectId(meshNode->objectId);
        meshNode->objectId = openableId;
    }
    else {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed because meshName '" << meshName << "' not found\n";
    }
}

void MeshNodes::DrawWorldspaceAABB(glm::vec4 color) {
    Renderer::DrawAABB(m_worldspaceAABB, color);
}

void MeshNodes::DrawWorldspaceAABBs(glm::vec4 color) {
    for (MeshNode& meshNode : m_meshNodes) {
        Renderer::DrawAABB(meshNode.worldspaceAabb, color);
    }
}

const AABB* MeshNodes::GetWorldSpaceAabbByMeshName(const std::string& meshName) {
    const static AABB invalidAabb;

    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        return &meshNode->worldspaceAabb;
    }

    Logging::Error() << "MeshNodes::GetWorldSpaceAabbByMeshName failed because meshName '" << meshName << "' not found\n";
    return &invalidAabb;
}

int32_t MeshNodes::GetGlobalMeshIndex(int nodeIndex) {
    MeshNode* meshNode = GetMeshNodeByLocalIndex(nodeIndex);
    if (meshNode) {
        return meshNode->globalMeshIndex;
    }
    Logging::Error() << "MeshNodes::GetGlobalMeshIndex failed because node index '" << nodeIndex << "' not found\n";
    return -1;
}

uint64_t MeshNodes::GetObjectIdOfFirstOpenableParentNode(int nodeIndex) {
    MeshNode* meshNode = GetMeshNodeByLocalIndex(nodeIndex);
    if (!meshNode) return 0;

    int parentIndex = meshNode->localParentIndex;

    // Loop as long as we have a valid parent
    while (parentIndex != -1) {
        MeshNode* parentMeshNode = GetMeshNodeByLocalIndex(parentIndex);
        if (!parentMeshNode) return 0; // Something went wrong..

        uint64_t parentId = parentMeshNode->objectId;
        ObjectType parentType = UniqueID::GetType(parentId);

        if (parentType == ObjectType::OPENABLE) {
            return parentId;
        }

        // Move to the next parent in the hierarchy
        parentIndex = parentMeshNode->localParentIndex;
    }

    return 0;
}

Material* MeshNodes::GetMaterial(int nodeIndex) {
    MeshNode* meshNode = GetMeshNodeByLocalIndex(nodeIndex);
    if (!meshNode) return AssetManager::GetDefaultMaterial();

    return AssetManager::GetMaterialByIndex(meshNode->materialIndex);
}

void MeshNodes::UpdateHierarchy() {
    for (MeshNode& meshNode : m_meshNodes) {
        MeshNode* parentMeshNode = GetMeshNodeByLocalIndex(meshNode.localParentIndex);
        if (parentMeshNode) {
            meshNode.modelMatrix = parentMeshNode->modelMatrix * meshNode.localTransform * meshNode.transform.to_mat4();
        }
        else {
            meshNode.modelMatrix = meshNode.localTransform * meshNode.transform.to_mat4();
        }
    }
}

void MeshNodes::UpdateRenderItems(const glm::mat4& worldMatrix) {
    // Check if the world matrix changed this frame
    const bool worldMatrixDirty = (!Util::Mat4NearlyEqual(worldMatrix, m_worldMatrixPreviousFrame));

    // Check if hierarchy has been modified by an Openable this frame
    bool hierarchyDirty = false;
    for (MeshNode& meshNode : m_meshNodes) {
        if (!Util::NearlyEqualTransform(meshNode.transform, meshNode.transformPreviousFrame)) {
            hierarchyDirty = true;
            break;
        }
    }    
    if (hierarchyDirty) {
        UpdateHierarchy();
    }

    // If neither are dirty, then no need to recompute RenderItems
    if (!worldMatrixDirty && !hierarchyDirty) {
        m_isDirty = false;
        return;
    }
    m_isDirty = worldMatrixDirty || hierarchyDirty;

    // If you made it this far, then AABBs are now also dirty
    UpdateAABBs(worldMatrix);

    m_renderItems.clear();
    m_renderItemsBlended.clear();
    m_renderItemsAlphaDiscarded.clear();
    m_renderItemsHairTopLayer.clear();
    m_renderItemsHairBottomLayer.clear();

    for (size_t i = 0; i < m_meshNodes.size(); i++) {
        MeshNode& meshNode = m_meshNodes[i];
        Material* material = GetMaterial(i);
        if (!material) continue;
        
        RenderItem renderItem;
        renderItem.objectType = (int)UniqueID::GetType(meshNode.objectId);
        renderItem.modelMatrix = worldMatrix * meshNode.modelMatrix;
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = GetGlobalMeshIndex(i);
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.normalMapTextureIndex = material->m_normal;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.aabbMin = glm::vec4(meshNode.worldspaceAabb.GetBoundsMin(), 0.0f);
        renderItem.aabbMax = glm::vec4(meshNode.worldspaceAabb.GetBoundsMax(), 0.0f);
        renderItem.localMeshNodeIndex = i;

        // If this object is gold, replace basecolor and rma textures with that of gold material, normal map does not change
        if (m_isGold) {
            static Material* goldMaterial = AssetManager::GetMaterialByName("Gold");
            renderItem.baseColorTextureIndex = goldMaterial->m_basecolor;
            renderItem.rmaTextureIndex = goldMaterial->m_rma;
        }

        Util::PackUint64(meshNode.objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);

        switch (meshNode.blendingMode) {
            case BlendingMode::BLEND_DISABLED:    m_renderItems.push_back(renderItem);                 break;
            case BlendingMode::BLENDED:           m_renderItemsBlended.push_back(renderItem);          break;
            case BlendingMode::ALPHA_DISCARDED:   m_renderItemsAlphaDiscarded.push_back(renderItem);   break;
            case BlendingMode::HAIR_TOP_LAYER:    m_renderItemsHairTopLayer.push_back(renderItem);     break;
            case BlendingMode::HAIR_UNDER_LAYER:  m_renderItemsHairBottomLayer.push_back(renderItem);  break;
            default: break;
        }
    }

    // Store previous frame data
    for (MeshNode& meshNode : m_meshNodes) {
        meshNode.transformPreviousFrame = meshNode.transform;
    }
    m_worldMatrixPreviousFrame = worldMatrix;
}

void MeshNodes::UpdateAABBs(const glm::mat4& worldMatrix) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());
    bool found = false;

    for (MeshNode& meshNode : m_meshNodes) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshNode.globalMeshIndex);
        if (!mesh) continue;

        glm::vec3 localMin = mesh->aabbMin;
        glm::vec3 localMax = mesh->aabbMax;
        glm::vec3 c = 0.5f * (localMin + localMax);
        glm::vec3 e = 0.5f * (localMax - localMin);

        glm::mat4 M = worldMatrix * meshNode.modelMatrix;

        glm::vec3 worldCenter = glm::vec3(M * glm::vec4(c, 1.0f));

        glm::vec3 col0 = glm::vec3(M[0]);
        glm::vec3 col1 = glm::vec3(M[1]);
        glm::vec3 col2 = glm::vec3(M[2]);

        glm::vec3 worldExtents =
            glm::abs(col0) * e.x +
            glm::abs(col1) * e.y +
            glm::abs(col2) * e.z;

        glm::vec3 worldMin = worldCenter - worldExtents;
        glm::vec3 worldMax = worldCenter + worldExtents;

        meshNode.worldspaceAabb = AABB(worldMin, worldMax);

        min = glm::min(min, worldMin);
        max = glm::max(max, worldMax);
        found = true;
    }

    m_worldspaceAABB = found ? AABB(min, max) : AABB();
}

const glm::mat4& MeshNodes::GetMeshModelMatrix(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;
    
    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;
    
    return m_meshNodes[i].modelMatrix;
}

const glm::mat4& MeshNodes::GetBoneLocalMatrix(const std::string& boneName) const {
    static glm::mat4 identity = glm::mat4(1.0f);

    for (const Bone& bone : m_armatureData.bones) {
        if (bone.name == boneName) {
            return bone.localRestPose;
        }
    }
    return identity;
}

const glm::mat4& MeshNodes::GetLocalTransform(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].localTransform;
}

const glm::mat4& MeshNodes::GetInverseBindTransform(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].inverseBindTransform; 
}


const std::string& MeshNodes::GetMeshNameByNodeIndex(int nodeIndex) const {
    static std::string notFound = "NOT_FOUND";

    for (const auto& [name, idx] : m_localIndexMap) {
        if (idx == nodeIndex) return name;
    }
    return notFound;
}