#pragma once
#include "CreateInfo.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Math/AABB.h"
#include "Types/Animation/Animation.h"
#include <vector>
#include <filesystem>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <span>
#include <type_traits>

#include <vector>

namespace Util {
    // Black magic
    void PackUint64(uint64_t value, uint32_t& xOut, uint32_t& yOut);
    void UnpackUint64(uint32_t xValue, uint32_t yValue, uint64_t& out);

    // Math
    glm::vec3 EulerRotationFromNormal(glm::vec3 normal, glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f));
    float YRotationBetweenTwoPoints(glm::vec3 a, glm::vec3 b);
    glm::mat4 GetRotationMat4FromForwardVector(glm::vec3 forward);
    glm::vec3 GetMidPoint(const glm::vec3& a, const glm::vec3& b);
    float EulerYRotationBetweenTwoPoints(glm::vec3 a, glm::vec3 b);
    glm::mat4 RotationMatrixFromForwardVector(glm::vec3 forward, glm::vec3 worldForward, glm::vec3 worldUp);
    glm::vec2 ComputeCentroid2D(const std::vector<glm::vec2>& points);
    std::vector<glm::vec2> SortConvexHullPoints2D(std::vector<glm::vec2>&points);
    std::vector<glm::vec2> ComputeConvexHull2D(std::vector<glm::vec2> points);
    float Cross2D(const glm::vec2& O, const glm::vec2& A, const glm::vec2& B);
    glm::vec3 ClosestPointOnSegmentToRay(const glm::vec3& A, const glm::vec3& B, const glm::vec3& rayOrigin, const glm::vec3& rayDir);
    float DistanceSquared(const glm::vec3& a, const glm::vec3& b);
    float ManhattanDistance(const glm::vec3& a, const glm::vec3& b);
    int RandomInt(int min, int max);
    void NormalizeWeights(std::vector<float>& weights); 
    void InterpolateQuaternion(glm::quat& Out, const glm::quat& Start, const glm::quat& End, float pFactor);
    float FInterpTo(float current, float target, float deltaTime, float interpSpeed);
    glm::vec3 LerpVec3(glm::vec3 current, glm::vec3 target, float deltaTime, float interpSpeed);
    float RandomFloat(float min, float max);
    bool IsWithinThreshold(const glm::ivec2& pointA, const glm::ivec2& pointB, float threshold);
    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY = false);
    //glm::ivec2 WorldToScreenCoordsOrtho(const glm::vec3& worldPos, const glm::mat4& orthoMatrix, int screenWidth, int screenHeight, bool flipY = false);
    glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b);
    glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b);
    bool IsNan(float value);
    bool IsNan(glm::vec2 value);
    bool IsNan(glm::vec3 value);
    bool IsNan(glm::vec4 value);
    float GetDensity(float mass, float volume);
    float GetConvexHullVolume(const std::span<Vertex>& vertices, const std::span<unsigned int>& indices);
    float GetCubeVolume(const glm::vec3& halfExtents);
    float GetCubeVolume(const float& halfWidth, const float& halfHeight, const float& halfDepth);
    float GetSphereVolume(float radius);
    float GetCapsuleVolume(float radius, float halfHeight);
    AABB GetAABBFromPoints(std::vector<glm::vec3>& points);
    bool Mat4NearlyEqual(const glm::mat4& a, const glm::mat4& b);
    bool NearlyEqualTransform(const Transform& a, const Transform& b);
    bool IsPointInTriangle2D(const glm::vec2& pt, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2);
    std::vector<glm::vec3> GetBeizerPointsFromControlPoints(const std::vector<glm::vec3>& controlPoints, float spacing);
    bool HoveredLine(glm::ivec2 mouseCoords, glm::ivec2 p1, glm::ivec2 p2, float threshold);
    float ChristmasLerp(float start, float end, float t);
    std::vector<glm::vec3> GenerateSagPoints(const glm::vec3& start, const glm::vec3& end, int numPoints, float sagAmount);
    std::vector<glm::vec3> GenerateCirclePoints(const glm::vec3& center, const glm::vec3& forward, float radius, int numPoints);

    // Raycasting
    CubeRayResult CastCubeRay(const glm::vec3& rayOrigin, const glm::vec3 rayDir, std::vector<Transform>& cubeTransforms, float maxDistance = 99999);
    glm::vec3 GetMouseRayDir(glm::mat4 projection, glm::mat4 view, int windowWidth, int windowHeight, int mouseX, int mouseY);
    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& spherePosition, float sphereRadius);

    // Mesh
    std::vector<Vertex> GenerateSphereVertices(float radius, int segments);
    std::vector<Vertex> GenerateRingVertices(float sphereRadius, float ringThickness, int ringSegments, int thicknessSegments);
    std::vector<Vertex> GenerateConeVertices(float radius, float height, int segments);
    std::vector<Vertex> GenerateCylinderVertices(float radius, float height, int subdivisions);
    std::vector<Vertex> GenerateCubeVertices();
    std::vector<uint32_t> GenerateRingIndices(int segments, int thicknessSegments);
    std::vector<uint32_t> GenerateSphereIndices(int segments); 
    std::vector<uint32_t> GenerateConeIndices(int segments);
    std::vector<uint32_t> GenerateCylinderIndices(int subdivisions);
    std::vector<uint32_t> GenerateCubeIndices();
    std::vector<uint32_t> GenerateSequentialIndices(int vertexCount);
    glm::vec3 ComputeFaceNormal(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
    glm::vec2 CalculateUV(const glm::vec3& vertexPosition, const glm::vec3& vertexNormal);
    void SetNormalsAndTangentsFromVertices(Vertex& vert0, Vertex& vert1, Vertex& vert2);

    // Text
    const char* ShortcutToString(Shortcut shortcut);
    std::string BoolToString(bool b);
    std::string Vec2ToString(glm::vec2 v);
    std::string Vec3ToString(glm::vec3 v);
    std::string Mat4ToString(glm::mat4 m);
    std::string Lowercase(std::string& str);
    std::string Uppercase(std::string& str);
    std::string FloatToString(float value, int precision = 3);
    std::string DoubleToString(double value, int precision = 3);



    const char* CopyConstChar(const char* text);
    bool StrCmp(const char* queryA, const char* queryB);

    // File
    int GetFileSize(const std::string& filepath);
    std::string GetFilename(const std::string& filepath);
    std::string GetFileName(const std::string& filepath);
    std::string RemoveFileExtension(const std::string& filename);
    std::string GetFullPath(const std::filesystem::directory_entry& entry);
    std::string GetFileName(const std::filesystem::directory_entry& entry);
    std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry);
    std::string GetFileExtension(const std::filesystem::directory_entry& entry);
    std::vector<FileInfo> IterateDirectory(const std::string& directory, std::vector<std::string> extensions = {});
    bool FileExists(const std::string_view name);
    bool RenameFile(const std::string& oldFilePath, const std::string& newFilePath);
    FileInfo GetFileInfoFromPath(const std::string& filepath);

    // Rendering
    void UpdateRenderItemAABB(RenderItem& renderItem);
    void UpdateRenderItemAABBFastA(RenderItem& renderItem);
    void UpdateRenderItemAABBFastB(RenderItem& renderItem);
    AABB ComputeWorldAABB(glm::vec3& localAabbMin, glm::vec3& localAabbMax, glm::mat4& modelMatrix);
    glm::mat4 GetLightSpaceMatrix(const glm::mat4& viewMatrix, glm::vec3 lightDir, const float viewportWidth, const float viewportHeight, const float fov, const float nearPlane, const float farPlane);
    std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
    std::vector<glm::mat4> GetLightProjectionViews(const glm::mat4& viewMatrix, glm::vec3 lightDir, std::vector<float>& shadowCascadeLevels, const float viewportWidth, const float viewportHeight, const float fov);

    // Easing
    float EaseIn(float t, float a);
    float EaseOut(float t, float a);
    float EaseInOut(float t, float a);
    float HermiteEaseInOut(float t);

    // JSON
    AdditionalMapData AdditionalMapDataFromJSON(const std::string& jsonString);
    CreateInfoCollection CreateInfoCollectionFromJSON(const std::string& jsonString);
    std::string AdditionalMapDataToJSON(AdditionalMapData& additionalMapData);
    std::string CreateInfoCollectionToJSON(CreateInfoCollection& createInfoCollection);

    // Animation
    //int FindAnimatedNodeIndex(float AnimationTime, const AnimatedNode* animatedNode);
    const AnimatedNode* FindAnimatedNode(Animation* animation, const char* NodeName);
    void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedScale(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const AnimatedNode* animatedNode);
    float SmoothStep(float t);
    float SmoothStepReverse(float t);
    float SteepSlowDownCurve(float t);
    float EaseOut(float t);
    glm::mat4 Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    glm::mat4 Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    glm::mat4 Mat4InitTranslationTransform(float x, float y, float z);
    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
    glm::mat4 aiMatrix3x3ToGlm(const aiMatrix3x3& from); 

    // Enum to string Conversions
    std::string BlendingModeToString(BlendingMode mode);
    std::string CameraViewToString(CameraView cameraView);
    std::string DebugRenderModeToString(DebugRenderMode mode);
    std::string EditorModeToString(EditorMode editorMode);
    std::string EditorSelectionModeToString(EditorSelectionMode mode);
    std::string EditorStateToString(EditorState state);
    std::string GenericObjectTypeToString(GenericObjectType houseType);
    std::string HousePlaneTypeToString(HousePlaneType type);
    std::string HouseTypeToString(HouseType houseType);
    std::string ImageDataTypeToString(ImageDataType imageDataType);
    std::string InventoryStateToString(InventoryState state);
    std::string LightTypeToString(LightType type);
    std::string ObjectTypeToString(ObjectType type);
    std::string OpenStateToString(OpenState mode);
    std::string PickUpTypeToString(PickUpType type);
    std::string PhysicsTypeToString(PhysicsType type);
    std::string PictureFrameTypeToString(PictureFrameType type);
    std::string SharkHuntingStateToString(SharkHuntingState state);
    std::string SharkMovementStateToString(SharkMovementState state);
    std::string TreeTypeToString(TreeType type);
    std::string TrimTypeToString(TrimType type);
    std::string ViewportModeToString(ShadingMode viewportMode);
    std::string WallTypeToString(WallType type);
    std::string WeaponActionToString(WeaponAction weaponAction);

    // String to Enum conversions
    BlendingMode StringToBlendingMode(const std::string& str);
    GenericObjectType StringToGenericObjectType(const std::string& str);
    HouseType StringToHouseType(const std::string& str);
    HousePlaneType StringToHousePlaneType(const std::string& str);
    LightType StringToLightType(const std::string& str);
    PickUpType StringToPickUpType(const std::string& str);
    PictureFrameType StringToPictureFrameType(const std::string& str);
    TreeType StringToTreeType(const std::string& str);
    TrimType StringToTrimType(const std::string& str);
    WallType StringToWallType(const std::string& str);

    // Int conversions
    ObjectType IntToEnum(int value);
    int32_t EnumToInt(ObjectType type);

    // Time
    double GetCurrentTime();
    std::string TimestampToString(uint64_t timestamp);

    // Debug Info
    void PrintDebugInfo(TextureData& textureData);
    std::string BytesToMBString(size_t bytes);

    // Templates
    template<typename In, typename MinIn, typename MaxIn, typename MinOut, typename MaxOut>
    inline float MapRange(In inValue, MinIn minInRange, MaxIn maxInRange, MinOut minOutRange, MaxOut maxOutRange) {
        static_assert(std::is_arithmetic<In>::value && std::is_arithmetic<MinIn>::value && std::is_arithmetic<MaxIn>::value && std::is_arithmetic<MinOut>::value && std::is_arithmetic<MaxOut>::value, "MapRange requires arithmetic types");
        float iv = static_cast<float>(inValue);
        float minI = static_cast<float>(minInRange);
        float maxI = static_cast<float>(maxInRange);
        float minO = static_cast<float>(minOutRange);
        float maxO = static_cast<float>(maxOutRange);
        float x = (iv - minI) / (maxI - minI);
        return minO + (maxO - minO) * x;
    }

}