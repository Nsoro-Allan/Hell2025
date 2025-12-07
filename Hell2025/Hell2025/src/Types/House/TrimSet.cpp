#include "TrimSet.h"
#include "AssetManagement/AssetManager.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Util/Util.h"
#include "World/World.h"

TrimSet::TrimSet(uint64_t id, const TrimSetCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;
    CreateRenderItems();
}

void TrimSet::CreateRenderItems() {
    m_corners.clear();
    m_externalCorners.clear();
    m_internalCorners.clear();
    m_renderItems.clear();

    // Find corners, by walking around the wall segment points, and check for raycast hits on all fireplace wall aabbs
    for (int i = 0; i < m_createInfo.points.size(); i++) {
        const glm::vec3& point = m_createInfo.points[i];
        const glm::vec3& nextPoint = m_createInfo.points[i + 1];
        float distanceToNextPoint = glm::distance(point, nextPoint);
        glm::vec3 directionToNextPoint = glm::normalize(nextPoint - point);

        glm::vec3 rayOrigin = point;
        glm::vec3 rayDir = directionToNextPoint;
        float maxDistance = distanceToNextPoint;

        // Add the current point
        m_corners.push_back(TrimCorner(point));

        // WARNING! This only works for one fireplace, because future ray casts overwrite any previous one
        AABBRayResult rayResult;
        for (Fireplace& fireplace : World::GetFireplaces()) {
            rayResult = Util::RayIntersectAABB(rayOrigin, rayDir, maxDistance, fireplace.GetWallsAABB(), fireplace.GetWorldMatrix());

            // Hit fireplace?
            if (rayResult.hitFound && rayResult.hitNormalLocal == glm::vec3(0.0f, 0.0f, 1.0f)) {
                glm::vec3 pointA = rayResult.hitPositionWorld;
                m_corners.push_back(TrimCorner(pointA));

                // Because you always walk clockwise around the room, you can safely hack in the extra fireplace points with the following             
                glm::vec3 pointB = pointA + (fireplace.GetWorldForward() * fireplace.GetWallDepth());
                m_corners.push_back(TrimCorner(pointB));

                glm::vec3 pointC = pointB + (-fireplace.GetWorldRight() * fireplace.GetWallWidth());
                m_corners.push_back(TrimCorner(pointC));

                glm::vec3 pointD = pointC + (-fireplace.GetWorldForward() * fireplace.GetWallDepth());
                m_corners.push_back(TrimCorner(pointD));
            }
        }
    }

    // Figure out which corners are internal/external
    size_t count = m_corners.size();
    if (count < 3) return;

    std::vector<glm::vec2> pts;
    pts.reserve(count);
    for (int i = 0; i < count; ++i) {
        const glm::vec3& p = m_corners[i].m_position;
        pts.emplace_back(p.x, p.z); // 2D projection
    }

    float area2 = 0.0f;
    for (int i = 0; i < count; ++i) {
        const glm::vec2& a = pts[i];
        const glm::vec2& b = pts[(i + 1) % count];
        area2 += a.x * b.y - b.x * a.y;
    }
    bool ccw = area2 > 0.0f; // True if polygon is CCW

    for (int i = 0; i < count; ++i) {
        int prev = (i - 1 + count) % count;
        int next = (i + 1) % count;

        glm::vec2 e1 = glm::normalize(pts[i] - pts[prev]);
        glm::vec2 e2 = glm::normalize(pts[next] - pts[i]);

        float cross = e1.x * e2.y - e1.y * e2.x; // Z of 2D cross

        bool external = false;
        if (ccw)
            external = cross < 0.0f;  // Concave for CCW
        else
            external = cross > 0.0f;  // Concave for CW

        const glm::vec3& cornerPosition = m_corners[i].m_position;
        const glm::vec3& nextCornerPosition = m_corners[next].m_position;

        if (external) {
            m_corners[i].m_internal = false;
        }
    }

    Material* material = nullptr;
    Model* model = nullptr;

    if (m_createInfo.type == TrimSetType::CEILING_FANCY) {
		model = AssetManager::GetModelByName("TrimsCeilingFancy");
		material = AssetManager::GetMaterialByName("WoodTrims");
    }

    if (!material) return;
    if (!model) return;

    std::string trimMeshName = "Trim";
	std::string internalCornerMeshName = "InternalCorner";
	std::string externalCornerMeshName = "ExternalCorner";

    int32_t trimMeshIndex = model->GetGlobalMeshIndexByMeshName(trimMeshName);
    int32_t internalCornerMeshIndex = model->GetGlobalMeshIndexByMeshName(internalCornerMeshName);
    int32_t externalCornerMeshIndex = model->GetGlobalMeshIndexByMeshName(externalCornerMeshName);

    float trimScale = m_createInfo.trimScale;
    float trimLength = 1.0f * trimScale;
    float internalCornerPieceSize = 0;

    // Get internal corner piece size if the mesh index is valid
    if (Mesh* internalCornerMesh = AssetManager::GetMeshByIndex(internalCornerMeshIndex)) {
        internalCornerPieceSize = (internalCornerMesh->aabbMax.z - 0.01f) * trimScale; // 0.01cm safety threshold to avoid gaps
    }

    for (int i = 0; i < m_corners.size() - 1; i++) {

        const glm::vec3& point = m_corners[i].m_position;
        const glm::vec3& nextPoint = m_corners[i + 1].m_position;

        float distanceToNextPoint = glm::distance(point, nextPoint);
        glm::vec3 directionToNextPoint = glm::normalize(nextPoint - point);

        // Make the corner render items
        Transform cornerTransform;
        cornerTransform.position = m_corners[i].m_position;
        cornerTransform.rotation.y = Util::YRotationBetweenTwoPoints(point, nextPoint);
        cornerTransform.scale *= trimScale;

        // This "cursor" is the amount walked so far on this wall
        float cursor = 0.0f;

        // If on the first corner and it is internal, push the cursor forward the width of the corner piece
        if (m_corners[i].m_internal) {
            cursor += internalCornerPieceSize;
        }

        // Internal corner
        if (m_corners[i].m_internal && internalCornerMeshIndex != -1) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.modelMatrix = cornerTransform.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = internalCornerMeshIndex;
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.normalMapTextureIndex = material->m_normal;
            Util::UpdateRenderItemAABB(renderItem);
            Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        }
        // External corner
        else if (!m_corners[i].m_internal && externalCornerMeshIndex != -1) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            renderItem.modelMatrix = cornerTransform.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = externalCornerMeshIndex;
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.normalMapTextureIndex = material->m_normal;
            Util::UpdateRenderItemAABB(renderItem);
            Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        }

        // Make the trim segments
        float distanceToWalk = distanceToNextPoint - trimLength;

		if (m_corners[i + 1].m_internal) {
            distanceToWalk -= internalCornerPieceSize;
		}

        while (cursor < distanceToWalk) {
            Transform transform;
            transform.position = point + (directionToNextPoint * cursor);
            transform.rotation.y = Util::YRotationBetweenTwoPoints(point, nextPoint);
			transform.scale = glm::vec3(trimScale);

            if (trimMeshIndex != -1) {
                RenderItem& renderItem = m_renderItems.emplace_back();
                renderItem.modelMatrix = transform.to_mat4();
                renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
                renderItem.meshIndex = trimMeshIndex;
                renderItem.baseColorTextureIndex = material->m_basecolor;
                renderItem.rmaTextureIndex = material->m_rma;
                renderItem.normalMapTextureIndex = material->m_normal;
                Util::UpdateRenderItemAABB(renderItem);
                Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
            }

            cursor += trimLength;
        }

        // Final trim segment
        float finalDistance = distanceToNextPoint - cursor;

        if (m_corners[i + 1].m_internal) {
            finalDistance -= internalCornerPieceSize;
        }

        Transform transform;
		transform.position = point + (directionToNextPoint * cursor);
		transform.rotation.y = Util::YRotationBetweenTwoPoints(point, nextPoint);
		transform.scale.x *= finalDistance;
		transform.scale.y *= trimScale;
		transform.scale.z *= trimScale;

		RenderItem& renderItem = m_renderItems.emplace_back();
		renderItem.modelMatrix = transform.to_mat4();
		renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
		renderItem.meshIndex = trimMeshIndex;
		renderItem.baseColorTextureIndex = material->m_basecolor;
		renderItem.rmaTextureIndex = material->m_rma;
		renderItem.normalMapTextureIndex = material->m_normal;
		Util::UpdateRenderItemAABB(renderItem);
		Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
    }
}

void TrimSet::Update() {
    if (Input::KeyPressed(HELL_KEY_T)) {
        CreateRenderItems();
        std::cout << "Recreated TrimSet " << m_objectId << "\n";
    }
}

void TrimSet::CleanUp() {

}

void TrimSet::RenderDebug(const glm::vec3& color) {

  //for (int i = 0; i < m_corners.size(); i++) {
  //    const glm::vec3& point = m_corners[i].m_position;
  //    if (m_corners[i].m_internal) {
  //        Renderer::DrawPoint(point, GREEN);
  //    }
  //    else {
  //        Renderer::DrawPoint(point, RED);
  //    }
  //}
}