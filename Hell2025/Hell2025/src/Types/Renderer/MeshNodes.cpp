#include "MeshNodes.h"
#include "AssetManagement/AssetManager.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Managers/OpenableManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util.h"

void MeshNodes::InitFromModel(const std::string & modelName) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (model) {
        InitFromModel(model);
    }
    else {
        CleanUp();
        std::cout << "MeshNodes::InitFromModel() failed: '" << modelName << "' does not exist!\n";
    }
}

void MeshNodes::InitFromModel(Model* model) {
    CleanUp();

    if (!model) return;

    m_modelName = model->GetName();
    m_nodeCount = model->GetMeshCount();
    int32_t materialIndex = AssetManager::GetMaterialIndexByName(DEFAULT_MATERIAL_NAME);

    m_blendingModes.resize(m_nodeCount, BlendingMode::BLEND_DISABLED);
    m_materialIndices.resize(m_nodeCount, materialIndex);
    m_transforms.resize(m_nodeCount, Transform());
    m_localParentIndices.resize(m_nodeCount, -1);
    m_localTransforms.resize(m_nodeCount, glm::mat4(1.0f));
    m_inverseBindTransforms.resize(m_nodeCount, glm::mat4(1.0f));
    m_modelMatrices.resize(m_nodeCount, glm::mat4(1.0f));
    m_objectTypes.resize(m_nodeCount, ObjectType::UNDEFINED);
    m_objectIds.resize(m_nodeCount, 0);
    m_worldspaceAabbs.resize(m_nodeCount);

    // Map mesh names to their local index and extra parent index and transforms
    for (int i = 0; i < m_nodeCount; i++) {
        int meshIndex = model->GetMeshIndices()[i];

        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh) {
            m_globalMeshIndices.push_back(meshIndex);
            m_localIndexMap[mesh->GetName()] = i;
            m_localParentIndices[i] = mesh->parentIndex;
            m_localTransforms[i] = mesh->localTransform;
            m_inverseBindTransforms[i] = mesh->inverseBindTransform;

            // Update parent
            if (mesh->parentIndex != -1) {
                Mesh* parentMesh = AssetManager::GetMeshByIndex(meshIndex);
            }
        }
    }

    // If the model contains armatures, store the first one (TODO: allow more maybe)
    if (model->m_modelData.armatures.size() > 0) {
        m_armatureData = model->m_modelData.armatures[0];
    }
}


void MeshNodes::SetObjectTypes(ObjectType type) {
    for (ObjectType& objectType : m_objectTypes) {
        objectType = type;
    }
}

void MeshNodes::SetObjectIds(uint64_t id) {
    for (uint64_t& objectId : m_objectIds) {
        objectId = id;
    }
}

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
    m_objectTypes.clear();
    m_objectIds.clear();
    m_renderItems.clear();
}

void MeshNodes::SetTransformByMeshName(const std::string& meshName, Transform transform) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
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

    for (int i = 0; i < m_materialIndices.size(); i++) {
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

        if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
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

        if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
            m_blendingModes[nodeIndex] = blendingMode;
        }
    }
}

void MeshNodes::SetObjectTypeByMeshName(const std::string& meshName, ObjectType objectType) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
            m_objectTypes[nodeIndex] = objectType;
        }
    }
}

void MeshNodes::SetObjectIdByMeshName(const std::string& meshName, uint64_t id) {
    if (m_localIndexMap.find(meshName) != m_localIndexMap.end()) {
        int nodeIndex = m_localIndexMap[meshName];

        if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
            m_objectIds[nodeIndex] = id;
        }
    }
}

void MeshNodes::SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId) {
    Openable* openable = OpenableManager::GetOpeneableByOpenableId(openableId);
    int nodeIndex = m_localIndexMap[meshName];

    if (nodeIndex >= 0 && nodeIndex < GetNodeCount() && openable) {
        m_objectIds[nodeIndex] = openableId;
        m_objectTypes[nodeIndex] = ObjectType::OPENABLE;
        openable->SetParentObjectId(parentObjectId);
    }
}


void MeshNodes::DrawWorldspaceAABB(glm::vec4 color) {
    Renderer::DrawAABB(m_worldspaceAABB, color);
}

void MeshNodes::DrawWorldspaceAABBs(glm::vec4 color) {
    for (AABB& aabb : m_worldspaceAabbs) {
        Renderer::DrawAABB(aabb, color);
    }
}

RenderItem* MeshNodes::GetRenderItemByNodeIndex(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < m_renderItems.size()) {
        return &m_renderItems[nodeIndex];
    }
    else {
        return nullptr;
    }
}

int32_t MeshNodes::GetGlobalMeshIndex(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return m_globalMeshIndices[nodeIndex];
    }
    else { 
        return -1; 
    }
}

Material* MeshNodes::GetMaterial(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return AssetManager::GetMaterialByIndex(m_materialIndices[nodeIndex]);
    }
    else {
        return AssetManager::GetDefaultMaterial();
    }
}

glm::mat4 MeshNodes::GetLocalTransform(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return m_localTransforms[nodeIndex];
    }
    else {
        return glm::mat4(1.0f);
    }
}

glm::mat4 MeshNodes::GetInverseBindTransform(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return m_inverseBindTransforms[nodeIndex];
    }
    else {
        return glm::mat4(1.0f);
    }
}

void MeshNodes::UpdateHierachy() {
    m_hierarchyChanged = false;
    const float eps = 1e-5f;

    for (int i = 0; i < GetNodeCount(); ++i) {
        int32_t parentIndex = m_localParentIndices[i];

        glm::mat4 newM = (parentIndex != -1)
            ? m_modelMatrices[parentIndex] * m_localTransforms[i] * m_transforms[i].to_mat4()
            : m_localTransforms[i] * m_transforms[i].to_mat4();

        if (!m_hierarchyChanged) {
            if (!Util::Mat4NearlyEqual(newM, m_modelMatrices[i], eps)) {
                m_hierarchyChanged = true;
            }
        }

        m_modelMatrices[i] = newM;
    }
}

void MeshNodes::UpdateRenderItems(const glm::mat4& worldMatrix) {
    UpdateHierachy();

    //if (m_hierarchyChanged) {
        UpdateAABBs(worldMatrix);
    //}

    m_renderItems.clear();
    m_renderItemsBlended.clear();
    m_renderItemsAlphaDiscarded.clear();
    m_renderItemsHairTopLayer.clear();
    m_renderItemsHairBottomLayer.clear();

    for (int i = 0; i < GetNodeCount(); i++) {
        Material* material = GetMaterial(i);
        if (!material) continue;

        int32_t meshIndex = GetGlobalMeshIndex(i);
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (!mesh) continue;

        glm::mat4 meshModelMatrix = GetMeshModelMatrix(i);

        RenderItem renderItem;
        renderItem.objectType = (int)m_objectTypes[i];
        renderItem.modelMatrix = worldMatrix * meshModelMatrix;
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
        //Util::UpdateRenderItemAABB(renderItem);

        BlendingMode blendingMode = m_blendingModes[i];
        switch (blendingMode) {
            case BlendingMode::BLEND_DISABLED:      m_renderItems.push_back(renderItem); break;
            case BlendingMode::BLENDED:             m_renderItemsBlended.push_back(renderItem); break;
            case BlendingMode::ALPHA_DISCARDED:     m_renderItemsAlphaDiscarded.push_back(renderItem); break;
            case BlendingMode::HAIR_TOP_LAYER:      m_renderItemsHairTopLayer.push_back(renderItem); break;
            case BlendingMode::HAIR_UNDER_LAYER:    m_renderItemsHairBottomLayer.push_back(renderItem); break;
            default: break;
        }
    }
}

void MeshNodes::UpdateAABBs(const glm::mat4& worldMatrix) {
    m_worldspaceAabbs.resize(GetNodeCount());

    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());
    bool found = false;

    for (int i = 0; i < GetNodeCount(); ++i) {
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

glm::mat4 MeshNodes::GetMeshModelMatrix(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return m_modelMatrices[nodeIndex];
    }
    else {
        return glm::mat4(1.0f);
    }
}

glm::mat4 MeshNodes::GetBoneLocalMatrix(const std::string& boneName) {
    for (Bone& bone : m_armatureData.bones) {
        if (bone.name == boneName) {
            return bone.localRestPose;
        }
    }
    return glm::mat4(1.0f);
}