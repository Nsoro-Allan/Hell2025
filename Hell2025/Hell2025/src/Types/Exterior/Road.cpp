#include "Road.h"
#include "Renderer/Renderer.h"
#include "Physics/Physics.h"
#include "Util.h"

#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"

void Road::Init() {
    m_controlPoints2D.push_back(glm::vec2(26.4136, 11.1253));
    m_controlPoints2D.push_back(glm::vec2(31.6507, 7.2496));
    m_controlPoints2D.push_back(glm::vec2(37.1036, 7.7737));
    m_controlPoints2D.push_back(glm::vec2(41.9232, 4.17412));
    m_controlPoints2D.push_back(glm::vec2(48.2309, 4.86765));

    m_controlPoints3D.clear();

    for (glm::vec2& point : m_controlPoints2D) {
        glm::vec3 worldPosition = Physics::GetHeightMapPositionAtXZ(point.x, point.y);
        m_controlPoints3D.push_back(worldPosition);
    }

    RoadCurveType curveType = RoadCurveType::BEIZER;

    if (curveType == BEIZER) {
        float spacing = 1.0f;
        m_worldPoints = Util::GetBeizerPointsFromControlPoints(m_controlPoints3D, spacing);

        // Snap to heightmap
        for (glm::vec3& point : m_worldPoints) {
            point = Physics::GetHeightMapPositionAtXZ(point.x, point.z);
        }
    }
}

void Road::Update() {
    //DrawPoints();
}

void Road::DrawPoints() {
    
    for (glm::vec3& point : m_worldPoints) {
        Renderer::DrawPoint(point, GREEN);
    }
    for (glm::vec3& point : m_controlPoints3D) {
        Renderer::DrawPoint(point, RED);
    }

    return;

    std::vector<ViewportData> viewportData = RenderDataManager::GetViewportData();
    if (viewportData.empty()) return;


    Renderer::DrawPoint(m_worldPoints[0], BLUE);

    glm::vec3 worldPos = m_worldPoints[0];
    glm::mat4 projectionView = viewportData[0].projectionView;
    int screenWidth = viewportData[0].width;
    int screenHeight = viewportData[0].height;
    bool flipY = false;


    glm::ivec2 mouseCoords = glm::ivec2(Input::GetMouseX(), Input::GetMouseY());
    glm::ivec2 pointCoords = Util::WorldToScreenCoords(worldPos, projectionView, screenWidth, screenHeight);



    Viewport* viewport = ViewportManager::GetViewportByIndex(0);
    glm::ivec2 mouseCoords2 = viewport->GetLocalMouseCoords();
    glm::ivec2 viewportCoords = viewport->WorldToScreen(viewportData[0].view, m_worldPoints[0]);

    bool hover = Util::IsWithinThreshold(mouseCoords2, pointCoords, 10);
    if (hover) {
        Renderer::DrawPoint(m_worldPoints[0], YELLOW);
    }

    std::cout << "Point:          " << pointCoords.x << ", " << pointCoords.y << "\n";
    std::cout << "Mouse:          " << mouseCoords.x << ", " << mouseCoords.y << "\n";
    std::cout << "Mouse2:         " << mouseCoords2.x << ", " << mouseCoords2.y << "\n";
    std::cout << "viewportCoords: " << viewportCoords.x << ", " << viewportCoords.y << "\n";

}