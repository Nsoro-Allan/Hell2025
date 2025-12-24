#include "Light.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "World/World.h"
#include "Util.h"
#include "UniqueID.h"
#include "Timer.hpp"

#include "Ragdoll/RagdollManager.h"

#include "Core/Game.h"


Light::Light(LightCreateInfo createInfo) {   
    m_createInfo = createInfo;
	m_objectId = UniqueID::GetNextObjectId(ObjectType::LIGHT);
    ConfigureMeshNodes();
}

void Light::Update(float deltaTime) {
    UpdateMatricesAndFrustum();
    UpdateDirtyState();

    if (m_doFlicker) {
        m_lightFlicker.Update(Game::GetDeltaTime() * 10, Game::GetTotalTime() * 10);
        SetColor(m_lightFlicker.m_currentColor * 1.5f);
    }

   //if (m_doFlicker) {
   //    SetColor(m_lightFlicker.m_currentColor);
   //}
   //SetColor(m_lightFlicker.m_currentColor);

}

void Light::ConfigureMeshNodes() {
    // Mount position
    glm::vec3 mountPosition = m_createInfo.position;
	PhysXRayResult rayResult = Physics::CastPhysXRay(m_createInfo.position, glm::vec3(0.0f, 1.0f, 0.0f), 100.0f, RaycastGroup::RAYCAST_ENABLED);
	if (rayResult.hitFound) {
        mountPosition = rayResult.hitPosition;
	}

    // Distance to roof
	float distanceToRoof = glm::distance(mountPosition, m_createInfo.position);

    // Transforms
	Transform worldTransform;
    worldTransform.position = m_createInfo.position;

	Transform localMountTransform;
    localMountTransform.position = glm::vec3(0.0f, distanceToRoof, 0.0f);

	Transform localCordTransform;
    localCordTransform.scale = glm::vec3(1.0f, distanceToRoof, 1.0f);

	glm::mat4 worldMatrix = worldTransform.to_mat4();

	if (m_createInfo.type == LightType::HANGING_LIGHT) {
		std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

		MeshNodeCreateInfo& light = meshNodeCreateInfoSet.emplace_back();
		light.meshName = "Light";
		light.materialName = "Light";
		light.castShadows = false;
        light.emissiveColor = m_createInfo.color;

		MeshNodeCreateInfo& mount = meshNodeCreateInfoSet.emplace_back();
		mount.meshName = "Mount";
		mount.materialName = "Light";
		mount.castShadows = false;

		MeshNodeCreateInfo& cord = meshNodeCreateInfoSet.emplace_back();
		cord.meshName = "Cord";
		cord.materialName = "Light";
		cord.castShadows = false;

        m_meshNodes.Init(m_objectId, "LightHanging", meshNodeCreateInfoSet);
		m_meshNodes.SetTransformByMeshName("Mount", localMountTransform);
		m_meshNodes.SetTransformByMeshName("Cord", localCordTransform);
		m_meshNodes.Update(worldMatrix);
    }
}

void Light::UpdateDirtyState() {

    //float deltaTime = Game::GetDeltaTime();
    //static float time = 0;
    //time += Game::GetDeltaTime();
    //float intensityT = 0;
    //
    //static FireLightFlicker fireLightFlicker;
    //fireLightFlicker.Update(deltaTime, time, m_createInfo.color);
    //SetColor(m_createInfo.color);

    if (m_forcedDirty) {
        m_forcedDirty = false;
        m_dirty = true;
        return;
    }

    m_dirty = false;
    bool printDebug = false;

    for (Door& object : World::GetDoors()) {
        if (object.IsDirty()) {
            for (const RenderItem& renderItem : object.GetRenderItems()) {
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                    m_dirty = true;
                    if (printDebug) std::cout << m_objectId << " Door triggered dirty\n";
                    return;
                }
            }
        }
    }

    for (GenericObject& object : World::GetGenericObjects()) {
        if (object.IsDirty()) {
            for (const RenderItem& renderItem : object.GetRenderItems()) {
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                    m_dirty = true;
                    if (printDebug) std::cout << m_objectId << " GenericObject triggered dirty\n";
                    return;
                }
            }
        }
    }
    for (Piano& object : World::GetPianos()) {
        if (object.IsDirty()) {
            for (const RenderItem& renderItem : object.GetRenderItems()) {
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                    m_dirty = true;
                    if (printDebug) std::cout << m_objectId << " Piano triggered dirty\n";
                    return;
                }
            }
        }
    }

    for (PickUp& object : World::GetPickUps()) {
        if (object.IsDirty()) {
            for (const RenderItem& renderItem : object.GetRenderItems()) {
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
                    m_dirty = true;
                    if (printDebug) std::cout << m_objectId << " PickUp triggered dirty\n";
                    return;
                }
            }
        }
    }

    //bool m_renderEnabled = true;
    //
    //// Ragdolls
    //auto& ragdolls = RagdollManager::GetRagdolls();
    //for (auto it = ragdolls.begin(); it != ragdolls.end(); ) {
    //    RagdollV2& ragdoll = it->second;
    //    if (ragdoll.RenderingEnabled() && ragdoll.IsInMotion()) {
    //        AABB aabb = ragdoll.GetWorldSpaceAABB();
    //        if (aabb.IntersectsSphere(GetPosition(), GetRadius())) {
    //            m_dirty = true;
    //            return;
    //        }
    //    }
    //    it++;
    //}
}

void Light::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    ConfigureMeshNodes();
}

void Light::SetColor(const glm::vec3& color) {
    m_createInfo.color = color;
    ConfigureMeshNodes();
}

void Light::SetRadius(float radius) {
    m_createInfo.radius = radius;
}

void Light::SetStrength(float strength) {
    m_createInfo.strength = strength;
}

Frustum* Light::GetFrustumByFaceIndex(uint32_t faceIndex) {
    if (faceIndex < 0 || faceIndex >= 6) return nullptr;

    return &m_frustum[faceIndex];
}

void Light::ForceDirty() {
    m_forcedDirty = true;
}

void Light::UpdateMatricesAndFrustum() {
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)SHADOW_MAP_HI_RES_SIZE / (float)SHADOW_MAP_HI_RES_SIZE, SHADOW_NEAR_PLANE, m_createInfo.radius);

    m_viewMatrix[0] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[1] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[2] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_viewMatrix[3] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    m_viewMatrix[4] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_viewMatrix[5] = glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    m_projectionTransforms[0] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[1] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[2] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_projectionTransforms[3] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    m_projectionTransforms[4] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    m_projectionTransforms[5] = projectionMatrix * glm::lookAt(m_createInfo.position, m_createInfo.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    for (int i = 0; i < 6; i++) {
        m_frustum[i].Update(m_projectionTransforms[i]);
    }
}


void LightFlicker::Update(float deltaTime, float timeSeconds) {
    float tSlow = timeSeconds * m_slowFrequencyHz;
    float tMid = timeSeconds * m_midFrequencyHz + 12.3f;
    float tFast = timeSeconds * m_fastFrequencyHz + 41.7f;


    m_lowColor = glm::vec3(1.00f, 0.35f, 0.10f) * 0.5f;
    m_highColor = glm::vec3(1.00f, 0.75f, 0.35f) * 1.0f;

    int32_t seed = 0; // Random gen this if u want
    float nSlow = Util::FractalNoise1D(tSlow, seed + 1);
    float nMid = Util::FractalNoise1D(tMid, seed + 2);
    float nFast = Util::FractalNoise1D(tFast, seed + 3);

    float rawFlicker01 = nSlow * m_slowWeight + nMid * m_midWeight + nFast * m_fastWeight;
    rawFlicker01 = glm::clamp(rawFlicker01, 0.0f, 1.0f);

    float shapedFlicker01 = std::pow(rawFlicker01, m_shapePower);

    float alpha = 1.0f - std::exp(-deltaTime * m_responseHz);
    m_currentFlicker = glm::mix(m_currentFlicker, shapedFlicker01, alpha);
    m_currentFlicker = glm::clamp(m_currentFlicker, 0.0f, 1.0f);

    float intensityScale = (1.0f - m_amplitude) + m_amplitude * m_currentFlicker;

    m_currentColor = glm::mix(m_lowColor, m_highColor, m_currentFlicker);
}