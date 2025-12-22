#include "PowerPoleSet.h"
#include "HellLogging.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Util.h"

void PowerPoleSet::Init() {
    std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;

    m_meshNodes.Init(NO_ID, "PowerPole", emptyMeshNodeCreateInfoSet);
    m_meshNodes.SetMeshMaterials("PowerPole");
    m_meshNodes.Update(glm::mat4(1.0f));

    std::vector<glm::vec2> controlPoints2D;
    controlPoints2D.push_back(glm::vec2(24.172f, 7.79272f));
    controlPoints2D.push_back(glm::vec2(28.9881f, 4.15977f));
    controlPoints2D.push_back(glm::vec2(33.348f, 3.38889f));
    controlPoints2D.push_back(glm::vec2(36.1279f, 4.16378f));
    controlPoints2D.push_back(glm::vec2(38.2437f, 1.94902f));
    controlPoints2D.push_back(glm::vec2(41.4394f, 0.514656f));
    controlPoints2D.push_back(glm::vec2(49.1863f, 1.213f));

    std::vector<glm::vec3> controlPoints3D;
    for (glm::vec2& point : controlPoints2D) {
        glm::vec3 worldPosition = Physics::GetHeightMapPositionAtXZ(point.x, point.y);
        controlPoints3D.push_back(worldPosition);
    }

    float spacing = 7.0f;
    m_finalPositions = Util::GetBeizerPointsFromControlPoints(controlPoints3D, spacing);

    // Error check
    if (m_finalPositions.size() < 2) {
        Logging::Error() << "PowerPoleSet::Init() failed because there were less than 2 final positions";
        return;
    }

    std::vector<RenderItem> meshNodeRenderItems = m_meshNodes.GetRenderItems();

    glm::vec3 localWirePositionBackA = m_meshNodes.GetBoneLocalMatrix("A_back")[3];
    glm::vec3 localWirePositionBackB = m_meshNodes.GetBoneLocalMatrix("B_back")[3];
    glm::vec3 localWirePositionBackC = m_meshNodes.GetBoneLocalMatrix("C_back")[3];
    glm::vec3 localWirePositionBackD = m_meshNodes.GetBoneLocalMatrix("D_back")[3];
    glm::vec3 localWirePositionFrontA = m_meshNodes.GetBoneLocalMatrix("A_front")[3];
    glm::vec3 localWirePositionFrontB = m_meshNodes.GetBoneLocalMatrix("B_front")[3];
    glm::vec3 localWirePositionFrontC = m_meshNodes.GetBoneLocalMatrix("C_front")[3];
    glm::vec3 localWirePositionFrontD = m_meshNodes.GetBoneLocalMatrix("D_front")[3];

    for (int i = 0; i < m_finalPositions.size() - 1; i++) {
        glm::vec3 position = m_finalPositions[i] * glm::vec3(1.0f, 0.0f, 1.0f);
        glm::vec3 nextPosition = m_finalPositions[i + 1] * glm::vec3(1.0f, 0.0f, 1.0f);

        Transform transform;
        transform.position = m_finalPositions[i];
        transform.rotation.y = Util::EulerYRotationBetweenTwoPoints(position, nextPosition) + (HELL_PI * 0.5f);

        for (RenderItem& renderItem : meshNodeRenderItems) {
            renderItem.modelMatrix = transform.to_mat4();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            Util::UpdateRenderItemAABB(renderItem);
            m_renderItems.push_back(renderItem);
        }

        // Get wire
        m_wirePositionsBackA.push_back(transform.to_mat4() * glm::vec4(localWirePositionBackA, 1.0f));
        m_wirePositionsBackB.push_back(transform.to_mat4() * glm::vec4(localWirePositionBackB, 1.0f));
        m_wirePositionsBackC.push_back(transform.to_mat4() * glm::vec4(localWirePositionBackC, 1.0f));
        m_wirePositionsBackD.push_back(transform.to_mat4() * glm::vec4(localWirePositionBackD, 1.0f));
        m_wirePositionsFrontA.push_back(transform.to_mat4() * glm::vec4(localWirePositionFrontA, 1.0f));
        m_wirePositionsFrontB.push_back(transform.to_mat4() * glm::vec4(localWirePositionFrontB, 1.0f));
        m_wirePositionsFrontC.push_back(transform.to_mat4() * glm::vec4(localWirePositionFrontC, 1.0f));
        m_wirePositionsFrontD.push_back(transform.to_mat4() * glm::vec4(localWirePositionFrontD, 1.0f));
    }

    for (int i = 0; i < m_wirePositionsBackA.size() - 1; i++) {
        int spacing = 2;

        Wire& wireA = m_wires.emplace_back();
        wireA.Init(m_wirePositionsBackA[i], m_wirePositionsFrontA[i + 1], 0.5f, 0.015f, spacing);

        Wire& wireB = m_wires.emplace_back();
        wireB.Init(m_wirePositionsBackB[i], m_wirePositionsFrontB[i + 1], 0.5f, 0.015f, spacing);

        Wire& wireC = m_wires.emplace_back();
        wireC.Init(m_wirePositionsBackC[i], m_wirePositionsFrontC[i + 1], 0.5f, 0.015f, spacing);

        Wire& wireD = m_wires.emplace_back();
        wireD.Init(m_wirePositionsBackD[i], m_wirePositionsFrontD[i + 1], 0.5f, 0.015f, spacing);
    }
}

void PowerPoleSet::Update() {
    // Nothing as of yet
}

void PowerPoleSet::CleanUp() {
    for (Wire& wire : m_wires) {
        wire.GetMeshBuffer().Reset();
    }

    m_meshNodes.CleanUp();
}

const std::vector<RenderItem>& const PowerPoleSet::GetRenderItems() {
    return m_renderItems;
}
