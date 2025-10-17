#include "Editor/Editor.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "World/World.h"

namespace Editor {
    void BeginWall();

    void UpdateWallPlacement() {
        glm::vec3 rayHitPosition = GetMouseRayPlaneIntersectionPoint(glm::vec3(0.0f), WORLD_UP);
        bool cusrsorAtWallOrigin = false;

        // Begin wall
        if (Input::LeftMousePressed() && GetPlacementObjectId() == 0) {
            BeginWall();
            return;
        }

        Wall* wall = World::GetWallByObjectId(GetPlacementObjectId());
        if (!wall) return;

        std::cout << "wall point count: " << wall->GetPointCount() << "\n";

        // Apply axis constraint
        if (wall->GetPointCount() > 1) {
            int previousPointIndex = wall->GetPointCount() - 2;
            glm::vec3 previousPosition = wall->GetPointByIndex(previousPointIndex);

            if (GetAxisConstraint() == Axis::X) {
                rayHitPosition.y = previousPosition.y;
                rayHitPosition.z = previousPosition.z;
            }
            if (GetAxisConstraint() == Axis::Z) {
                rayHitPosition.x = previousPosition.x;
                rayHitPosition.y = previousPosition.y;
            }
        }

        // Snap to the first point if within range
        if (wall->GetPointCount() > 2) {
            float threshold = 0.25f;
            glm::vec3 firstPoint = wall->GetPointByIndex(0);
            if (glm::distance(rayHitPosition, firstPoint) < threshold) {
                rayHitPosition = firstPoint;
                cusrsorAtWallOrigin = true;
            }
        }

        // Complete wall
        if (Input::LeftMousePressed() && cusrsorAtWallOrigin) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            ExitObjectPlacement();
            return;
        }

        // Add wall segment
        if (Input::LeftMousePressed() && wall->GetPointCount() > 0) {
            glm::vec3 endPoint = wall->GetPointByIndex(wall->GetPointCount() -1);
            glm::vec3 newPoint = endPoint + glm::vec3(0.5f, 0.0, 0.5f);
            if (wall->AddPointToEnd(newPoint)) {
                World::UpdateHouseMeshBuffer();
                Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                return;
            }
        }

        // Flip faces
        if (Input::KeyPressed(HELL_KEY_F)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            wall->FlipFaces();
            World::UpdateHouseMeshBuffer();
        }

        if (wall->GetPointCount() > 1) {
            // Move last point to mouse cursor
            int pointIndex = wall->GetPointCount() - 1;
            if (wall->UpdatePointPosition(pointIndex, rayHitPosition)) {
                World::UpdateHouseMeshBuffer();
            }

            // Draw lines and points
            glm::vec4 pointColor = cusrsorAtWallOrigin ? WHITE : OUTLINE_COLOR;
            wall->DrawSegmentVertices(pointColor);
            wall->DrawSegmentLines(WHITE);
        }
        
        // Bail
        if (Input::RightMouseDown()) {
            ExitObjectPlacement();
        }
    }

    void BeginWall() {
        glm::vec3 pointA = GetMouseRayPlaneIntersectionPoint(glm::vec3(0.0f), WORLD_UP);
        glm::vec3 pointB = pointA + glm::vec3(0.5f, 0.0, 0.5f);

        UnselectAnyObject();
        WallCreateInfo createInfo;
        createInfo.points.push_back(pointA);
        createInfo.points.push_back(pointB);
        createInfo.materialName = "WallPaper";
        createInfo.materialName = "Ceiling2";
        createInfo.textureOffsetU = 0.0f;
        createInfo.textureOffsetV = -1.4f;
        createInfo.textureScale = 1 / 2.4f;
        createInfo.height = 2.4f;
        createInfo.ceilingTrimType = TrimType::TIMBER;
        createInfo.floorTrimType = TrimType::TIMBER;
        createInfo.useReversePointOrder = false;
        SetPlacementObjectId(World::AddWall(createInfo));
        World::UpdateHouseMeshBuffer();
        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
        std::cout << "Beginning wall!\n";
    }
}