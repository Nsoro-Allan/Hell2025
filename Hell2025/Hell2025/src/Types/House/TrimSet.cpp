#include "TrimSet.h"
#include "AssetManagement/AssetManager.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Util/Util.h"
#include "World/World.h"

TrimSet::TrimSet(uint64_t id, const TrimSetCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;

    //World::GetFireplaces();

    CreateRenderItems();
}

void TrimSet::CreateRenderItems() {

    // Find corners, by walking around the wall segment points, and check for raycast hits on all fireplace wall aabbs
    m_corners.clear();

    for (int i = 0; i < m_createInfo.points.size(); i++) {

        std::cout << "walking point: " << i << "\n";

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
            if (rayResult.hitFound) {
                glm::vec3 pointA = rayResult.hitPosition;
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
    m_internalCorners.clear();
    m_externalCorners.clear();

    int count = m_corners.size();
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

        bool external;
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
















    m_renderItems.clear();

    Material* material = AssetManager::GetMaterialByName("WoodTrims");
    Model* model = AssetManager::GetModelByName("Trims");

    std::string trimMeshName = "TrimMisc";
    std::string internalCornerMeshName = "TrimMiscInternalCorner";
    std::string externalCornerMeshName = "TrimMiscExternalCorner";

    uint32_t trimMeshIndex = model->GetGlobalMeshIndexByMeshName(trimMeshName);
    uint32_t internalCornerMeshIndex = model->GetGlobalMeshIndexByMeshName(internalCornerMeshName);
    uint32_t externalCornerMeshIndex = model->GetGlobalMeshIndexByMeshName(externalCornerMeshName);

    if (trimMeshIndex == -1 || internalCornerMeshIndex == -1 || externalCornerMeshIndex == -1) {
        std::cout << "Shit fucked up\n";
        return;
    }
    else {
        std::cout << "Shit did not fuck up\n";
    }

    float cornerPieceSize = 0.135f;

    for (int i = 0; i < m_corners.size() - 1; i++) {

        const glm::vec3& point = m_corners[i].m_position;
        const glm::vec3& nextPoint = m_corners[i + 1].m_position;

        float distanceToNextPoint = glm::distance(point, nextPoint);
        glm::vec3 directionToNextPoint = glm::normalize(nextPoint - point);

        // Make the corner render items
        Transform cornerTransform;
        cornerTransform.position = m_corners[i].m_position;
        cornerTransform.rotation.y = Util::YRotationBetweenTwoPoints(point, nextPoint);

        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.modelMatrix = cornerTransform.to_mat4();
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        if (m_corners[i].m_internal) {
            renderItem.meshIndex = internalCornerMeshIndex;
        }
        else {
            renderItem.meshIndex = externalCornerMeshIndex;
        }
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        Util::UpdateRenderItemAABB(renderItem);
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);


      
       // if (false) // REMOVE ME WHEN U WANT YOUR TRIMS BACK!!!!!!!
        // Make the trim render items
        for (float j = 0.0f; j < distanceToNextPoint; j++) {

            Transform transform;
            transform.position = point + (directionToNextPoint * j);
            transform.rotation.y = Util::YRotationBetweenTwoPoints(point, nextPoint);

            // First piece needs space for corner piece is added (if internal)
            if (j == 0.0f && m_corners[i].m_internal) {
                transform.scale.x = 1.0f - cornerPieceSize;
                transform.position += (directionToNextPoint * cornerPieceSize);
            }

            // Final piece is scaled to fit, plus space for corner piece is added
            if (j > distanceToNextPoint - 1.0f) {

                float distanceFromThisTrimSegmentToNextCorner = glm::distance(transform.position, nextPoint);

                transform.scale.x = distanceFromThisTrimSegmentToNextCorner - cornerPieceSize;
                
                // Unless the next corner is an external corner, then you go all the way
                if (!m_corners[i + 1].m_internal) {
                    transform.scale.x += cornerPieceSize;
                }

                // This feels right, but you might be confused based on what you're seeing, aka this is not actually the true solution...
                //if (j == 0.0f) {
                //    transform.scale.x -= cornerPieceSize;
                //}
            }

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
        //break;
    }






}

void TrimSet::Update() {
    if (Input::KeyPressed(HELL_KEY_T)) {
        CreateRenderItems();
    }

    for (RenderItem& renderItem : m_renderItems) {

        glm::vec3 point = renderItem.modelMatrix[3];

        //Renderer::DrawPoint(point, GREEN);
    }


    //for (int i = 0; i < m_createInfo.points.size() - 1; i++) {
    //for (int i = 0; i < 2; i++) {
    //    const glm::vec3& point = m_createInfo.points[i];
    //    Renderer::DrawPoint(point, BLUE);
    //}
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