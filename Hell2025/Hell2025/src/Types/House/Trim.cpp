#include "Trim.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Util.h"
#include "UniqueID.h"

void Trim::Init(Transform transform, const std::string& modelName, const std::string& materialName) {
    m_transform = transform;
    m_objectId = UniqueID::GetNext(ObjectType::TRIM);

    Model* model = AssetManager::GetModelByName(modelName);
    Material* material = AssetManager::GetMaterialByName(materialName);

    if (!model) {
        std::cout << "Trim::Init() failed: model name '" << modelName << "' not found\n";
    }

    m_renderItem.modelMatrix = transform.to_mat4();
    m_renderItem.inverseModelMatrix = glm::inverse(m_renderItem.modelMatrix);
    m_renderItem.meshIndex = model->GetMeshIndices()[0];
    m_renderItem.baseColorTextureIndex = material->m_basecolor;
    m_renderItem.rmaTextureIndex = material->m_rma;
    m_renderItem.normalMapTextureIndex = material->m_normal;
    Util::UpdateRenderItemAABB(m_renderItem);
    Util::PackUint64(m_objectId, m_renderItem.objectIdLowerBit, m_renderItem.objectIdUpperBit);
}

void Trim::SubmitRenderItem() {
    RenderDataManager::SubmitRenderItem(m_renderItem);
}