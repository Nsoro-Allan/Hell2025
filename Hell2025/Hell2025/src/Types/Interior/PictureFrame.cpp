#include "PictureFrame.h"
#include "AssetManagement/AssetManager.h"
#include "UniqueID.h"
#include "Util.h"

const std::vector<const char*> bigLandscapeImages = {
    //"Picture_RainbowMage",
    "Picture_SHNakedLady",
    "Picture_Minotaur"
};

void PictureFrame::Init(PictureFrameCreateInfo createInfo) {
    m_createInfo = createInfo;
    m_objectId = UniqueID::GetNextObjectId(ObjectType::PICTURE_FRAME);
    SelectRandomPicture();
    UpdateRenderItems();   
}

void PictureFrame::UpdateRenderItems() {
    m_renderItems.clear();

    Material* material = AssetManager::GetMaterialByName("PictureFrame0");
    Material* defaultMaterial = AssetManager::GetDefaultMaterial();

    Transform transform;
    transform.position = m_createInfo.position;
    transform.rotation = m_createInfo.rotation;
    transform.scale = m_createInfo.scale;

    if (m_createInfo.type == PictureFrameType::BIG_LANDSCAPE) {
        Model* model = AssetManager::GetModelByName("PictureFrame_BigLandscape");

        for (int i = 0; i < 2; i++) {
            uint32_t meshIndex = model->GetMeshIndices()[i];
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.modelMatrix = transform.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = meshIndex;
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.normalMapTextureIndex = material->m_normal;
            renderItem.objectType = (int)ObjectType::PICTURE_FRAME;
            if (i == 0) {
                renderItem.baseColorTextureIndex = AssetManager::GetTextureIndexByName(m_pictureTextureName);
                renderItem.rmaTextureIndex = defaultMaterial->m_rma;
                renderItem.normalMapTextureIndex = defaultMaterial->m_normal;
            }
            Util::UpdateRenderItemAABB(renderItem);
            Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        }
    }
}

void PictureFrame::SelectRandomPicture() {
    if (m_createInfo.type == PictureFrameType::BIG_LANDSCAPE) {
        int random = rand() % bigLandscapeImages.size();
        m_pictureTextureName = bigLandscapeImages[random];
    }
    else {
        m_pictureTextureName = "CheckerBoard";
    }
}

void PictureFrame::SetPosition(glm::vec3 position) {
    m_createInfo.position = position;
    UpdateRenderItems();
}

void PictureFrame::SetRotation(glm::vec3 rotation) {
    m_createInfo.rotation = rotation;
    UpdateRenderItems();
}

void PictureFrame::SetScale(glm::vec3 scale) {
    m_createInfo.scale = scale;
    UpdateRenderItems();
}