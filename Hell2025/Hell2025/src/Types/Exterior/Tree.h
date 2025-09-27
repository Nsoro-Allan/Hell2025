#pragma once
#include "HellTypes.h"
#include "HellEnums.h"
#include "CreateInfo.h"
#include "Types/Renderer/Model.h"
#include "Types/Renderer/MeshNodes.h"

struct Tree {
    Tree() = default;
    Tree(TreeCreateInfo createInfo);
    void CleanUp();
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void BeginFrame();
    void MarkAsSelected();
    void Update(float deltaTime);
    void UpdateRenderItems();

    bool IsSelected();

    uint64_t GetObjectId() const                                            { return m_objectId; }
    const TreeCreateInfo& GetCreateInfo() const                             { return m_createInfo; }
    const TreeType& TreeType() const                                        { return m_createInfo.type; }
    const std::string& GetEditorName() const                                { return m_createInfo.editorName; }
    const glm::vec3& GetPosition() const                                    { return m_createInfo.position; }
    const glm::vec3& GetRotation() const                                    { return m_createInfo.rotation; }
    const glm::vec3& GetScale() const                                       { return m_createInfo.scale; }
    const glm::mat4& GetModelMatrix() const                                 { return m_modelMatrix; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_meshNodes.GetRenderItems(); }
    const std::vector<RenderItem>& GetRenderItemsBlended()const             { return m_meshNodes.GetRenderItemsBlended(); }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_meshNodes.GetRenderItemsAlphaDiscarded(); }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_meshNodes.GetRenderItemsHairTopLayer(); }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_meshNodes.GetRenderItemsHairBottomLayer(); }

private:
    void UpdateTransformAndModelMatrix();

    TreeCreateInfo m_createInfo;
    MeshNodes m_meshNodes;
    uint64_t m_objectId = 0;
    uint64_t m_rigidStaticId = 0;
    Model* m_model = nullptr;
    Transform m_transform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    bool m_isSelected = false;
};