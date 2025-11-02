#include "Gizmo.h"
#include "HellEnums.h"
#include "HellConstants.h"

#include "Audio/Audio.h"
#include "Config/Config.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Util/Util.h"   
#include "Viewport/ViewportManager.h"

#include <glm/gtx/norm.hpp> 
#include "glm/gtx/intersect.hpp"

namespace Gizmo {
    enum MeshIndex {
        RING = 0,
        SPHERE,
        CONE,
        CYLINDER,
        CUBE,
        MESH_COUNT
    };

    struct RotationDragState {
        bool active = false;
        GizmoFlag axisFlag = GizmoFlag::NONE;
        glm::vec3 center = glm::vec3(0);
        glm::vec3 axisWorld = glm::vec3(0, 1, 0); // Locked at mouse-down
        glm::vec3 basisU = glm::vec3(1, 0, 0);    // Spans plane with basisV
        glm::vec3 basisV = glm::vec3(0, 1, 0);
        glm::quat startRot;                       // Starting gizmo rotation
        float startAngle = 0.0f;                  // Atan2 angle on plane at mouse down
    } g_rotDrag;

    inline glm::vec3 ProjectOntoPlane(glm::vec3 v, glm::vec3 n) { return v - n * glm::dot(v, n); }

    inline void BuildPlaneBasis(const glm::vec3& planeNormal, const glm::vec3& cameraRight, glm::vec3& outU, glm::vec3& outV) {
        glm::vec3 u = ProjectOntoPlane(cameraRight, planeNormal);
        if (glm::length2(u) < 1e-6f) {
            // Fallback if cameraRight is nearly parallel to normal
            u = ProjectOntoPlane(glm::abs(planeNormal.y) > 0.5f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0), planeNormal);
        }
        outU = glm::normalize(u);
        outV = glm::normalize(glm::cross(planeNormal, outU));
    }

    inline float AngleOnBasis(const glm::vec3& pOnPlane, const glm::vec3& center, const glm::vec3& U, const glm::vec3& V) {
        glm::vec3 r = glm::normalize(pOnPlane - center);
        float x = glm::dot(r, U);
        float y = glm::dot(r, V);
        return std::atan2(y, x);
    }
    
    inline glm::mat3 QuatToMat3(const glm::quat& q) {
        return glm::mat3_cast(q);
    }

    float g_gizmoSize = 1.0f;
    float g_armLength = 1.0f;
    glm::vec3 g_gizmoPosition = glm::vec3(0.0, 0.0f, 0.0f);
    glm::quat g_gizmoRotationQ = glm::quat(glm::vec3(0.0f));
    std::vector<GizmoRenderItem> g_renderItems[4];
    std::vector<MeshBuffer> g_meshBuffers;
    GizmoFlag g_hoverFlag = GizmoFlag::NONE;
    GizmoFlag g_actionFlag = GizmoFlag::NONE;
    GizmoAction g_action = GizmoAction::IDLE;
    GizmoMode g_mode = GizmoMode::TRANSLATE;
    bool g_offsetNeedsUpdate = false;
    bool g_gizmoHasHover = false;
    glm::vec3 g_translateOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::ivec2 g_scaleOffset = glm::ivec2(0, 0);

    glm::vec3 g_localUpAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 g_localRightAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 g_localForwardAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 g_rotationRayHitPosPreviousFrame = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 g_rotationRayHitPosThisFrame = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 g_sourceObjectOffset = glm::vec3(0.0f);
    
    void UpdateInput();
    void UpdateLocalAxes();

    void Init() {        
        g_meshBuffers.resize(MESH_COUNT);

        // Generate ring mesh
        float ringThickness = 0.03f;
        int ringSegments = 32;
        int ringThicknessSegments = 5;
        std::vector<Vertex> ringVertices = Util::GenerateRingVertices(g_gizmoSize, ringThickness, ringSegments, ringThicknessSegments);
        std::vector<uint32_t> ringIndices = Util::GenerateRingIndices(ringSegments, ringThicknessSegments);
        g_meshBuffers[RING].AddMesh(ringVertices, ringIndices);
        g_meshBuffers[RING].UpdateBuffers();

        // Generate sphere mesh
        float sphereRadius = g_gizmoSize - (ringThickness * 2);
        int sphereSegments = 32;
        std::vector<Vertex> sphereVerices = Util::GenerateSphereVertices(sphereRadius, sphereSegments);
        std::vector<uint32_t> sphereIndices = Util::GenerateSphereIndices(sphereSegments);
        g_meshBuffers[SPHERE].AddMesh(sphereVerices, sphereIndices);
        g_meshBuffers[SPHERE].UpdateBuffers();

        // Generate cone mesh
        int coneSegments = 12;
        float coneRadius = 0.125f;
        float coneHeight = 0.6f;
        std::vector<Vertex> coneVertices = Util::GenerateConeVertices(coneRadius, coneHeight, coneSegments);
        std::vector<uint32_t> coneIndices = Util::GenerateConeIndices(coneSegments);
        g_meshBuffers[CONE].AddMesh(coneVertices, coneIndices);
        g_meshBuffers[CONE].UpdateBuffers();

        // Generate cone mesh
        float cylinderRadius = 0.015f;
        float cylinderHeight = 1.0f;
        int cylinderSegments = 5;
        std::vector<Vertex> cylinderVertices = Util::GenerateCylinderVertices(cylinderRadius, cylinderHeight, cylinderSegments);
        std::vector<uint32_t> cylinderIndices = Util::GenerateCylinderIndices(cylinderSegments);
        g_meshBuffers[CYLINDER].AddMesh(cylinderVertices, cylinderIndices);
        g_meshBuffers[CYLINDER].UpdateBuffers();

        // Generate cube one mesh
        std::vector<Vertex> cubeVertices = Util::GenerateCubeVertices();
        std::vector<uint32_t> cubeIndices = Util::GenerateCubeIndices();
        g_meshBuffers[CUBE].AddMesh(cubeVertices, cubeIndices);
        g_meshBuffers[CUBE].UpdateBuffers();
    }

    MeshBuffer* GetMeshBufferByIndex(int index) {
        if (index >= 0 && index < static_cast<int>(g_meshBuffers.size())) {
            return g_meshBuffers.data() + index;
        }
        else {
            return nullptr;
        }
    }

    void Update() {
        if (!Editor::IsOpen()) return;
        UpdateLocalAxes();
        UpdateInput();
        UpdateRenderItems();
    }

    void UpdateLocalAxes() {
        if (GetMode() == GizmoMode::ROTATE) {
            glm::mat3 R = QuatToMat3(g_gizmoRotationQ);
            g_localRightAxis = R * glm::vec3(1, 0, 0);
            g_localUpAxis = R * glm::vec3(0, 1, 0);
            g_localForwardAxis = R * glm::vec3(0, 0, 1);
        }
        else {
            g_localUpAxis = glm::vec3(0, 1, 0);
            g_localRightAxis = glm::vec3(1, 0, 0);
            g_localForwardAxis = glm::vec3(0, 0, 1);
        }
    }

    void UpdateInput() {
        int viewportIndex = Editor::GetHoveredViewportIndex();
        const Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        if (!viewport) return;

        const glm::vec3 rayOrigin = Editor::GetMouseRayOriginByViewportIndex(viewportIndex);
        const glm::vec3 rayDir = Editor::GetMouseRayDirectionByViewportIndex(viewportIndex);

        glm::mat4 viewMatrix = Editor::GetViewportViewMatrix(viewportIndex);

        glm::mat4 projectionMatrix = viewport->GetProjectionMatrix();
        //glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projectionView = projectionMatrix * viewMatrix;
        glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
        glm::vec3 camRight = glm::vec3(inverseViewMatrix[0]);
        glm::vec3 camUp = glm::vec3(inverseViewMatrix[1]);
        glm::vec3 camForward = glm::vec3(inverseViewMatrix[2]);
        glm::vec3 viewPos = inverseViewMatrix[3];

        // Toggle mode
        if (Input::KeyPressed(HELL_KEY_T) && g_mode != GizmoMode::TRANSLATE) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            g_mode = GizmoMode::TRANSLATE;
        }
        if (Input::KeyPressed(HELL_KEY_R) && g_mode != GizmoMode::ROTATE) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            g_mode = GizmoMode::ROTATE;
        }
        if (Input::KeyPressed(HELL_KEY_S) && g_mode != GizmoMode::SCALE) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            g_mode = GizmoMode::SCALE;
        }

        // Get mouse ray direction
        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();

        // Raycast against all render item triangles to find hover
        float closestDistance = 9999;
        g_gizmoHasHover = false;
        g_hoverFlag = GizmoFlag::NONE;
        for (GizmoRenderItem& renderItem : g_renderItems[viewportIndex]) {
            MeshBuffer* mesh = Gizmo::GetMeshBufferByIndex(renderItem.meshIndex);
            if (mesh) {
                std::vector<Vertex>& vertices = mesh->GetVertices();
                std::vector<uint32_t>& indices = mesh->GetIndices();

                for (int i = 0; i < indices.size(); i += 3) {
                    Vertex& v0 = vertices[mesh->GetIndices()[i + 0]];
                    Vertex& v1 = vertices[mesh->GetIndices()[i + 1]];
                    Vertex& v2 = vertices[mesh->GetIndices()[i + 2]];
                    glm::vec3 pos0 = renderItem.modelMatrix * glm::vec4(v0.position, 1.0f);
                    glm::vec3 pos1 = renderItem.modelMatrix * glm::vec4(v1.position, 1.0f);
                    glm::vec3 pos2 = renderItem.modelMatrix * glm::vec4(v2.position, 1.0f);

                    float t = 0;

                    if (Util::RayIntersectsTriangle(rayOrigin, rayDir, pos0, pos1, pos2, t)) {
                        if (t < closestDistance) {
                            closestDistance = t;
                            g_hoverFlag = renderItem.flag;
                            g_gizmoHasHover = true;
                        }
                    }
                }
            }
        }

        // Translating
        if (g_actionFlag == GizmoFlag::TRANSLATE_X || g_actionFlag == GizmoFlag::TRANSLATE_Y || g_actionFlag == GizmoFlag::TRANSLATE_Z) {
            if (g_action == GizmoAction::DRAGGING) {
                glm::vec3 planeNormal = -rayDir;
                glm::vec3 planeOrigin = glm::vec3(g_gizmoPosition);
                float distanceToHit = 0;
                bool hitFound = glm::intersectRayPlane(rayOrigin, rayDir, planeOrigin, planeNormal, distanceToHit);

                if (hitFound) {
                    glm::vec3 hitPosition = rayOrigin + (rayDir * distanceToHit);
                    if (g_offsetNeedsUpdate) {
                        g_offsetNeedsUpdate = false;
                        g_translateOffset = hitPosition - g_gizmoPosition;
                    }

                    switch (g_actionFlag) {
                        case GizmoFlag::TRANSLATE_X: g_gizmoPosition.x = hitPosition.x - g_translateOffset.x; break;
                        case GizmoFlag::TRANSLATE_Y: g_gizmoPosition.y = hitPosition.y - g_translateOffset.y; break;
                        case GizmoFlag::TRANSLATE_Z: g_gizmoPosition.z = hitPosition.z - g_translateOffset.z; break;
                    }

                    // Snap to grid
                    if (Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
                        g_gizmoPosition = glm::round(g_gizmoPosition * 20.0f) / 20.0f;
                    }
                }
            }
        }

        if (g_actionFlag == GizmoFlag::SCALE_X ||
            g_actionFlag == GizmoFlag::SCALE_Y ||
            g_actionFlag == GizmoFlag::SCALE_Z)
        {
            if (g_action == GizmoAction::DRAGGING) {

                if (g_offsetNeedsUpdate) {
                    glm::vec3 armOffset = glm::vec3(
                        (g_actionFlag == GizmoFlag::SCALE_X) ? g_armLength * GetGizmoScalingFactorByViewportIndex(viewportIndex) : 0.0f,
                        (g_actionFlag == GizmoFlag::SCALE_Y) ? g_armLength * GetGizmoScalingFactorByViewportIndex(viewportIndex) : 0.0f,
                        (g_actionFlag == GizmoFlag::SCALE_Z) ? g_armLength * GetGizmoScalingFactorByViewportIndex(viewportIndex) : 0.0f
                    );
                    glm::mat4 mvpArm = projectionMatrix * viewMatrix * Transform(g_gizmoPosition + armOffset).to_mat4();
                    glm::ivec2 centerScreenCoords = Util::WorldToScreenCoords(g_gizmoPosition, projectionView, windowWidth, windowHeight, true);
                    glm::ivec2 armScreenCoords = Util::WorldToScreenCoords(g_gizmoPosition + armOffset, projectionView, windowWidth, windowHeight, true);
                    g_scaleOffset = armScreenCoords - centerScreenCoords;
                    g_offsetNeedsUpdate = false;

                    std::cout << "\n";
                    std::cout << "Mouse: " << mouseX << " " << mouseY << "\n";
                    std::cout << "Center: " << centerScreenCoords.x << " " << centerScreenCoords.y << "\n";
                    std::cout << "Arm: " << armScreenCoords.x << " " << armScreenCoords.y << "\n";
                    std::cout << "Offset: " << g_scaleOffset.x << " " << g_scaleOffset.y << "\n";
                }
            }            
        }

        // Begin rotate
        if ((g_hoverFlag == GizmoFlag::ROTATE_X || g_hoverFlag == GizmoFlag::ROTATE_Y || g_hoverFlag == GizmoFlag::ROTATE_Z) &&
            Input::LeftMousePressed() && g_action == GizmoAction::IDLE) {

            g_action = GizmoAction::DRAGGING;
            g_actionFlag = g_hoverFlag;
            g_rotDrag.active = true;
            g_rotDrag.axisFlag = g_hoverFlag;
            g_rotDrag.center = g_gizmoPosition;
            g_rotDrag.startRot = g_gizmoRotationQ;

            glm::vec3 axis;
            switch (g_hoverFlag) {
                case GizmoFlag::ROTATE_X: axis = g_localRightAxis; break;
                case GizmoFlag::ROTATE_Y: axis = g_localUpAxis; break;
                default:                  axis = g_localForwardAxis; break;
            }
            g_rotDrag.axisWorld = glm::normalize(axis);

            // Intersect with locked plane
            float t = 0.0f;
            if (glm::intersectRayPlane(rayOrigin, rayDir, g_rotDrag.center, g_rotDrag.axisWorld, t)) {
                glm::vec3 hit = rayOrigin + rayDir * t;
                // Choose a stable 2D basis on the plane using camera right
                glm::mat4 invV = glm::inverse(Editor::GetViewportViewMatrix(viewportIndex));
                glm::vec3 camRight = glm::vec3(invV[0]);
                BuildPlaneBasis(g_rotDrag.axisWorld, camRight, g_rotDrag.basisU, g_rotDrag.basisV);
                g_rotDrag.startAngle = AngleOnBasis(hit, g_rotDrag.center, g_rotDrag.basisU, g_rotDrag.basisV);
                g_rotationRayHitPosPreviousFrame = hit;
                g_rotationRayHitPosThisFrame = hit;
            }
        }

        // Rotate
        if (g_rotDrag.active && g_action == GizmoAction::DRAGGING &&
            (g_actionFlag == GizmoFlag::ROTATE_X || g_actionFlag == GizmoFlag::ROTATE_Y || g_actionFlag == GizmoFlag::ROTATE_Z)) {

            float t = 0.0f;
            if (glm::intersectRayPlane(rayOrigin, rayDir, g_rotDrag.center, g_rotDrag.axisWorld, t)) {
                glm::vec3 hit = rayOrigin + rayDir * t;
                float angleNow = AngleOnBasis(hit, g_rotDrag.center, g_rotDrag.basisU, g_rotDrag.basisV);
                float delta = angleNow - g_rotDrag.startAngle;

                // Snapping
                if (Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
                    const float snap = glm::radians(5.0f);
                    delta = std::round(delta / snap) * snap;
                }

                float side = glm::dot(camForward, g_rotDrag.axisWorld) < 0.0f ? -1.0f : 1.0f;
                delta *= side;

                // Apply about locked axis in world
                glm::quat dq = glm::angleAxis(delta, g_rotDrag.axisWorld);
                g_gizmoRotationQ = glm::normalize(dq * g_rotDrag.startRot);
                g_rotationRayHitPosPreviousFrame = g_rotationRayHitPosThisFrame;
                g_rotationRayHitPosThisFrame = hit;
            }
        }

        // Gizmo selection
        if (Input::LeftMousePressed() && g_hoverFlag != GizmoFlag::NONE) {
            g_action = GizmoAction::DRAGGING;
            g_actionFlag = g_hoverFlag;
            g_offsetNeedsUpdate = true;
        }

        // User ended drag
        if (!Input::LeftMouseDown()) {
            if (g_action != GizmoAction::IDLE) {
                g_action = GizmoAction::IDLE;
                g_actionFlag = GizmoFlag::NONE;
                g_rotDrag = RotationDragState{};
            }
        }
    }

    

    void UpdateRenderItems() {

        if (!Editor::IsOpen()) return;

        for (int i = 0; i < 4; i++) {
            g_renderItems[i].clear();

            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            glm::mat4 projectionMatrix = viewport->GetProjectionMatrix();

            glm::mat4 viewMatrix = Editor::GetViewportViewMatrix(i);
            glm::mat4 projectionView = projectionMatrix * viewMatrix;
            glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
            glm::vec3 camRight = glm::vec3(inverseViewMatrix[0]);
            glm::vec3 camUp = glm::vec3(inverseViewMatrix[1]);
            glm::vec3 camForward = glm::vec3(inverseViewMatrix[2]);
            glm::vec3 camPos = inverseViewMatrix[3];

            float scaleCubeSize = 0.23f;
            float coneOffset = 0.9f;

            // Scale the gizmo based on camera distance.
            float scalingFactor = GetGizmoScalingFactorByViewportIndex(i);
            Transform transform;
            transform.position = g_gizmoPosition;
            transform.scale = glm::vec3(scalingFactor);

            if (g_mode == GizmoMode::TRANSLATE) {
                GizmoRenderItem& centerCube = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor * 0.2f);
                centerCube.modelMatrix = transform.to_mat4();
                centerCube.meshIndex = CUBE;
                centerCube.flag = GizmoFlag::NONE;
                centerCube.color = YELLOW;

                GizmoRenderItem& cylinderX = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, HELL_PI * -0.5f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderX.modelMatrix = transform.to_mat4();
                cylinderX.meshIndex = CYLINDER;
                cylinderX.flag = GizmoFlag::TRANSLATE_X;
                cylinderX.color = RED;

                GizmoRenderItem& cylinderY = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderY.modelMatrix = transform.to_mat4();
                cylinderY.meshIndex = CYLINDER;
                cylinderY.flag = GizmoFlag::TRANSLATE_Y;
                cylinderY.color = GREEN;

                GizmoRenderItem& cylinderZ = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(HELL_PI * 0.5f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderZ.modelMatrix = transform.to_mat4();
                cylinderZ.meshIndex = CYLINDER;
                cylinderZ.flag = GizmoFlag::TRANSLATE_Z;
                cylinderZ.color = BLUE;

                GizmoRenderItem& coneX = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(coneOffset * scalingFactor, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, HELL_PI * -0.5f);
                transform.scale = glm::vec3(scalingFactor);
                coneX.modelMatrix = transform.to_mat4();
                coneX.meshIndex = CONE;
                coneX.flag = GizmoFlag::TRANSLATE_X;
                coneX.color = RED;

                GizmoRenderItem& coneY = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, coneOffset * scalingFactor, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                coneY.modelMatrix = transform.to_mat4();
                coneY.meshIndex = CONE;
                coneY.flag = GizmoFlag::TRANSLATE_Y;
                coneY.color = GREEN;

                GizmoRenderItem& coneZ = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, coneOffset * scalingFactor);
                transform.rotation = glm::vec3(HELL_PI * 0.5f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                coneZ.modelMatrix = transform.to_mat4();
                coneZ.meshIndex = CONE;
                coneZ.flag = GizmoFlag::TRANSLATE_Z;
                coneZ.color = BLUE;
            }

            if (g_mode == GizmoMode::SCALE) {
                GizmoRenderItem& centerCube = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor * 0.2f);
                centerCube.modelMatrix = transform.to_mat4();
                centerCube.meshIndex = CUBE;
                centerCube.flag = GizmoFlag::SCALE;
                centerCube.color = YELLOW;

                GizmoRenderItem& cylinderX = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, HELL_PI * -0.5f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderX.modelMatrix = transform.to_mat4();
                cylinderX.meshIndex = CYLINDER;
                cylinderX.flag = GizmoFlag::SCALE_X;
                cylinderX.color = RED;

                GizmoRenderItem& cylinderY = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderY.modelMatrix = transform.to_mat4();
                cylinderY.meshIndex = CYLINDER;
                cylinderY.flag = GizmoFlag::SCALE_Y;
                cylinderY.color = GREEN;

                GizmoRenderItem& cylinderZ = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(HELL_PI * 0.5f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                cylinderZ.modelMatrix = transform.to_mat4();
                cylinderZ.meshIndex = CYLINDER;
                cylinderZ.flag = GizmoFlag::SCALE_Z;
                cylinderZ.color = BLUE;

                GizmoRenderItem& coneX = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(g_armLength * scalingFactor, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor * scaleCubeSize);
                coneX.modelMatrix = transform.to_mat4();
                coneX.meshIndex = CUBE;
                coneX.flag = GizmoFlag::SCALE_X;
                coneX.color = RED;
                
                GizmoRenderItem& coneY = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, g_armLength * scalingFactor, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor * scaleCubeSize);
                coneY.modelMatrix = transform.to_mat4();
                coneY.meshIndex = CUBE;
                coneY.flag = GizmoFlag::SCALE_Y;
                coneY.color = GREEN;

                GizmoRenderItem& coneZ = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, g_armLength * scalingFactor);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor * scaleCubeSize);
                coneZ.modelMatrix = transform.to_mat4();
                coneZ.meshIndex = CUBE;
                coneZ.flag = GizmoFlag::SCALE_Z;
                coneZ.color = BLUE;
            }

            if (g_mode == GizmoMode::ROTATE) {
                // Rotate 
                GizmoRenderItem& sphere = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                sphere.modelMatrix = transform.to_mat4();
                sphere.meshIndex = SPHERE;
                sphere.color = TRANSPARENT;
                
                // Rotate X
                GizmoRenderItem& rotateX = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, HELL_PI * 0.5f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                rotateX.modelMatrix = transform.to_mat4();
                rotateX.meshIndex = RING;
                rotateX.flag = GizmoFlag::ROTATE_X;
                rotateX.color = RED;

                // Rotate Y
                GizmoRenderItem& rotateY = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(HELL_PI * 0.5f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                rotateY.modelMatrix = transform.to_mat4();
                rotateY.meshIndex = RING;
                rotateY.flag = GizmoFlag::ROTATE_Y;
                rotateY.color = GREEN;

                // Rotate Z
                GizmoRenderItem& rotateZ = g_renderItems[i].emplace_back();
                transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                transform.scale = glm::vec3(scalingFactor);
                rotateZ.modelMatrix = transform.to_mat4();
                rotateZ.meshIndex = RING;
                rotateZ.flag = GizmoFlag::ROTATE_Z;
                rotateZ.color = BLUE;
            }

            for (GizmoRenderItem& renderItem : g_renderItems[i]) {
                if (renderItem.flag == g_hoverFlag && renderItem.flag != GizmoFlag::NONE) {
                    renderItem.color = ORANGE;
                }
            }
        }

        // Final transform
        Transform transform;
        transform.position = g_gizmoPosition + g_sourceObjectOffset;
        
        if (GetMode() == GizmoMode::ROTATE) {
            transform.rotation = GetRotation();
        }

        for (int i = 0; i < 4; i++) {
            for (GizmoRenderItem& renderItem : g_renderItems[i]) {
                renderItem.modelMatrix = transform.to_mat4() * renderItem.modelMatrix;
            }
        }
    }

    std::vector<GizmoRenderItem>& GetRenderItemsByViewportIndex(int index) {
        return g_renderItems[index];
    }

    void SetPosition(const glm::vec3& position) {
        g_gizmoPosition = position;
    }

    void SetRotation(const glm::vec3& rotation) {
        g_gizmoRotationQ = glm::normalize(glm::quat(rotation));
    }

    void SetSourceObjectOffeset(const glm::vec3& offset) {
        g_sourceObjectOffset = offset;
    }

    const std::string GizmoFlagToString(const GizmoFlag& flag) {
        switch (flag) {
        case GizmoFlag::NONE: return "NONE";
        case GizmoFlag::TRANSLATE_X: return "TRANSLATE_X";
        case GizmoFlag::TRANSLATE_Y: return "TRANSLATE_Y";
        case GizmoFlag::TRANSLATE_Z: return "TRANSLATE_Z";
        case GizmoFlag::ROTATE_X: return "ROTATE_X";
        case GizmoFlag::ROTATE_Y: return "ROTATE_Y";
        case GizmoFlag::ROTATE_Z: return "ROTATE_Z";
        case GizmoFlag::SCALE_X: return "SCALE_X";
        case GizmoFlag::SCALE_Y: return "SCALE_Y";
        case GizmoFlag::SCALE_Z: return "SCALE_Z";
        default: return "UNKNOWN"; // Handle unexpected cases
        }
    }

    float GetGizmoScalingFactorByViewportIndex(int viewportIndex) {
        Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        if (!viewport) return 0.0f;

        const Resolutions& resolutions = Config::GetResolutions();
        float desiredGizmoHeightPixels = 75.0f;

        int renderTargetWidth = resolutions.gBuffer.x;
        int renderTargetHeight = resolutions.gBuffer.y;
        float viewportWidth = viewport->GetSize().x * renderTargetWidth;
        float viewportHeight = viewport->GetSize().y * renderTargetHeight;

        if (viewport->IsOrthographic()) {
            float m_aspect = viewportWidth / viewportHeight;
            float left = -viewport->GetOrthoSize() * m_aspect;
            float right = viewport->GetOrthoSize() * m_aspect;
            float bottom = -viewport->GetOrthoSize();
            float top = viewport->GetOrthoSize();
            float worldHeight = top - bottom;
            float worldPerPixel = worldHeight / viewportHeight;
            float gizmoHeightInWorld = desiredGizmoHeightPixels * worldPerPixel;
            return gizmoHeightInWorld;
        }
        else {
            glm::mat4 viewMatrix = Editor::GetViewportViewMatrix(viewportIndex);
            glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
            glm::vec3 camPos = glm::vec3(inverseViewMatrix[3]);
            float distance = glm::length(g_gizmoPosition - camPos);
            float fov = viewport->GetPerspectiveFOV(); // radians
            float worldHeightAtDist = 2.0f * distance * tanf(fov * 0.5f);
            float worldPerPixel = worldHeightAtDist / viewportHeight;
            return desiredGizmoHeightPixels * worldPerPixel;
        }
    }

    const glm::vec3 GetPosition() {
        return g_gizmoPosition;
    }

    const glm::vec3 GetRotation() {
        return glm::eulerAngles(g_gizmoRotationQ);;
    }

    const bool HasHover() {
        return g_gizmoHasHover;
    }

    const GizmoAction GetAction() {
        return g_action;
    }

    const GizmoMode GetMode() {
        return g_mode;
    }
}