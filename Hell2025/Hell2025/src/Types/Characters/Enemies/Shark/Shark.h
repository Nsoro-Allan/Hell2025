#pragma once
#include "Types/Game/AnimatedGameObject.h"
#include "Physics/Types/Ragdoll.h"
#include <vector>

#define SHARK_SPINE_SEGMENT_COUNT 11
#define COLLISION_SPHERE_RADIUS 1
#define COLLISION_TEST_STEP_COUNT 40
#define SHARK_HEALTH_MAX 1000

struct Shark {
    void Init(glm::vec3 initialPosition);
    void Update(float deltaTime);
    void SetPosition(glm::vec3 position);
    void CleanUp();
    void DrawSpinePoints();
    void HuntPlayer(uint64_t playerId); 
    void GiveDamage(uint64_t playerId, int damageAmount);
    void Kill();
    void Respawn();
    void SetPositionToBeginningOfPath();
    void PlayAnimation(const std::string& animationName, float speed);
    void PlayAndLoopAnimation(const std::string& animationName, float speed);
    void SetMovementState(SharkMovementState state);
    void StraightenSpine(float deltaTime, float straightSpeed);
    
    std::string GetDebugInfoAsString();

    AnimatedGameObject* GetAnimatedGameObject();
    Ragdoll* GetRadoll();

    glm::vec3 m_spinePositions[SHARK_SPINE_SEGMENT_COUNT];
    std::string m_spineBoneNames[SHARK_SPINE_SEGMENT_COUNT];
    float m_spineSegmentLengths[SHARK_SPINE_SEGMENT_COUNT - 1];

    const bool IsDead() const           { return !m_alive; }
    const bool IsAlive() const          { return m_alive; }
    const uint64_t& GetObjectId() const { return m_objectId; };

private:
    void CalculateTargetFromPath();
    void CalculateForwardVectorFromTarget(float deltaTime);
    void CalculateForwardVectorFromArrowKeys(float deltaTime);
    void CalculateTargetFromPlayer();
    void MoveShark(float deltaTime);

    void UpdateHuntingLogic(float deltaTime);

    // Util
    int GetAnimationFrameNumber();
    float GetDistanceMouthToTarget3D();
    float GetDistanceToTarget2D();
    float GetTurningRadius() const;
    bool TargetIsOnLeft(glm::vec3 targetPosition);
    bool IsBehindEvadePoint(glm::vec3 position);
    glm::vec3 GetMouthPosition3D();
    glm::vec3 GetForwardVector();
    glm::vec3 GetTargetPosition2D();
    glm::vec3 GetHeadPosition2D();
    glm::vec3 GetMouthPosition2D();
    glm::vec3 GetCollisionLineEnd();
    glm::vec3 GetCollisionSphereFrontPosition();
    glm::vec3 GetSpinePosition(int index);
    glm::vec3 GetEvadePoint3D();
    glm::vec3 GetEvadePoint2D();

    uint64_t m_objectId = 0;
    uint64_t g_animatedGameObjectObjectId = 0; 
    uint64_t m_huntedPlayerId = 0;
    int m_health = SHARK_HEALTH_MAX;
    int m_logicSubStepCount = 8;
    float m_swimSpeed = 8.0f;
    float m_rotationSpeed = 2.5f;
    glm::vec3 m_forward = glm::vec3(0);
    glm::vec3 m_right = glm::vec3(0);
    glm::vec3 m_lastKnownTargetPosition = glm::vec3(0);
    glm::vec3 m_left = glm::vec3(0); 
    bool m_hasBitPlayer = false;
    bool m_alive = false; 
    bool m_playerSafe = false;

    SharkHuntingState m_huntingState = SharkHuntingState::UNDEFINED;
    SharkMovementState m_movementState = SharkMovementState::FOLLOWING_PATH;

    int32_t m_nextPathPointIndex = 0;
    glm::vec3 m_targetPosition = glm::vec3(0);
    std::vector<glm::vec3> m_path;

    //

   // RigidComponent* m_rigidComponents[SHARK_SPINE_SEGMENT_COUNT];

  // void UpdateHuntingLogic(float deltaTime);
  // void Reset();
  //
  // void MoveShark(float deltaTime);
  //
  // void UpdateMovementFollowingPath2(float deltaTime);
  // void CalculateTargetFromPlayer();
  // void CalculateTargetFromPath();
  //
  // float GetDotToTarget2D();
  // float GetDotMouthDirectionToTarget3D();
  // 
  // void Respawn();
  // void Kill();
  // void CleanUp();
  // void SetPosition(glm::vec3 position);
  // void PlayAnimation(const std::string& animationName, float speed = 1.0f);
  // void PlayAndLoopAnimation(const std::string& animationName, float speed = 1.0f);
  // int GetAnimationFrameNumber();
  //
  // 
  //
  // bool IsDead();
  // bool IsAlive();
  // bool LeftTurningArcIntersectsLine(glm::vec3 p1, glm::vec3 p2);
  // bool RightTurningArcIntersectsLine(glm::vec3 p1, glm::vec3 p2);
  // bool ShouldTurnLeftToAvoidLine(glm::vec3 lineStart, glm::vec3 lineEnd);
  // bool TargetIsOnLeft(glm::vec3 targetPosition);
  // //bool TargetIsStraightAhead();
  //
  // 
  // void CalculateForwardVectorFromArrowKeys(float deltaTime);
  //
  // float GetDistanceToTarget2D();
  // float GetDistanceMouthToTarget3D();
  // glm::vec3 GetMouthPosition3D();
  // glm::vec3 GetMouthForwardVector();
  // glm::vec3 GetTargetPosition2D();
  // glm::vec3 GetTargetDirection2D();
  //
  // glm::vec3 GetEvadePoint3D();
  // glm::vec3 GetEvadePoint2D();
  //
  // float m_currentHeight = -2.0f;
  // bool m_drawDebug = false;
  //
  // // Debug
  // std::vector<Vertex> GetDebugPointVertices();
  // std::vector<Vertex> GetDebugLineVertices();
  //
  // PxRigidDynamic* m_headPxRigidDynamic = nullptr;
  //
  // Ragdoll* GetRadoll();
  // AnimatedGameObject* GetAnimatedGameObject();
  // std::string GetDebugText();       
  // glm::vec3 GetForwardVector();
  // glm::vec3 GetRightVector();
  // glm::vec3 GetHeadPosition2D();
  // glm::vec3 GetSpinePosition(int index);
  // glm::vec3 GetCollisionSphereFrontPosition();
  // glm::vec3 GetCollisionLineEnd();
  // std::vector<glm::vec3> GetNextMovementStepsLeft(int stepCount);
  // std::vector<glm::vec3> GetNextMovementStepsRight(int stepCount);
  // float GetTurningRadius() const;
  // void CheckDebugKeyPresses();
  //
  // void SetPositionToBeginningOfPath();
  //
  // void HuntClosestPlayerInLineOfSight();
  //
  //
  // 
  //
  // 
  // 
  //
  //
  //
  //
  // bool m_init = false;
  // float m_rotation = 0;
  // int m_animatedGameObjectIndex = -1;
  //
  // glm::vec3 m_spinePositions[SHARK_SPINE_SEGMENT_COUNT];
  // std::string m_spineBoneNames[SHARK_SPINE_SEGMENT_COUNT];
  // float m_spineSegmentLengths[SHARK_SPINE_SEGMENT_COUNT - 1];
  // RigidComponent* m_rigidComponents[SHARK_SPINE_SEGMENT_COUNT];
  //
  // std::vector<PxShape*> m_collisionPxShapes;
  //
  // 
  //
  // 
  // 
  //
  // 
  // 
  // 
  //
  // 
  // 
  // SharkMovementDirection m_movementDirection;
  //
  //
  // 
  //
  // bool m_drawPath = false;
  //
  // 
  //
  // 
  //
  // static std::string SharkMovementStateToString(SharkMovementState state);
  // static std::string SharkHuntingStateToString(SharkHuntingState state);
  //
  // bool IsBehindEvadePoint(glm::vec3 position);
  // glm::vec3 GetMouthPosition2D();
  // void StraightenSpine(float deltaTime, float straightSpeed);
  //
  // private:
  //     glm::vec3 m_headPositionLastFrame = glm::vec3(0);
  //     glm::vec3 m_mouthPositionLastFrame = glm::vec3(0);
  //     glm::vec3 m_evadePointPositionLastFrame = glm::vec3(0);
  //
  //     int m_logicSubStepCount = 8;
  //
};