#include "Editor.h"
#include "Audio/Audio.h"
#include "Editor/Gizmo.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace Editor {

    void UpdateObjectHover() {
        // Reset values from last frame
        SetHoveredObjectType(ObjectType::NONE);
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
                SetHoveredObjectType(bvhRayResult.objectType);
                SetHoveredObjectId(bvhRayResult.objectId);
            }
        }

        ObjectType hovererdType = Editor::GetHoveredObjectType();

        // Find parents if necessary
        if (GetHoveredObjectType() == ObjectType::DOOR_FRAME) {
            Door* door = World::GetDoorByDoorFrameObjectId(GetHoveredObjectId());
            if (door) {
                SetHoveredObjectType(ObjectType::DOOR);
                SetHoveredObjectId(door->GetObjectId());
            }
        }
        if (GetHoveredObjectType() == ObjectType::PIANO_TOP_COVER ||
            GetHoveredObjectType() == ObjectType::PIANO_KEY ||
            GetHoveredObjectType() == ObjectType::PIANO_SHEET_MUSIC_REST ||
            GetHoveredObjectType() == ObjectType::PIANO_SHEET_SUSTAIN_PEDAL ||
            GetHoveredObjectType() == ObjectType::PIANO_KEYBOARD_COVER) {
            Piano* piano = World::GetPianoByMeshNodeObjectId(GetHoveredObjectId());
            if (piano) {
                SetHoveredObjectType(ObjectType::PIANO);
                SetHoveredObjectId(piano->GetObjectId());
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
        if (GetEditorState() != EditorState::IDLE) return;

        if (Input::LeftMousePressed() && !Gizmo::HasHover()) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            SetSelectedObjectType(GetHoveredObjectType());
            SetSelectedObjectId(GetHoveredObjectId());

            std::cout << "Selected: " << Util::ObjectTypeToString(GetSelectedObjectType()) << " " << GetSelectedObjectId() << "\n";

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
                    Gizmo::SetPosition(plane->GetWorldSpaceCenter());
                }
            }

            if (GetSelectedObjectType() == ObjectType::PICTURE_FRAME) {
                PictureFrame* pictureFrame = World::GetPictureFrameByObjectId(GetSelectedObjectId());
                if (pictureFrame) {
                    Gizmo::SetPosition(pictureFrame->GetPosition());
                }
            }

            if (GetSelectedObjectType() == ObjectType::WALL) {
                Wall* wall = World::GetWallByObjectId(GetSelectedObjectId());
                if (wall) {
                    Gizmo::SetPosition(wall->GetWorldSpaceCenter());
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
        }
    }

    void UpdateObjectGizmoInteraction() {

        UpdateGizmoInteract();

        if (GetEditorState() == EditorState::GIZMO_TRANSLATING) {
            World::SetObjectPosition(GetSelectedObjectId(), Gizmo::GetPosition());
        }

    }

    void UnselectAnyObject() {
        SetSelectedObjectType(ObjectType::NONE);
        SetSelectedObjectId(0);
    }
}