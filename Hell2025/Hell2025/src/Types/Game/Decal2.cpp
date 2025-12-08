#pragma once
#include "Decal2.h"
#include "Renderer/Renderer.h"
#include "World/World.h"

#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"

Decal2::Decal2(const Decal2CreateInfo& createInfo) {
    m_createInfo = createInfo;

    m_localPosition = glm::vec3(glm::inverse(GetParentWorldMatrix()) * glm::vec4(createInfo.surfaceHitPosition, 1.0f));
    m_localNormal = glm::vec3(glm::inverse(GetParentWorldMatrix()) * glm::vec4(createInfo.surfaceHitNormal, 0.0f));
      
    // Re-normalize because it got fucked up somewhere further up the chain
    m_localNormal = glm::normalize(m_localNormal);

    // Offset along local normal to avoid z fighting
    m_localPosition += m_localNormal * 0.005f;

    // Determine type
    m_type = DecalType::PLASTER;
    MeshNodes* meshNodes = nullptr;
    if (GenericObject* genericObject = World::GetGenericObjectById(m_createInfo.parentObjectId)) {
        meshNodes = &genericObject->GetMeshNodes();
    }
    if (Window* window = World::GetWindowByObjectId(m_createInfo.parentObjectId)) {
        meshNodes = &window->GetMeshNodes();
    }
    if (meshNodes) {
        if (MeshNode* meshNode = meshNodes->GetMeshNodeByLocalIndex(m_createInfo.localMeshNodeIndex)) {
            m_type = meshNode->decalType;
        }
    }
    // UGLYYYYYYY ^^^


    float scale = 0.1f;

    if (m_type == DecalType::GLASS) {
        m_material = AssetManager::GetMaterialByName("BulletHole_Glass");
        scale = 0.035f * 0.825f;
    }
    else if (m_type == DecalType::PLASTER) {
        m_material = AssetManager::GetMaterialByName("BulletHole_Plaster");
        scale = 0.02f * 0.5f;
    }

    // Compute the local matrix once because it never changes
    float randomRotation = Util::RandomFloat(0.0f, HELL_PI * 2.0f);
    m_localMatrix = glm::translate(glm::mat4(1.0f), m_localPosition);
    m_localMatrix *= Util::RotationMatrixFromForwardVector(m_localNormal, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
    m_localMatrix *= glm::rotate(glm::mat4(1.0f), randomRotation, glm::vec3(0, 0, 1));
    m_localMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(scale));
}

void Decal2::Update() {
    glm::vec3 position = GetParentWorldMatrix() * glm::vec4(m_localPosition, 1.0f);
   // Renderer::DrawPoint(position, OUTLINE_COLOR);

    m_worldNormal = GetParentWorldMatrix() * glm::vec4(m_localNormal, 0.0f);
    m_worldMatrix = GetParentWorldMatrix() * m_localMatrix;

    //Renderer::DrawLine(position, position + (m_worldNormal * 0.05f), WHITE);

    glm::vec3 boundsMin = glm::vec3(-0.5f);
    glm::vec3 boundsMax = glm::vec3(0.5f);
    AABB m_localAABB(boundsMin, boundsMax);

    static int meshIndex = AssetManager::GetMeshIndexByModelNameMeshName("Primitives", "Quad");


    m_renderItem.meshIndex = meshIndex;
    m_renderItem.modelMatrix = m_worldMatrix;
    m_renderItem.inverseModelMatrix = glm::inverse(m_renderItem.modelMatrix);
    m_renderItem.aabbMin = glm::vec4(GetPosition() - m_localAABB.GetBoundsMin(), 1.0);
    m_renderItem.aabbMax = glm::vec4(GetPosition() + m_localAABB.GetBoundsMax(), 1.0);
    m_renderItem.baseColorTextureIndex = m_material->m_basecolor;
    m_renderItem.normalMapTextureIndex = m_material->m_normal;
    m_renderItem.rmaTextureIndex = m_material->m_rma; 
    Util::UpdateRenderItemAABB(m_renderItem);

    //RenderDataManager::SubmitDecalRenderItem(m_renderItem);
    RenderDataManager::SubmitRenderItemsAlphaDiscard({ m_renderItem });

}


const glm::mat4& Decal2::GetParentWorldMatrix() {
    static glm::mat4 identity = glm::mat4(1.0f);

	if (GenericObject* genericObject = World::GetGenericObjectById(m_createInfo.parentObjectId)) {
		MeshNodes& meshNodes = genericObject->GetMeshNodes();
		return meshNodes.GetWorldModelMatrix(m_createInfo.localMeshNodeIndex);
	}
	if (Door* door = World::GetDoorByObjectId(m_createInfo.parentObjectId)) {
		MeshNodes& meshNodes = door->GetMeshNodes();
		return meshNodes.GetWorldModelMatrix(m_createInfo.localMeshNodeIndex);
	}
    
    return identity;
}