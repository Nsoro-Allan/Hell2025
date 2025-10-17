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
        PhysXRayResult physxRayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);
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
        if (Plane* plane = World::GetPlaneByObjectId(GetSelectedObjectId())) {

            plane->DrawEdges(OUTLINE_COLOR);
            plane->DrawVertices(OUTLINE_COLOR);

            // Draw hovered verets and HACK to select it
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

            GenericObject* drawers = World::GetDrawersByObjectId(GetSelectedObjectId());
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

            if (GetSelectedObjectType() == ObjectType::PLANE) {
                Plane* plane = World::GetPlaneByObjectId(GetSelectedObjectId());
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

        float roomZmin = -1.8f;
        float roomZmax = 1.8f;
        float roomXmin = -2.75f;
        float roomXmax = 1.6f;

        float bathroomXmin = -2.715f;
        float bathroomXmax = -0.76f;
        float bathroomZmin = 1.9f;
        float bathroomZmax = 3.8f;

        float bathroomDoorX = -1.6f;
        float doorWidth = 0.9;
        float hallDoorX = 0.5f;


        glm::vec3 door0 = glm::vec3(hallDoorX + 0.39550f, 0, -1.85 - 0.058520);
        glm::vec3 door1 = glm::vec3(bathroomDoorX - 0.39550f, 0, bathroomZmin - 0.05f + 0.058520);
        glm::vec3 vase = glm::vec3(1.6f - 0.4f, 1.49f, 0.395f);
        glm::vec3 toilet = glm::vec3(-1.4, 0.0f, 3.8f);

        glm::vec3 wallAbegin = glm::vec3(roomXmax, 0, roomZmax);        glm::vec3 wallAend = glm::vec3(bathroomDoorX + doorWidth / 2, 0, roomZmax);
        glm::vec3 wallBbegin = glm::vec3(roomXmin, 0, roomZmin);        glm::vec3 wallBend = glm::vec3(hallDoorX - doorWidth / 2, 0, -roomZmax);
        glm::vec3 wallCbegin = glm::vec3(roomXmax, 0, roomZmin);        glm::vec3 wallCend = glm::vec3(roomXmax, 0, roomZmax);
        glm::vec3 wallDbegin = glm::vec3(roomXmin, 0, roomZmax);        glm::vec3 wallDend = glm::vec3(roomXmin, 0, roomZmin);

        glm::vec3 wallB2begin = glm::vec3(hallDoorX + doorWidth / 2, 0, roomZmin);      glm::vec3 wallB2end = glm::vec3(roomXmax, 0, roomZmin);
        glm::vec3 wallHbegin = glm::vec3(bathroomDoorX - doorWidth / 2, 0, roomZmax);   glm::vec3 wallHend = glm::vec3(roomXmin, 0, roomZmax);

        glm::vec3 wallE = glm::vec3(bathroomXmax, 0, bathroomZmin);        glm::vec3 wallEend = glm::vec3(bathroomXmax, 0, bathroomZmax);
        glm::vec3 wallG = glm::vec3(bathroomXmax, 0, bathroomZmax);        glm::vec3 wallGend = glm::vec3(bathroomXmin, 0, bathroomZmax);
        glm::vec3 wallI = glm::vec3(bathroomXmin, 0, bathroomZmax);        glm::vec3 wallIend = glm::vec3(bathroomXmin, 0, bathroomZmin);
        glm::vec3 wallHI2DOOR = glm::vec3(bathroomXmin, 0, bathroomZmin);  glm::vec3 wallHI2DOORend = glm::vec3(bathroomDoorX - doorWidth / 2, 0, bathroomZmin);

        float angleDegrees = 9.0f;
        float angleRadians = HELL_PI * 0.5f;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));

        door0 = glm::vec3(rotationMatrix * glm::vec4(door0, 1.0f));
        door1 = glm::vec3(rotationMatrix * glm::vec4(door1, 1.0f));
        vase = glm::vec3(rotationMatrix * glm::vec4(vase, 1.0f));
        toilet = glm::vec3(rotationMatrix * glm::vec4(toilet, 1.0f));

        wallAbegin = glm::vec3(rotationMatrix * glm::vec4(wallAbegin, 1.0f));
        wallAend = glm::vec3(rotationMatrix * glm::vec4(wallAend, 1.0f));
        wallBbegin = glm::vec3(rotationMatrix * glm::vec4(wallBbegin, 1.0f));
        wallBend = glm::vec3(rotationMatrix * glm::vec4(wallBend, 1.0f));
        wallCbegin = glm::vec3(rotationMatrix * glm::vec4(wallCbegin, 1.0f));
        wallCend = glm::vec3(rotationMatrix * glm::vec4(wallCend, 1.0f));
        wallDbegin = glm::vec3(rotationMatrix * glm::vec4(wallDbegin, 1.0f));
        wallDend = glm::vec3(rotationMatrix * glm::vec4(wallDend, 1.0f));

        wallB2begin = glm::vec3(rotationMatrix * glm::vec4(wallB2begin, 1.0f));
        wallB2end = glm::vec3(rotationMatrix * glm::vec4(wallB2end, 1.0f));
        wallHbegin = glm::vec3(rotationMatrix * glm::vec4(wallHbegin, 1.0f));
        wallHend = glm::vec3(rotationMatrix * glm::vec4(wallHend, 1.0f));

        wallE = glm::vec3(rotationMatrix * glm::vec4(wallE, 1.0f));
        wallEend = glm::vec3(rotationMatrix * glm::vec4(wallEend, 1.0f));
        wallG = glm::vec3(rotationMatrix * glm::vec4(wallG, 1.0f));
        wallGend = glm::vec3(rotationMatrix * glm::vec4(wallGend, 1.0f));
        wallI = glm::vec3(rotationMatrix * glm::vec4(wallI, 1.0f));
        wallIend = glm::vec3(rotationMatrix * glm::vec4(wallIend, 1.0f));
        wallHI2DOOR = glm::vec3(rotationMatrix * glm::vec4(wallHI2DOOR, 1.0f));
        wallHI2DOORend = glm::vec3(rotationMatrix * glm::vec4(wallHI2DOORend, 1.0f));

        glm::vec3 translationVector = glm::vec3(6.3f, 0.0f, 0.7f); // Example translation values
        translationVector = glm::vec3(6.3f, 0.0f, -1.3f); // Example translation values

        door0 += translationVector;
        door1 += translationVector;
        vase += translationVector;
        toilet += translationVector;

        wallAbegin += translationVector;
        wallAend += translationVector;
        wallBbegin += translationVector;
        wallBend += translationVector;
        wallCbegin += translationVector;
        wallCend += translationVector;
        wallDbegin += translationVector;
        wallDend += translationVector;

        wallB2begin += translationVector;
        wallB2end += translationVector;
        wallHbegin += translationVector;
        wallHend += translationVector;

        wallE += translationVector;
        wallEend += translationVector;
        wallG += translationVector;
        wallGend += translationVector;
        wallI += translationVector;
        wallIend += translationVector;
        wallHI2DOOR += translationVector;
        wallHI2DOORend += translationVector;

    /*	X		   C
        |	  ___________
        |	 |	     Wife|
        |	 |			 |A
        |	 |			 |   E
        |  B |			 |______
        |	 |			 |      |
        |	 |			 |______| G
        |	 |___________|H  I
        |
        |		   D        F
        (0,0)----------------- Z
    */

        Renderer::DrawPoint(door0, RED);
        Renderer::DrawPoint(door1, BLUE);
        Renderer::DrawPoint(vase, GREEN);
        Renderer::DrawPoint(toilet, PINK);

        Renderer::DrawLine(wallAbegin, wallAend, YELLOW);
        Renderer::DrawLine(wallBbegin, wallBend, YELLOW);
        Renderer::DrawLine(wallCbegin, wallCend, YELLOW);
        Renderer::DrawLine(wallDbegin, wallDend, YELLOW);

        Renderer::DrawLine(wallHbegin, wallHend, YELLOW);
        Renderer::DrawLine(wallB2begin, wallB2end, YELLOW);

        Renderer::DrawLine(wallE, wallEend, YELLOW);
        Renderer::DrawLine(wallG, wallGend, YELLOW);
        Renderer::DrawLine(wallI, wallIend, YELLOW);
        Renderer::DrawLine(wallHI2DOOR, wallHI2DOORend, YELLOW);

        UpdateGizmoInteract();

        if (Input::KeyPressed(HELL_KEY_SPACE)) {
            int wallCount = World::GetWalls().size() - 1;
            Wall& wall = World::GetWalls()[wallCount];
            wall.SetMaterial("BathroomWall");
            wall.SetFloorTrimType(TrimType::NONE);
            World::UpdateHouseMeshBuffer();
        }

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

        for (Plane& plane : World::GetPlanes()) {

            PlaneCreateInfo createInfo = plane.GetCreateInfo();
            Renderer::DrawPoint(createInfo.p0, PURPLE);
            Renderer::DrawPoint(createInfo.p1, PURPLE);
            Renderer::DrawPoint(createInfo.p2, PURPLE);
            Renderer::DrawPoint(createInfo.p3, PURPLE);
        }

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
                if (GetSelectedObjectType() == ObjectType::PLANE) {
                    if (Plane* plane = World::GetPlaneByObjectId(GetSelectedObjectId())) {

                        PlaneCreateInfo& createInfo = plane->GetCreateInfo();

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