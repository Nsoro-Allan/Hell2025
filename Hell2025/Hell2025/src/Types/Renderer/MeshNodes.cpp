#include "MeshNodes.h"
#include "AssetManagement/AssetManager.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Managers/OpenableManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util.h"

void MeshNodes::InitFromModel(uint64_t parentId, const std::string & modelName, const std::vector<MeshNodeCreateInfo>& meshNodeCreateInfoSet) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (!model) {
        Logging::Error() << "MeshNodes::InitFromModel() failed because modelName was not found";
        return;
    }

    CleanUp();

    m_modelName = modelName;
    m_nodeCount = model->GetMeshCount();
    int32_t materialIndex = AssetManager::GetMaterialIndexByName(DEFAULT_MATERIAL_NAME);

    m_blendingModes.resize(m_nodeCount, BlendingMode::BLEND_DISABLED);
    m_materialIndices.resize(m_nodeCount, materialIndex);
    m_transforms.resize(m_nodeCount, Transform());
    m_transformsPreviousFrame.resize(m_nodeCount, Transform());
    m_localParentIndices.resize(m_nodeCount, -1);
    m_localTransforms.resize(m_nodeCount, glm::mat4(1.0f));
    m_inverseBindTransforms.resize(m_nodeCount, glm::mat4(1.0f));
    m_modelMatrices.resize(m_nodeCount, glm::mat4(1.0f));
    //m_objectTypes.resize(m_nodeCount, ObjectType::UNDEFINED);
    m_worldspaceAabbs.resize(m_nodeCount);
    //m_openableIds.resize(m_nodeCount, 0);
    //m_rigidStaticIds.resize(m_nodeCount, 0);
    //m_rigidDynamictIds.resize(m_nodeCount, 0);

    // By default, match that parent id/type. If you later set a node to
    // ObjectType::OPENABLE, ObjectType::PHYSICS_RIGID, ObjectType::PHYSICS_STATIC
    // then the parent id/type will be now contained within that new object.
    m_objectIds.resize(m_nodeCount, parentId); 

    // Force dirty previous frame transforms
    for (size_t i = 0; i < m_nodeCount; i++) {
        m_transformsPreviousFrame[i].position = glm::vec3(666.0f);
    }

    // Map mesh names to their local index and extra parent index and transforms
    for (size_t i = 0; i < m_nodeCount; i++) {
        int meshIndex = model->GetMeshIndices()[i];

        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh) {
            m_globalMeshIndices.push_back(meshIndex);
            m_localIndexMap[mesh->GetName()] = i;
            m_localParentIndices[i] = mesh->parentIndex;
            m_localTransforms[i] = mesh->localTransform;
            m_inverseBindTransforms[i] = mesh->inverseBindTransform;
        }
        else {
            Logging::Error() << "Some weird shit that should never happen happened";
        }
    }

    // If the model contains armatures, store the first one (TODO: allow more maybe)
    if (model->m_modelData.armatures.size() > 0) {
        m_armatureData = model->m_modelData.armatures[0];
    }

    // Apply any mesh node create info
    for (const MeshNodeCreateInfo& createInfo : meshNodeCreateInfoSet) {
        if (!NodeExists(createInfo.meshName)) {
            Logging::Error() << "MeshNodes::InitFromModel() failed to process meshNodeCreateInfoSet, mesh name '" << createInfo.meshName << "' not found";
            continue;
        }
        int nodeIndex = m_localIndexMap[createInfo.meshName];

        SetMaterialByMeshName(createInfo.meshName, createInfo.materialName);
        SetBlendingModeByMeshName(createInfo.meshName, createInfo.blendingMode);
        SetGoldFlag(createInfo.isGold); // REMOVE ME WHEN CARLOS GIVES U BALLER GOLDEN GLOCK TEXTURE SET

        if (createInfo.openable.createMe) {
            uint64_t openableId = OpenableManager::CreateOpenable(createInfo.openable);
            Openable* openable = OpenableManager::GetOpenableByOpenableId(openableId);
            if (openable) {
                openable->SetParentObjectId(m_objectIds[nodeIndex]);
                openable->m_meshName = createInfo.meshName;
                m_objectIds[nodeIndex] = openableId;
                Logging::Fatal() << "SET " << nodeIndex << " to " << m_objectIds[nodeIndex];
            }
        }
    }

}

//void MeshNodes::SetObjectTypes(ObjectType type) {
//    for (ObjectType& objectType : m_objectTypes) {
//        objectType = type;
//    }
//}

void MeshNodes::PrintMeshNames() {
    std::cout << m_modelName << "\n";
    for (uint32_t meshIndex : m_globalMeshIndices) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh) {
            std::cout << "-" << meshIndex << ": " << mesh->GetName() << "\n";
        }
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
    for (uint64_t queryId : m_objectIds) {
        if (queryId == objectId) {
            return true;
        }
    }
    return false;
}

void MeshNodes::CleanUp() {
    m_modelName = "";
    m_nodeCount = 0;
    m_blendingModes.clear();
    m_globalMeshIndices.clear();
    m_materialIndices.clear();
    m_transforms.clear();
    m_localIndexMap.clear();
    m_localParentIndices.clear();
    m_localTransforms.clear();
    m_inverseBindTransforms.clear();
    m_modelMatrices.clear();
    //m_objectTypes.clear();
    m_objectIds.clear();
    m_renderItems.clear();
}

void MeshNodes::SetTransformByMeshName(const std::string& meshName, Transform transform) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
            m_transforms[nodeIndex] = transform;
        }
    }
}

void MeshNodes::SetMeshMaterials(const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        std::cout << "MeshNodes::SetMeshMaterials() failed: '" << materialName << "' not found!\n";
        return;
    }

    for (size_t i = 0; i < m_nodeCount; i++) {
        m_materialIndices[i] = materialIndex;
    }
}

void MeshNodes::SetMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        std::cout << "MeshNodes::SetMaterialByMeshName() failed: '" << materialName << "' not found!\n";
        return;
    }

    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
            m_materialIndices[nodeIndex] = materialIndex;
            return;
        }
    }

    // If you made it this far then your names were wrong. Print everything to help you.
    std::cout << "\n\nFailed to set mesh name '" << meshName << "' to " << " material '" << materialName << "' \n";
    Model* model = AssetManager::GetModelByName(m_modelName);
    AssetManager::PrintModelMeshNames(model);
}

void MeshNodes::SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
            m_blendingModes[nodeIndex] = blendingMode;
        }
    }
}

//void MeshNodes::SetObjectTypeByMeshName(const std::string& meshName, ObjectType objectType) {
//    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
//        int nodeIndex = m_localIndexMap[meshName];
//
//        if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
//            m_objectTypes[nodeIndex] = objectType;
//        }
//    }
//}

void MeshNodes::SetObjectIdByMeshName(const std::string& meshName, uint64_t id) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
            m_objectIds[nodeIndex] = id;
        }
    }
}

//void MeshNodes::SetOpenable(const std::string& meshName, OpenableCreateInfo& openableCreateInfo) {
//    const auto it = m_localIndexMap.find(meshName);
//    if (it == m_localIndexMap.end()) {
//        Logging::Error() << "MeshNodes::SetOpenable failed because meshName '" << meshName << "' not found";
//        return;
//    }
//
//    const int nodeIndex = static_cast<int>(it->second);
//
//    m_openableIds[nodeIndex] = OpenableManager::CreateOpenable(openableCreateInfo);
//    //m_objectIds[nodeIndex] = 
//}

void MeshNodes::SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId) {
    const auto it = m_localIndexMap.find(meshName);
    if (it == m_localIndexMap.end()) {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed: meshName '" << meshName << "' not found\n";
        return;
    }

    Openable* openable = OpenableManager::GetOpenableByOpenableId(openableId);
    if (!openable) {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed: openableId " << openableId << " not found\n";
        return;
    }

    const int nodeIndex = static_cast<int>(it->second);
    if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= m_nodeCount) {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed: nodeIndex out of range\n";
        return;
    }

    openable->SetParentObjectId(m_objectIds[nodeIndex]); // Store a reference id to the parent object that owns this MeshNodes (plural intended)
    m_objectIds[nodeIndex] = openableId;                 // Replace the node id with the openable id
}

void MeshNodes::DrawWorldspaceAABB(glm::vec4 color) {
    Renderer::DrawAABB(m_worldspaceAABB, color);
}

void MeshNodes::DrawWorldspaceAABBs(glm::vec4 color) {
    for (AABB& aabb : m_worldspaceAabbs) {
        Renderer::DrawAABB(aabb, color);
    }
}

const AABB* MeshNodes::GetWorldSpaceAabbByMeshName(const std::string& meshName) {
    const auto it = m_localIndexMap.find(meshName);
    if (it == m_localIndexMap.end()) return nullptr;

    const size_t nodeIndex = it->second;
    return &m_worldspaceAabbs[nodeIndex];
}

int32_t MeshNodes::GetGlobalMeshIndex(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
        return m_globalMeshIndices[nodeIndex];
    }
    else { 
        return -1; 
    }
}

Material* MeshNodes::GetMaterial(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
        return AssetManager::GetMaterialByIndex(m_materialIndices[nodeIndex]);
    }
    else {
        return AssetManager::GetDefaultMaterial();
    }
}

void MeshNodes::UpdateHierarchy() {
    for (size_t i = 0; i < m_nodeCount; ++i) {
        const int32_t parentIndex = m_localParentIndices[i];

        if (parentIndex != -1) {
            m_modelMatrices[i] = m_modelMatrices[parentIndex] * m_localTransforms[i] * m_transforms[i].to_mat4();
        }
        else {
            m_modelMatrices[i] = m_localTransforms[i] * m_transforms[i].to_mat4();
        }
    }
}

void MeshNodes::UpdateRenderItems(const glm::mat4& worldMatrix) {
    // Check if the world matrix changed this frame
    const bool worldMatrixDirty = (!Util::Mat4NearlyEqual(worldMatrix, m_worldMatrixPreviousFrame));

    // Check if hierarchy has been modified by an Openable this frame
    bool hierarchyDirty = false;
    for (size_t i = 0; i < m_nodeCount; ++i) {
        if (!Util::NearlyEqualTransform(m_transforms[i], m_transformsPreviousFrame[i])) {
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

    for (size_t i = 0; i < m_nodeCount; i++) {
        Material* material = GetMaterial(i);
        if (!material) continue;

        int32_t meshIndex = GetGlobalMeshIndex(i);
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (!mesh) continue;

        RenderItem renderItem;
        renderItem.objectType = (int)UniqueID::GetType(m_objectIds[i]);
        renderItem.modelMatrix = worldMatrix * m_modelMatrices[i];
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.normalMapTextureIndex = material->m_normal;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.aabbMin = glm::vec4(m_worldspaceAabbs[i].GetBoundsMin(), 0.0f);
        renderItem.aabbMax = glm::vec4(m_worldspaceAabbs[i].GetBoundsMax(), 0.0f);

        // If this object is gold, replace basecolor and rma textures with that of gold material, normal map does not change
        if (m_isGold) {
            static Material* goldMaterial = AssetManager::GetMaterialByName("Gold");
            renderItem.baseColorTextureIndex = goldMaterial->m_basecolor;
            renderItem.rmaTextureIndex = goldMaterial->m_rma;
        }

        Util::PackUint64(m_objectIds[i], renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);

        switch (m_blendingModes[i]) {
            case BlendingMode::BLEND_DISABLED:    m_renderItems.push_back(renderItem);                 break;
            case BlendingMode::BLENDED:           m_renderItemsBlended.push_back(renderItem);          break;
            case BlendingMode::ALPHA_DISCARDED:   m_renderItemsAlphaDiscarded.push_back(renderItem);   break;
            case BlendingMode::HAIR_TOP_LAYER:    m_renderItemsHairTopLayer.push_back(renderItem);     break;
            case BlendingMode::HAIR_UNDER_LAYER:  m_renderItemsHairBottomLayer.push_back(renderItem);  break;
            default: break;
        }
    }

    // Store previous frame data
    m_worldMatrixPreviousFrame = worldMatrix;
    m_transformsPreviousFrame = m_transforms;
}

void MeshNodes::UpdateAABBs(const glm::mat4& worldMatrix) {
    m_worldspaceAabbs.resize(m_nodeCount);

    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());
    bool found = false;

    for (size_t i = 0; i < m_nodeCount; ++i) {
        int32_t meshIndex = m_globalMeshIndices[i];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (!mesh) continue;

        glm::vec3 localMin = mesh->aabbMin;
        glm::vec3 localMax = mesh->aabbMax;
        glm::vec3 c = 0.5f * (localMin + localMax);
        glm::vec3 e = 0.5f * (localMax - localMin);

        glm::mat4 M = worldMatrix * m_modelMatrices[i];

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

        m_worldspaceAabbs[i] = AABB(worldMin, worldMax);

        min = glm::min(min, worldMin);
        max = glm::max(max, worldMax);
        found = true;
    }

    m_worldspaceAABB = found ? AABB(min, max) : AABB();
}

const glm::mat4& MeshNodes::GetMeshModelMatrix(int nodeIndex) const {
    static glm::mat4 identity = glm::mat4(1.0f);

    if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
        return m_modelMatrices[nodeIndex];
    }
    return identity;
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
    static glm::mat4 identity = glm::mat4(1.0f);

    if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
        return m_localTransforms[nodeIndex];
    }
    return identity;
}

const glm::mat4& MeshNodes::GetInverseBindTransform(int nodeIndex) const {
    static glm::mat4 identity = glm::mat4(1.0f);

    if (nodeIndex >= 0 && nodeIndex < m_nodeCount) {
        return m_inverseBindTransforms[nodeIndex];
    }
    return identity;
}