#include "Fence.h"
#include "HellLogging.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Util.h"

void Fence::Init() {
    std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;

    m_meshNodesThin.Init(NO_ID, "FencePostThin", emptyMeshNodeCreateInfoSet);
    m_meshNodesThin.SetMeshMaterials("Fence");
    m_meshNodesThin.UpdateHierarchy();
    m_meshNodesThin.UpdateRenderItems(glm::mat4(1.0f));

    m_meshNodesFat.Init(NO_ID, "FencePost", emptyMeshNodeCreateInfoSet);
    m_meshNodesFat.SetMeshMaterials("Fence");
    m_meshNodesFat.UpdateHierarchy();
    m_meshNodesFat.UpdateRenderItems(glm::mat4(1.0f));

    m_meshNodesWireBarbed.Init(NO_ID, "FenceWireBarbed", emptyMeshNodeCreateInfoSet);
    m_meshNodesWireBarbed.SetMeshMaterials("Fence");
    m_meshNodesWireBarbed.UpdateHierarchy();
    m_meshNodesWireBarbed.UpdateRenderItems(glm::mat4(1.0f));

    m_meshNodesWire.Init(NO_ID, "FenceWire", emptyMeshNodeCreateInfoSet);
    m_meshNodesWire.SetMeshMaterials("Fence");
    m_meshNodesWire.UpdateHierarchy();
    m_meshNodesWire.UpdateRenderItems(glm::mat4(1.0f));

    std::vector<glm::vec2> controlPoints2D;
    controlPoints2D.push_back(glm::vec2(27.7062, 6.50534));
    controlPoints2D.push_back(glm::vec2(30.2548, 4.96767));
    controlPoints2D.push_back(glm::vec2(33.451, 4.56134));
    controlPoints2D.push_back(glm::vec2(36.6244, 5.21321));
    controlPoints2D.push_back(glm::vec2(38.1925, 3.54931));
    controlPoints2D.push_back(glm::vec2(39.5836, 2.05737));
    controlPoints2D.push_back(glm::vec2(42.9357, 1.34804));
    controlPoints2D.push_back(glm::vec2(46.7641, 1.83892));
    controlPoints2D.push_back(glm::vec2(49.0083, 2.23763));

    std::vector<glm::vec3> controlPoints3D;
    for (glm::vec2& point : controlPoints2D) {
        glm::vec3 worldPosition = Physics::GetHeightMapPositionAtXZ(point.x, point.y);
        controlPoints3D.push_back(worldPosition);
    }

    float spacing = 1.0f;
    m_finalPositions = Util::GetBeizerPointsFromControlPoints(controlPoints3D, spacing);

    // Error check
    if (m_finalPositions.size() < 2) {
        Logging::Error() << "Fence::Init() failed because there were less than 2 final positions";
        return;
    }

    std::vector<RenderItem> meshNodeRenderItemsThin = m_meshNodesThin.GetRenderItems();
    std::vector<RenderItem> meshNodeRenderItemsFat = m_meshNodesFat.GetRenderItems();
    std::vector<RenderItem> meshNodeRenderWire = m_meshNodesWire.GetRenderItems();
    std::vector<RenderItem> meshNodeRenderWireBarbed = m_meshNodesWireBarbed.GetRenderItems();

    glm::vec3 localWirePositionAThin = m_meshNodesThin.GetBoneLocalMatrix("A")[3];
    glm::vec3 localWirePositionBThin = m_meshNodesThin.GetBoneLocalMatrix("B")[3];
    glm::vec3 localWirePositionCThin = m_meshNodesThin.GetBoneLocalMatrix("C")[3];
    glm::vec3 localWirePositionDThin = m_meshNodesThin.GetBoneLocalMatrix("D")[3];
    glm::vec3 localWirePositionEThin = m_meshNodesThin.GetBoneLocalMatrix("E")[3];
    glm::vec3 localWirePositionAFat = m_meshNodesFat.GetBoneLocalMatrix("A")[3];
    glm::vec3 localWirePositionBFat = m_meshNodesFat.GetBoneLocalMatrix("B")[3];
    glm::vec3 localWirePositionCFat = m_meshNodesFat.GetBoneLocalMatrix("C")[3];
    glm::vec3 localWirePositionDFat = m_meshNodesFat.GetBoneLocalMatrix("D")[3];
    glm::vec3 localWirePositionEFat = m_meshNodesFat.GetBoneLocalMatrix("E")[3];

    int counter = 0;
    for (int i = 0; i < m_finalPositions.size() - 1; i++) {
        glm::vec3 position = m_finalPositions[i] * glm::vec3(1.0f, 0.0f, 1.0f);
        glm::vec3 nextPosition = m_finalPositions[i + 1] * glm::vec3(1.0f, 0.0f, 1.0f);

        float maxWonkiness = 0.055f;

        Transform transform;
        transform.position = m_finalPositions[i];
        transform.rotation.y = Util::EulerYRotationBetweenTwoPoints(position, nextPosition);
        transform.rotation.x += Util::RandomFloat(-maxWonkiness, maxWonkiness);
        transform.rotation.y += Util::RandomFloat(-maxWonkiness, maxWonkiness);
        transform.rotation.z += Util::RandomFloat(-maxWonkiness, maxWonkiness);

        if (counter == 0) {
            for (RenderItem& renderItem : meshNodeRenderItemsFat) {
                renderItem.modelMatrix = transform.to_mat4();
                renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
                Util::UpdateRenderItemAABB(renderItem);
                m_renderItems.push_back(renderItem);
            }

            // Get wire
            m_wirePositionsA.push_back(transform.to_mat4()* glm::vec4(localWirePositionAThin, 1.0f));
            m_wirePositionsB.push_back(transform.to_mat4()* glm::vec4(localWirePositionBThin, 1.0f));
            m_wirePositionsC.push_back(transform.to_mat4()* glm::vec4(localWirePositionCThin, 1.0f));
            m_wirePositionsD.push_back(transform.to_mat4()* glm::vec4(localWirePositionDThin, 1.0f));
            m_wirePositionsE.push_back(transform.to_mat4()* glm::vec4(localWirePositionEThin, 1.0f));
        }
        if (counter > 0) {
            for (RenderItem& renderItem : meshNodeRenderItemsThin) {
                renderItem.modelMatrix = transform.to_mat4();
                renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
                Util::UpdateRenderItemAABB(renderItem);
                m_renderItems.push_back(renderItem);
            }

            // Get wire
            m_wirePositionsA.push_back(transform.to_mat4() * glm::vec4(localWirePositionAFat, 1.0f));
            m_wirePositionsB.push_back(transform.to_mat4() * glm::vec4(localWirePositionBFat, 1.0f));
            m_wirePositionsC.push_back(transform.to_mat4() * glm::vec4(localWirePositionCFat, 1.0f));
            m_wirePositionsD.push_back(transform.to_mat4() * glm::vec4(localWirePositionDFat, 1.0f));
            m_wirePositionsE.push_back(transform.to_mat4() * glm::vec4(localWirePositionEFat, 1.0f));
        }
        counter++;
        if (counter == 3) {
            counter = 0;
        }

    }

    for (int i = 0; i < m_wirePositionsA.size() - 1; i++) {

        for (RenderItem& renderItem : meshNodeRenderWireBarbed) {
            m_renderItems.push_back(CreateWireRenderItem(renderItem, m_wirePositionsA[i], m_wirePositionsA[i + 1]));
            m_renderItems.push_back(CreateWireRenderItem(renderItem, m_wirePositionsC[i], m_wirePositionsC[i + 1]));
            m_renderItems.push_back(CreateWireRenderItem(renderItem, m_wirePositionsE[i], m_wirePositionsE[i + 1]));
        }
        for (RenderItem& renderItem : meshNodeRenderWire) {
            m_renderItems.push_back(CreateWireRenderItem(renderItem, m_wirePositionsB[i], m_wirePositionsB[i + 1]));
            m_renderItems.push_back(CreateWireRenderItem(renderItem, m_wirePositionsD[i], m_wirePositionsD[i + 1]));
        }
    }
}

void Fence::Update() {
    // Nothing as of yet
}

void Fence::CleanUp() {
    // Nothing as of yet
}

RenderItem Fence::CreateWireRenderItem(RenderItem& localSpaceRenderItem, glm::vec3& position, glm::vec3 nextPosition) {
    // Translation
    Transform translation;
    translation.position = position;

    // Rotation
    glm::vec3 forwardToNext = glm::normalize(nextPosition - position);
    glm::mat4 rotationMatrix = Util::RotationMatrixFromForwardVector(forwardToNext, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));

    // Scale
    Transform scale;
    scale.scale.x = glm::distance(position, nextPosition);

    RenderItem renderItem = localSpaceRenderItem;
    renderItem.modelMatrix = translation.to_mat4() * rotationMatrix * scale.to_mat4();
    renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
    Util::UpdateRenderItemAABB(renderItem);

    return renderItem;
}

const std::vector<RenderItem>& const Fence::GetRenderItems() {
    return m_renderItems;
}
