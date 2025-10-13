#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"

struct PictureFrame {
    PictureFrame() = default;
    void Init(PictureFrameCreateInfo createInfo);
    void SelectRandomPicture();
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void UpdateRenderItems();

    const uint64_t& GetObjectId() const                     { return m_objectId; }
    const glm::vec3& GetPosition() const                    { return m_createInfo.position; }
    const glm::vec3& GetRotation() const                    { return m_createInfo.rotation; }
    const glm::vec3& GetScale() const                       { return m_createInfo.scale; }
    const PictureFrameCreateInfo& GetCreateInfo() const     { return m_createInfo; }
    const PictureFrameType& GetType() const                 { return m_createInfo.type; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }

private:
    std::string m_pictureTextureName = "CheckerBoard";
    uint64_t m_objectId = 0;
    PictureFrameCreateInfo m_createInfo;
    std::vector<RenderItem> m_renderItems;
};