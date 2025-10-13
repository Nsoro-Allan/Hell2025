#include "Light.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "World/World.h"
#include "Util.h"
#include "UniqueID.h"

#include "Ragdoll/RagdollManager.h"

Light::Light(LightCreateInfo createInfo) {   
    m_createInfo = createInfo;

    m_position = createInfo.position;
    m_color = createInfo.color;
    m_radius = createInfo.radius;
    m_strength = createInfo.strength;
    m_type = createInfo.type;

    if (m_type == LightType::LAMP_POST) {
        m_material = AssetManager::GetMaterialByIndex(AssetManager::GetMaterialIndexByName("LampPost"));
        m_model0 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("LampPost"));
        m_transform0.position = m_position;
    }
    if (m_type == LightType::HANGING_LIGHT) {
        m_material = AssetManager::GetMaterialByIndex(AssetManager::GetMaterialIndexByName("Light"));
        m_model0 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("LightHanging"));
        m_transform0.position = m_position;
        PhysXRayResult rayResult = Physics::CastPhysXRay(m_position, glm::vec3(0.0f, 1.0f, 0.0f), 100.0f, RaycastGroup::RAYCAST_ENABLED);
        if (rayResult.hitFound) {
            m_model1 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("LightHangingCord"));
            m_model2 = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("LightHangingMount"));
            m_transform1.position = m_position;
            m_transform1.scale.y = glm::distance(rayResult.hitPosition, m_position);
            m_transform2.position = rayResult.hitPosition;
            //std::cout << "hit found: " << Util::Vec3ToString(rayResult.hitPosition) << "\n";
        }
        else {
            m_model1 = nullptr;
            m_model2 = nullptr;
            //std::cout << "no hit\n";
        }
    }

    m_objectId = UniqueID::GetNext(ObjectType::LIGHT);
}

void Light::Update(float deltaTime) {
    UpdateRenderItems();
    UpdateMatricesAndFrustum();
    UpdateDirtyState();
}


void Light::UpdateDirtyState() {
    if (m_forcedDirty) {
        m_forcedDirty = false;
        m_dirty = true;
        return;
    }

    m_dirty = false;

    // Doors
    for (Door& door : World::GetDoors()) {
        if (door.MovedThisFrame()) {
            for (const RenderItem& renderItem : door.GetRenderItems()) {
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                    m_dirty = true;
                    return;
                }
            }
        }
    }

    // TODO: make this only actually affect lights in range
    // TODO: make this only actually affect lights in range
    // TODO: make this only actually affect lights in range
    for (Toilet& toilet : World::GetToilets()) {
        if (toilet.MovedThisFrame()) {
            m_dirty = true;
            return;
        }
    }

    // Drawers
    for (Drawers& drawers: World::GetDrawers()) {
        if (drawers.MovedThisFrame()) {
            m_dirty = true;
            return;
        }
    }

    bool m_renderEnabled = true;

    // Ragdolls
    auto& ragdolls = RagdollManager::GetRagdolls();
    for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
        RagdollV2& ragdoll = it->second;
        if (ragdoll.RenderingEnabled() && ragdoll.IsInMotion()) {
            AABB aabb = ragdoll.GetWorldSpaceAABB();
            if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                m_dirty = true;
                return;
            }
        }
        it++;
    }
}

void Light::UpdateRenderItems() {
    m_renderItems.clear();

    if (m_model0) {
        for (uint32_t meshIndex : m_model0->GetMeshIndices()) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.objectType = (int)ObjectType::LIGHT;
            renderItem.modelMatrix = m_transform0.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = meshIndex;
            renderItem.castShadows = false;
            if (m_material) {
                renderItem.baseColorTextureIndex = m_material->m_basecolor;
                renderItem.normalMapTextureIndex = m_material->m_normal;
                renderItem.rmaTextureIndex = m_material->m_rma;
            }
            Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
            if (mesh->GetName() == "Lamp") {
                renderItem.emissiveR = m_color.r;
                renderItem.emissiveG = m_color.g;
                renderItem.emissiveB = m_color.b;
            }
        }
    }
   
    if (m_model1) {
        for (uint32_t meshIndex : m_model1->GetMeshIndices()) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.objectType = (int)ObjectType::LIGHT;
            renderItem.modelMatrix = m_transform1.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = meshIndex;
            renderItem.castShadows = false;
            if (m_material) {
                renderItem.baseColorTextureIndex = m_material->m_basecolor;
                renderItem.normalMapTextureIndex = m_material->m_normal;
                renderItem.rmaTextureIndex = m_material->m_rma;
            }
            Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
            if (mesh->GetName() == "Lamp") {
                renderItem.emissiveR = m_color.r;
                renderItem.emissiveG = m_color.g;
                renderItem.emissiveB = m_color.b;
            }
        }
    }
    if (m_model2) {
        for (uint32_t meshIndex : m_model2->GetMeshIndices()) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.objectType = (int)ObjectType::LIGHT;
            renderItem.modelMatrix = m_transform2.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = meshIndex;
            if (m_material) {
                renderItem.baseColorTextureIndex = m_material->m_basecolor;
                renderItem.normalMapTextureIndex = m_material->m_normal;
                renderItem.rmaTextureIndex = m_material->m_rma;
            }
            Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
            if (mesh->GetName() == "Lamp") {
                renderItem.emissiveR = m_color.r;
                renderItem.emissiveG = m_color.g;
                renderItem.emissiveB = m_color.b;
            }
        }
    }

    for (RenderItem& renderItem : m_renderItems) {
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }
}

void Light::SetPosition(glm::vec3 position) {
    m_position = position;
    m_createInfo.position = m_position;
}

Frustum* Light::GetFrustumByFaceIndex(uint32_t faceIndex) {
    if (faceIndex < 0 || faceIndex >= 6) return nullptr;

    return &m_frustum[faceIndex];
}

void Light::ForceDirty() {
    m_forcedDirty = true;
}

void Light::UpdateMatricesAndFrustum() {
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)SHADOW_MAP_HI_RES_SIZE / (float)SHADOW_MAP_HI_RES_SIZE, SHADOW_NEAR_PLANE, m_radius);

    m_viewMatrix[0] = glm::lookAt(m_position, m_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[1] = glm::lookAt(m_position, m_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[2] = glm::lookAt(m_position, m_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_viewMatrix[3] = glm::lookAt(m_position, m_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    m_viewMatrix[4] = glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[5] = glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    m_projectionTransforms[0] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[1] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[2] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_projectionTransforms[3] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    m_projectionTransforms[4] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[5] = projectionMatrix * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    for (int i = 0; i < 6; i++) {
        m_frustum[i].Update(m_projectionTransforms[i]);
    }
}
