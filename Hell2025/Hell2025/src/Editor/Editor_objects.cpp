#include "Editor.h"
#include "Audio/Audio.h"
#include "Editor/Gizmo.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include "UniqueID.h"

#include "Core/Game.h"

namespace Editor {

    int g_selectedVertexIndex = 0;

    void UpdateObjectHover() {
        // Reset values from last frame
        SetHoveredObjectType(ObjectType::NO_TYPE);
        SetHoveredObjectId(0);

        // Bail if there is no hovered viewport
        Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
        if (!viewport) return;

        // Cast physx ray
        float maxRayDistance = 2000;
        glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
        glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
        bool backfaceCulling = BackfaceCullingEnabled();

        PhysXRayResult physxRayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, backfaceCulling);
        if (physxRayResult.hitFound) {
            SetHoveredObjectType(physxRayResult.userData.objectType);
            SetHoveredObjectId(physxRayResult.userData.objectId);
        }

        // BVH ray
        BvhRayResult bvhRayResult = World::ClosestHit(rayOrigin, rayDir, maxRayDistance, GetHoveredViewportIndex());
        if (bvhRayResult.hitFound) {
            float physXDistance = glm::distance(physxRayResult.hitPosition, rayOrigin);
            float bvhDistance = glm::distance(bvhRayResult.hitPosition, rayOrigin);
            if (bvhDistance < physXDistance) {
                SetHoveredObjectType(UniqueID::GetType(bvhRayResult.objectId));
                SetHoveredObjectId(bvhRayResult.objectId);
            }
        }

        if (GetHoveredObjectType() == ObjectType::WALL_SEGMENT) {
            Wall* wall = World::GetWallByWallSegmentObjectId(GetHoveredObjectId());
            if (wall) {
                SetHoveredObjectType(ObjectType::WALL);
                SetHoveredObjectId(wall->GetObjectId());
            }
        }
    }

    void UpdateObjectSelection() {

        //std::cout << Util::EditorSelectionModeToString(GetEditorSelectionMode()) << "\n";


        // Vertex interaction HACK. Find a better place for me
        int viewportIndex = GetHoveredViewportIndex();
        glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(viewportIndex);
        glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(viewportIndex);


        if (GetEditorSelectionMode() != EditorSelectionMode::VERTEX) {
            g_selectedVertexIndex = 0; // maybe -1 is better?
        }

        if (GetSelectedObjectType() == ObjectType::WALL) {
            if (Wall* wall = World::GetWallByObjectId(GetSelectedObjectId())) {

                wall->DrawSegmentVertices(OUTLINE_COLOR);
                wall->DrawSegmentLines(OUTLINE_COLOR);


                // Draw hovered verets and HACK to select it
                for (int i = 0; i < wall->GetWallSegments().size(); i++) {
                    WallSegment& wallSegment = wall->GetWallSegments()[i];
                    glm::vec3 position = wallSegment.GetStart();
                    float radius = Editor::GetScalingFactor(10);

                    bool rayHit = Util::RayIntersectsSphere(rayOrigin, rayDir, position, radius);

                    if (rayHit) {
                        Renderer::DrawPoint(position, WHITE);
                    }

                    if (rayHit && Input::LeftMousePressed()) {
                        g_selectedVertexIndex = i;
                        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                        Gizmo::SetPosition(position);
                        SetEditorSelectionMode(EditorSelectionMode::VERTEX);
                    }
                }

                if (GetEditorSelectionMode() == EditorSelectionMode::VERTEX) {
                    // Draw selcted vertex
                    WallSegment& wallSegment = wall->GetWallSegments()[g_selectedVertexIndex];
                    glm::vec3 position = wallSegment.GetStart();
                    Renderer::DrawPoint(position, YELLOW);


                    if (g_selectedVertexIndex != 0) {
                       // std::cout << "selectedVertexIndex: " << position << "\n";
                    }
                }

            }
        }




        // HACKKK
        if (HousePlane* plane = World::GetHousePlaneByObjectId(GetSelectedObjectId())) {

            plane->DrawEdges(OUTLINE_COLOR);
            plane->DrawVertices(OUTLINE_COLOR);

            // Draw hovered verts and HACK to select it
            for (int i = 0; i < 4; i++) {

                glm::vec3 position = plane->GetVertices()[i].position;
                float radius = Editor::GetScalingFactor(10);

                bool rayHit = Util::RayIntersectsSphere(rayOrigin, rayDir, position, radius);

                if (rayHit) {
                    Renderer::DrawPoint(position, WHITE);
                }

                if (rayHit && Input::LeftMousePressed()) {
                    g_selectedVertexIndex = i;
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    Gizmo::SetPosition(position);
                    SetEditorSelectionMode(EditorSelectionMode::VERTEX);
                }
            }

            if (GetEditorSelectionMode() == EditorSelectionMode::VERTEX) {
                // Draw selcted vertex
                glm::vec3 position = plane->GetVertices()[g_selectedVertexIndex].position;
                Renderer::DrawPoint(position, YELLOW);
            }

            // is this IF neccesssary? write safer less confusing logic!!!
           // if (GetEditorSelectionMode() == EditorSelectionMode::OBJECT) {
           //     Gizmo::SetPosition(plane->GetWorldSpaceCenter());
           // }
        }














        if (GetEditorState() != EditorState::IDLE) return;
        if (GetEditorSelectionMode() == EditorSelectionMode::VERTEX) return;

        if (Input::LeftMousePressed() && !Gizmo::HasHover() && Input::GetMouseX() > EDITOR_LEFT_PANEL_WIDTH) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            SetSelectedObjectType(GetHoveredObjectType());
            SetSelectedObjectId(GetHoveredObjectId());

            //std::cout << "Selected: " << Util::ObjectTypeToString(GetSelectedObjectType()) << " " << GetSelectedObjectId() << "\n";
            Gizmo::SetSourceObjectOffeset(World::GetGizmoOffest(GetSelectedObjectId()));

            GenericObject* drawers = World::GetGenericObjectById(GetSelectedObjectId());
            if (drawers) {
                Gizmo::SetPosition(drawers->GetPosition());
                Gizmo::SetRotation(drawers->GetRotation());
            }

            if (GetSelectedObjectType() == ObjectType::DOOR) {
                Door* door = World::GetDoorByObjectId(GetSelectedObjectId());
                if (door) {
                    Gizmo::SetPosition(door->GetPosition());
                }
            }

            if (GetSelectedObjectType() == ObjectType::PIANO) {
                Piano* piano = World::GetPianoByObjectId(GetSelectedObjectId());
                if (piano) {
                    Gizmo::SetPosition(piano->GetPosition());
                }
            }

            if (GetSelectedObjectType() == ObjectType::HOUSE_PLANE) {
                HousePlane* plane = World::GetHousePlaneByObjectId(GetSelectedObjectId());
                if (plane) {
                    // is this IF neccesssary? write safer less confusing logic!!!
                    if (GetEditorSelectionMode() == EditorSelectionMode::OBJECT) {
                        Gizmo::SetPosition(plane->GetWorldSpaceCenter());
                    }
                }
            }

            if (GetSelectedObjectType() == ObjectType::PICTURE_FRAME) {
                PictureFrame* pictureFrame = World::GetPictureFrameByObjectId(GetSelectedObjectId());
                if (pictureFrame) {
                    Gizmo::SetPosition(pictureFrame->GetPosition());
                }
            }

            if (GetSelectedObjectType() == ObjectType::WALL) {
                if (Wall* wall = World::GetWallByObjectId(GetSelectedObjectId())) {

                    // is this IF neccesssary? write safer less confusing logic!!!
                    if (GetEditorSelectionMode() == EditorSelectionMode::OBJECT) {
                        Gizmo::SetPosition(wall->GetWorldSpaceCenter());
                    }
                }
            }

            if (GetSelectedObjectType() == ObjectType::WINDOW) {
                Window* window = World::GetWindowByObjectId(GetSelectedObjectId());
                if (window) {
                    Gizmo::SetPosition(window->GetPosition());
                }
            }

            if (GetSelectedObjectType() == ObjectType::TREE) {
                Tree* tree = World::GetTreeByObjectId(GetSelectedObjectId());
                if (tree) {
                    Gizmo::SetPosition(tree->GetPosition());
                }
            }
            UpdateOutliner();
        }
    }

    void UpdateObjectGizmoInteraction() {

        UpdateGizmoInteract();

        // if (Input::KeyPressed(HELL_KEY_SPACE)) {
        //     int wallCount = World::GetWalls().size() - 1;
        //     Wall& wall = World::GetWalls()[wallCount];
        //     wall.SetMaterial("BathroomWall");
        //     wall.SetFloorTrimType(TrimType::NONE);
        //     World::UpdateHouseMeshBuffer();
        // }

        //if (Input::KeyPressed(HELL_KEY_SPACE)) {
        //    //DoorCreateInfo createInfo;
        //    //createInfo.position = Game::GetLocalPlayerByIndex(0)->GetFootPosition();
        //    //createInfo.rotation = glm::vec3(0.0f);
        //    //World::AddDoor(createInfo, SpawnOffset());
        //
        //    float xMin = 8.2f;
        //    float xMax = 10.1f;
        //    float zMin = 1.5f;
        //    float zMax = -0.5f;
        //
        //    //LightCreateInfo createInfo = World::GetLights()[0].GetCreateInfo();
        //    //createInfo.position.x = (xMin + xMax) * 0.5f;
        //    //createInfo.position.z = (zMin + zMax) * 0.5f;
        //    //createInfo.radius = 3.0f;
        //    //World::AddLight(createInfo);
        //
        //    PlaneCreateInfo createInfo = World::GetPlanes()[1].GetCreateInfo();
        //    createInfo.p0 = glm::vec3(xMin, 2.4f, zMin);
        //    createInfo.p1 = glm::vec3(xMin, 2.4f, zMax);
        //    createInfo.p2 = glm::vec3(xMax, 2.4f, zMax);
        //    createInfo.p3 = glm::vec3(xMax, 2.4f, zMin);
        //    //createInfo.textureScale = 1.0f;
        //    //createInfo.materialName = "BathroomFloor";
        //    World::AddHousePlane(createInfo, SpawnOffset());
        //    World::UpdateHouseMeshBuffer();
        //    std::cout << "created plane\n";
        //
        //
        //}

        //for (Plane& plane : World::GetPlanes()) {
        //
        //    PlaneCreateInfo createInfo = plane.GetCreateInfo();
        //    Renderer::DrawPoint(createInfo.p0, PURPLE);
        //    Renderer::DrawPoint(createInfo.p1, PURPLE);
        //    Renderer::DrawPoint(createInfo.p2, PURPLE);
        //    Renderer::DrawPoint(createInfo.p3, PURPLE);
        //}

        if (GetEditorState() == EditorState::GIZMO_TRANSLATING) {


            if (GetEditorSelectionMode() == EditorSelectionMode::OBJECT) {
                World::SetObjectPosition(GetSelectedObjectId(), Gizmo::GetPosition());
            }
            else if (GetEditorSelectionMode() == EditorSelectionMode::VERTEX) {


                // HACK
                // HACK
                // HACK
                // HACK
                // HACK
                // HACK
                if (GetSelectedObjectType() == ObjectType::WALL) {
                    if (Wall* wall = World::GetWallByObjectId(GetSelectedObjectId())) {
                        if (wall->UpdatePointPosition(g_selectedVertexIndex, Gizmo::GetPosition())) {
                            World::UpdateHouseMeshBuffer();
                        }
                    }
                }


                // HACK
                // HACK
                // HACK
                // HACK
                // HACK
                // HACK
                if (GetSelectedObjectType() == ObjectType::HOUSE_PLANE) {
                    if (HousePlane* plane = World::GetHousePlaneByObjectId(GetSelectedObjectId())) {

                        HousePlaneCreateInfo& createInfo = plane->GetCreateInfo();

                        if (g_selectedVertexIndex == 0) {
                            createInfo.p0 = Gizmo::GetPosition();
                        }
                        if (g_selectedVertexIndex == 1) {
                            createInfo.p1 = Gizmo::GetPosition();
                        }
                        if (g_selectedVertexIndex == 2) {
                            createInfo.p2 = Gizmo::GetPosition();
                        }
                        if (g_selectedVertexIndex == 3) {
                            createInfo.p3 = Gizmo::GetPosition();
                        }
                        plane->UpdateVertexDataFromCreateInfo();
                            World::UpdateHouseMeshBuffer();
                    }
                }
            }

     

        }
        if (GetEditorState() == EditorState::GIZMO_ROTATING) {
            World::SetObjectRotation(GetSelectedObjectId(), Gizmo::GetRotation());
        }

    }

    void UnselectAnyObject() {
        SetSelectedObjectType(ObjectType::NO_TYPE);
        SetSelectedObjectId(0);
    }
}