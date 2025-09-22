#include "MeshNodes.h"
#include "AssetManagement/AssetManager.h"
#include "Core/OpenStateHandlerManager.h"
#include "Renderer/RenderDataManager.h"
#include "Input/Input.h"
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
    m_openHandlerStateIds.resize(m_nodeCount, 0);

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
    for (int i = 0; i < GetNodeCount(); i++) {
        int32_t parentIndex = m_localParentIndices[i];

        // If this node has an open state handler Id, then retrieve it's transform from the opens state handler hive mind
        if (m_openHandlerStateIds[i] != 0) { // 0 means no handler
            m_transforms[i] = OpenStateHandlerManager::GetOpenStateHandleTransformById(m_openHandlerStateIds[i]);
        }

        if (parentIndex != -1) {
            m_modelMatrices[i] = m_modelMatrices[parentIndex] * m_localTransforms[i] * m_transforms[i].to_mat4();
        }
        else {
            m_modelMatrices[i] = m_localTransforms[i] * m_transforms[i].to_mat4();
        }
    }
}

void MeshNodes::UpdateRenderItems(const glm::mat4& worldMatrix) {
    UpdateHierachy();

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

        //if (mesh->GetName() == "Wire" || mesh->GetName() == "WireBarbed" || mesh->GetName() == "WireDemo") {
        //    material = AssetManager::GetMaterialByName("Tokarev");
        //}
        //if (mesh->GetName() == "FencePoleA" || mesh->GetName() == "FencePoleB" ||
        //    mesh->GetName() == "FenceInnerPoleA" || mesh->GetName() == "FenceInnerPoleB") {
        //    material = AssetManager::GetMaterialByName("TreeLarge_0");
        //}


        if (mesh->GetName() == "ReflectorPole") {
            material = AssetManager::GetMaterialByName("White");
        }
        if (mesh->GetName() == "ReflectorRed") {
            material = AssetManager::GetMaterialByName("Red");
        }


        RenderItem renderItem;
        renderItem.objectType = (int)m_objectTypes[i];
        renderItem.modelMatrix = worldMatrix * meshModelMatrix;
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.normalMapTextureIndex = material->m_normal;
        renderItem.rmaTextureIndex = material->m_rma;


        // If this object is gold, replace basecolor and rma textures with that of gold material, normal map does not change
        if (m_isGold) {
            static Material* goldMaterial = AssetManager::GetMaterialByName("Gold");
            renderItem.baseColorTextureIndex = goldMaterial->m_basecolor;
            renderItem.rmaTextureIndex = goldMaterial->m_rma;
        }

        Util::PackUint64(m_objectIds[i], renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        Util::UpdateRenderItemAABB(renderItem);

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

glm::mat4 MeshNodes::GetMeshModelMatrix(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < GetNodeCount()) {
        return m_modelMatrices[nodeIndex];
    }
    else {
        return glm::mat4(1.0f);
    }
}