#include "Shark.h"
#include "Core/Game.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Math/LineMath.hpp"
#include "World/World.h"
#include "UniqueId.h"

std::vector<glm::vec3> GetCirclePoints(const glm::vec3& center, int segments, float radius) {
    std::vector<glm::vec3> pts;
    pts.reserve(segments);
    const float PI = 3.14159265358979323846f;
    float dTheta = 2.0f * PI / segments;
    for (int i = 0; i < segments; ++i) {
        float theta = i * dTheta;
        pts.emplace_back(
            center.x + std::cos(theta) * radius,
            center.y,
            center.z + std::sin(theta) * radius
        );
    }
    return pts;
}

void Shark::Init(glm::vec3 initialPosition) {
    m_objectId = UniqueID::GetNextGlobal();


    g_animatedGameObjectObjectId = World::CreateAnimatedGameObject();

    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    animatedGameObject->SetSkinnedModel("Shark");
    animatedGameObject->SetName("GreatestGreatWhiteShark");
    animatedGameObject->SetAllMeshMaterials("Shark");
    animatedGameObject->PlayAndLoopAnimation("MainLayer", "Shark_Swim", 1.0f);

    animatedGameObject->SetScale(0.01);
    animatedGameObject->SetPosition(glm::vec3(0, 0, 0));
    animatedGameObject->SetRagdoll("Shark", 1500.0f);

    SkinnedModel* skinnedModel = animatedGameObject->GetSkinnedModel();
    std::vector<Node>& nodes = skinnedModel->m_nodes;
    std::map<std::string, unsigned int>& boneMapping = skinnedModel->m_boneMapping;


    // Extract spine positions
    int nodeCount = nodes.size();
    std::vector<glm::mat4> skinnedTransformations(nodeCount);

    for (int i = 0; i < skinnedModel->m_nodes.size(); i++) {
        glm::mat4 nodeTransformation = glm::mat4(1);
        std::string& nodeName = skinnedModel->m_nodes[i].name;
        nodeTransformation = skinnedModel->m_nodes[i].inverseBindTransform;
        unsigned int parentIndex = skinnedModel->m_nodes[i].parentIndex;
        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : skinnedTransformations[parentIndex];
        glm::mat4 GlobalTransformation = ParentTransformation * nodeTransformation;
        skinnedTransformations[i] = AnimatedTransform(GlobalTransformation).to_mat4();

        float scale = 0.01f;
        glm::vec3 position = skinnedTransformations[i][3] * scale;

        if (nodeName == "BN_Head_00") {
            m_spinePositions[0] = position;
            m_spineBoneNames[0] = nodeName;
        }
        else if (nodeName == "BN_Neck_01") {
            m_spinePositions[1] = position;
            m_spineBoneNames[1] = nodeName;
        }
        else if (nodeName == "BN_Neck_00") {
            m_spinePositions[2] = position;
            m_spineBoneNames[2] = nodeName;
        }
        else if (nodeName == "Spine_00") {
            m_spinePositions[3] = position;
            m_spineBoneNames[3] = nodeName;
        }
        else if (nodeName == "BN_Spine_01") {
            m_spinePositions[4] = position;
            m_spineBoneNames[4] = nodeName;
        }
        else if (nodeName == "BN_Spine_02") {
            m_spinePositions[5] = position;
            m_spineBoneNames[5] = nodeName;
        }
        else if (nodeName == "BN_Spine_03") {
            m_spinePositions[6] = position;
            m_spineBoneNames[6] = nodeName;
        }
        else if (nodeName == "BN_Spine_04") {
            m_spinePositions[7] = position;
            m_spineBoneNames[7] = nodeName;
        }
        else if (nodeName == "BN_Spine_05") {
            m_spinePositions[8] = position;
            m_spineBoneNames[8] = nodeName;
        }
        else if (nodeName == "BN_Spine_06") {
            m_spinePositions[9] = position;
            m_spineBoneNames[9] = nodeName;
        }
        else if (nodeName == "BN_Spine_07") {
            m_spinePositions[10] = position;
            m_spineBoneNames[10] = nodeName;
        }
    }

    // Compute distances between spine segments
    m_spinePositions[0].y = 0.0f;

    // Reset height
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        m_spinePositions[i].y = m_spinePositions[0].y;
    }
    // Print names
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        //std::cout << i << ": " << m_spineBoneNames[i] << "\n";
    }
    // Calculate distances
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT - 1; i++) {
        m_spineSegmentLengths[i] = glm::distance(m_spinePositions[i], m_spinePositions[i + 1]);
    }

    m_forward = glm::normalize(m_spinePositions[0] - m_spinePositions[1]);

    SetPosition(initialPosition);

    Ragdoll* ragdoll = Physics::GetRagdollById(animatedGameObject->GetRagdollId());
    ragdoll->SetPhysicsData(m_objectId, ObjectType::SHARK);

    // Hack in a path
    glm::vec3 center(10.0f, 30.0f, 58.0f);
    float radius = 10;
    int segments = 9;
    m_path = GetCirclePoints(center, segments, radius);

    m_alive = true;

}

void Shark::Update(float deltaTime) {
    if (false) {
        for (glm::vec3 point : m_path) {
            Renderer::DrawPoint(point, RED);
        }
    }

    // Did the player enter the water again while the shark is still angry from being like shot before
    if (m_movementState == SharkMovementState::FOLLOWING_PATH_ANGRY) {
        Player* player = Game::GetPlayerByPlayerId(m_huntedPlayerId);
        if (player && player->FeetBelowWater()) {
            // TO DO: Only activate hunt state again if she shark has line of sight to the player
            m_movementState = SharkMovementState::HUNT_PLAYER;
            m_huntingState = SharkHuntingState::CHARGE_PLAYER;
        }
    }

    if (Input::KeyPressed(HELL_KEY_SLASH)) {
        if (m_movementState == SharkMovementState::FOLLOWING_PATH) {
            m_movementState = SharkMovementState::ARROW_KEYS;
        }
        else {
            m_movementState = SharkMovementState::FOLLOWING_PATH;
            m_nextPathPointIndex = 0;
        }
    }

    // Put these somewhere better!
    m_right = glm::cross(m_forward, glm::vec3(0, 1, 0));
    m_left = -m_right;

    glm::mat4 rootTranslationMatrix = glm::translate(glm::mat4(1), m_spinePositions[3]);

    // Root to the end of the spine
    float rot0 = Util::YRotationBetweenTwoPoints(m_spinePositions[3], m_spinePositions[2]) + HELL_PI * 0.5f;
    float rot1 = Util::YRotationBetweenTwoPoints(m_spinePositions[4], m_spinePositions[3]) + HELL_PI * 0.5f;
    float rot2 = Util::YRotationBetweenTwoPoints(m_spinePositions[5], m_spinePositions[4]) + HELL_PI * 0.5f;
    float rot3 = Util::YRotationBetweenTwoPoints(m_spinePositions[6], m_spinePositions[5]) + HELL_PI * 0.5f;
    float rot4 = Util::YRotationBetweenTwoPoints(m_spinePositions[7], m_spinePositions[6]) + HELL_PI * 0.5f;
    float rot5 = Util::YRotationBetweenTwoPoints(m_spinePositions[8], m_spinePositions[7]) + HELL_PI * 0.5f;
    float rot6 = Util::YRotationBetweenTwoPoints(m_spinePositions[9], m_spinePositions[8]) + HELL_PI * 0.5f;
    float rot7 = Util::YRotationBetweenTwoPoints(m_spinePositions[10], m_spinePositions[9]) + HELL_PI * 0.5f;

    // From the neck to the head
    float rot8 = Util::YRotationBetweenTwoPoints(m_spinePositions[3], m_spinePositions[2]) + HELL_PI * 0.5f;
    float rot9 = Util::YRotationBetweenTwoPoints(m_spinePositions[2], m_spinePositions[1]) + HELL_PI * 0.5f;
    float rot10 = Util::YRotationBetweenTwoPoints(m_spinePositions[1], m_spinePositions[0]) + HELL_PI * 0.5f;

    // Update animation
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        glm::vec3 rootPosition = m_spinePositions[3];
        float yRotation = rot0;

        animatedGameObject->SetPosition(rootPosition);
        animatedGameObject->SetRotationY(yRotation);

        animatedGameObject->SetAdditiveTransform("BN_Spine_01", glm::rotate(glm::mat4(1.0f), rot1 - rot0, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_02", glm::rotate(glm::mat4(1.0f), rot2 - rot1, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_03", glm::rotate(glm::mat4(1.0f), rot3 - rot2, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_04", glm::rotate(glm::mat4(1.0f), rot4 - rot3, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_05", glm::rotate(glm::mat4(1.0f), rot5 - rot4, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_06", glm::rotate(glm::mat4(1.0f), rot6 - rot5, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Spine_07", glm::rotate(glm::mat4(1.0f), rot7 - rot6, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Neck_00", glm::rotate(glm::mat4(1.0f), rot8 - rot1, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Neck_01", glm::rotate(glm::mat4(1.0f), rot9 - rot8, glm::vec3(0, 1, 0)));
        animatedGameObject->SetAdditiveTransform("BN_Head_00", glm::rotate(glm::mat4(1.0f), rot10 - rot9, glm::vec3(0, 1, 0)));

        animatedGameObject->Update(deltaTime);
        animatedGameObject->UpdateRenderItems();
    }

    // Arrow key movement
    if (IsAlive()) {
        if (m_movementState == SharkMovementState::ARROW_KEYS) {
            if (Input::KeyDown(HELL_KEY_UP)) {
                CalculateForwardVectorFromArrowKeys(deltaTime);

                for (int i = 0; i < m_logicSubStepCount; i++) {
                    MoveShark(deltaTime);
                }
            }
        }
        // Path following
        else if (m_movementState == SharkMovementState::FOLLOWING_PATH ||
                 m_movementState == SharkMovementState::FOLLOWING_PATH_ANGRY) {
            CalculateForwardVectorFromTarget(deltaTime);
            CalculateTargetFromPath();

            for (int i = 0; i < m_logicSubStepCount; i++) {
                MoveShark(deltaTime);
            }
        }
        else if (m_movementState == SharkMovementState::HUNT_PLAYER) {
            if (m_huntingState == SharkHuntingState::CHARGE_PLAYER ||
                m_huntingState == SharkHuntingState::BITING_PLAYER && GetAnimationFrameNumber() > 17 ||
                m_huntingState == SharkHuntingState::BITING_PLAYER && GetAnimationFrameNumber() > 7 && !IsBehindEvadePoint(m_targetPosition)) {
                CalculateTargetFromPlayer();
                CalculateForwardVectorFromTarget(deltaTime);
            }
            for (int i = 0; i < m_logicSubStepCount; i++) {
                UpdateHuntingLogic(deltaTime);
                MoveShark(deltaTime);
            }
        }
    }

    // Is it alive 
    if (m_health > 0) {
        m_alive = true;
    }
    // Kill if health zero
    if (IsAlive() && m_health <= 0) {
        Kill();
        //Game::g_sharkDeaths++;
        //std::ofstream out("SharkDeaths.txt");
        //out << Game::g_sharkDeaths;
        //out.close();
    }
    m_health = std::max(m_health, 0);

    if (IsDead()) {
        StraightenSpine(deltaTime, 0.25f);
        if (animatedGameObject->GetAnimationFrameNumber("MainLayer") > 100) {
            animatedGameObject->PauseAllAnimationLayers();
        }
    }
}

void Shark::CleanUp() {
    World::RemoveObject(g_animatedGameObjectObjectId);
}

void Shark::StraightenSpine(float deltaTime, float straightSpeed) {
    // Fake moving the head forward
    glm::vec3 originalHeadPosition = m_spinePositions[0];
    glm::vec3 fakeForwardMovement = GetForwardVector() * m_swimSpeed * deltaTime * straightSpeed;
    m_spinePositions[0] += fakeForwardMovement;
    // Straighten the rest of the spine using movement logic
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; ++i) {
        glm::vec3 direction = m_spinePositions[i - 1] - m_spinePositions[i];
        float currentDistance = glm::length(direction);
        if (currentDistance > m_spineSegmentLengths[i - 1]) {
            glm::vec3 correction = glm::normalize(direction) * (currentDistance - m_spineSegmentLengths[i - 1]);
            m_spinePositions[i] += correction;
        }
    }
    // Move the head back to its original position
    glm::vec3 correctionVector = m_spinePositions[0] - originalHeadPosition;
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT; ++i) {
        m_spinePositions[i] -= correctionVector;
    }
}

void Shark::GiveDamage(uint64_t playerId, int damageAmount) {
    m_health -= damageAmount;
    std::cout << "Shark health: " << m_health << "\n";
    //if (m_huntedPlayerId == 0) {
    HuntPlayer(playerId);
    //}
}

void Shark::HuntPlayer(uint64_t playerId) {
    m_huntedPlayerId = playerId;
    m_movementState = SharkMovementState::HUNT_PLAYER;
    m_huntingState = SharkHuntingState::CHARGE_PLAYER;
}

void Shark::SetPositionToBeginningOfPath() {
    if (m_path.empty()) {
        SetPosition(glm::vec3(0, 30.0f, 0));
    }
    else {
        glm::vec3 position = m_path[0];
        SetPosition(position);
        m_nextPathPointIndex = 1;
    }
}

void Shark::Respawn() {
    SetPositionToBeginningOfPath();
    m_movementState = SharkMovementState::FOLLOWING_PATH;
    m_health = SHARK_HEALTH_MAX;
    m_alive = true;
    m_hasBitPlayer = false;
    PlayAndLoopAnimation("Shark_Swim", 1.0f);
}

void Shark::Kill() {
    m_health = 0;
    m_alive = false;
    Audio::PlayAudio("Shark_Death.wav", 1.0f);
    PlayAndLoopAnimation("Shark_Die", 1.0f);
}

void Shark::SetMovementState(SharkMovementState movementState) {
    m_movementState = movementState;
}

void Shark::SetPosition(glm::vec3 position) {
    m_spinePositions[0] = position;
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        m_spinePositions[i].x = m_spinePositions[0].x;
        m_spinePositions[i].y = m_spinePositions[0].y;
        m_spinePositions[i].z = m_spinePositions[i - 1].z - m_spineSegmentLengths[i - 1];
        //m_rotation = 0;           TRIPLE CHECK YOU DON'T NEED THIS !!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    m_forward = glm::vec3(0, 0, 1);
}

void Shark::CalculateTargetFromPlayer() {
    if (m_huntedPlayerId != 0) {
        Player* player = Game::GetPlayerByPlayerId(m_huntedPlayerId);
        m_targetPosition = player->GetCameraPosition() - glm::vec3(0.0, 0.1f, 0.0f);
        //std::cout << Util::Vec3ToString(m_targetPosition) << "\n";

        static bool attackLeft = true;
        if (GetDistanceToTarget2D() < 6.5f) {
            if (attackLeft) {
                m_targetPosition += m_left * 0.975f;
                //std::cout << "attacking left\n";
            }
            else {
                m_targetPosition += m_right * 0.975f;
                //std::cout << "attacking right\n";
            }
        }
        else {
            attackLeft = !attackLeft;
            //std::cout << "attack left: " << attackLeft << "\n";
        }
        m_lastKnownTargetPosition = m_targetPosition;
    }
}

void Shark::CalculateTargetFromPath() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (m_nextPathPointIndex >= m_path.size()) {
        m_nextPathPointIndex = 0;
    }
    glm::vec3 nextPathPoint = m_path[m_nextPathPointIndex];
    // Are you actually at the next point?
    float nextPointThreshold = 1.0f;
    if (GetDistanceToTarget2D() < nextPointThreshold) {
        m_nextPathPointIndex++;
        nextPathPoint = m_path[m_nextPathPointIndex];
    }
    glm::vec3 dirToNextPathPoint = glm::normalize(GetHeadPosition2D() - nextPathPoint);
    m_targetPosition = nextPathPoint;

    //std::cout << "m_nextPathPointIndex: " << m_nextPathPointIndex << "\n";
    //std::cout << "m_path.size(): " << m_path.size() << "\n";
}

void Shark::MoveShark(float deltaTime) {

    //m_mouthPositionLastFrame = GetMouthPosition2D();
    //m_headPositionLastFrame = GetHeadPosition2D();
    //m_evadePointPositionLastFrame = GetEvadePoint2D();
    // Move head
    m_spinePositions[0] += m_forward * m_swimSpeed * deltaTime / (float)m_logicSubStepCount;
    // Move spine segments
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; ++i) {
        glm::vec3 direction = glm::normalize(m_spinePositions[i - 1] - m_spinePositions[i]);
        m_spinePositions[i] = m_spinePositions[i - 1] - direction * m_spineSegmentLengths[i - 1];
    }
}

void Shark::CalculateForwardVectorFromTarget(float deltaTime) {
    // Calculate angular difference from forward to target
    glm::vec3 directionToTarget = glm::normalize(GetTargetPosition2D() - GetHeadPosition2D());
    float dotProduct = glm::clamp(glm::dot(m_forward, directionToTarget), -1.0f, 1.0f);
    float angleDifference = glm::degrees(std::acos(dotProduct));
    if (m_forward.x * directionToTarget.z - m_forward.z * directionToTarget.x < 0.0f) {
        angleDifference = -angleDifference;
    }
    // Clamp it to a max of 4.5 degrees rotation
    float maxRotation = 4.5f;
    angleDifference = glm::clamp(angleDifference, -maxRotation, maxRotation);
    // Calculate new forward vector based on that angle
    if (TargetIsOnLeft(m_targetPosition)) {
        float blendFactor = glm::clamp(glm::abs(-angleDifference) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_left, blendFactor));
    }
    else {
        float blendFactor = glm::clamp(glm::abs(angleDifference) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_right, blendFactor));
    }
}

void Shark::CalculateForwardVectorFromArrowKeys(float deltaTime) {
    float maxRotation = 5.0f;
    if (Input::KeyDown(HELL_KEY_LEFT)) {
        float blendFactor = glm::clamp(glm::abs(-maxRotation) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_left, blendFactor));
        std::cout << "PRESSED LEFT\n";
    }
    if (Input::KeyDown(HELL_KEY_RIGHT)) {
        float blendFactor = glm::clamp(glm::abs(maxRotation) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_right, blendFactor));
        std::cout << "PRESSED RIGHT\n";
    }
}

glm::vec3 Shark::GetForwardVector() {
    return m_forward;
}

glm::vec3 Shark::GetTargetPosition2D() {
    return m_targetPosition * glm::vec3(1.0f, 0.0f, 1.0f);
}

glm::vec3 Shark::GetHeadPosition2D() {
    return m_spinePositions[0] * glm::vec3(1.0f, 0.0f, 1.0f);
}

glm::vec3 Shark::GetCollisionLineEnd() {
    return GetCollisionSphereFrontPosition() + (GetForwardVector() * GetTurningRadius());
}

glm::vec3 Shark::GetCollisionSphereFrontPosition() {
    return GetHeadPosition2D() + GetForwardVector() * glm::vec3(COLLISION_SPHERE_RADIUS);
}

float Shark::GetTurningRadius() const {
    float turningRadius = m_swimSpeed / m_rotationSpeed;
    return turningRadius;
}

bool Shark::TargetIsOnLeft(glm::vec3 targetPosition) {
    glm::vec3 lineStart = GetHeadPosition2D();
    glm::vec3 lineEnd = GetCollisionLineEnd();
    glm::vec3 lineNormal = LineMath::GetLineNormal(lineStart, lineEnd);
    glm::vec3 midPoint = LineMath::GetLineMidPoint(lineStart, lineEnd);
    return LineMath::IsPointOnOtherSideOfLine(lineStart, lineEnd, lineNormal, targetPosition);
}

float Shark::GetDistanceToTarget2D() {
    return glm::distance(GetHeadPosition2D() * glm::vec3(1, 0, 1), m_targetPosition * glm::vec3(1, 0, 1));
}

/*
void Shark::CalculateForwardVectorFromTarget(float deltaTime) {
    // Calculate angular difference from forward to target
    glm::vec3 directionToTarget = glm::normalize(GetTargetPosition2D() - GetHeadPosition2D());
    float dotProduct = glm::clamp(glm::dot(m_forward, directionToTarget), -1.0f, 1.0f);
    float angleDifference = glm::degrees(std::acos(dotProduct));
    if (m_forward.x * directionToTarget.z - m_forward.z * directionToTarget.x < 0.0f) {
        angleDifference = -angleDifference;
    }
    // Clamp it to a max of 4.5 degrees rotation
    float maxRotation = 4.5f;
    angleDifference = glm::clamp(angleDifference, -maxRotation, maxRotation);
    // Calculate new forward vector based on that angle
    if (TargetIsOnLeft(m_targetPosition)) {
        float blendFactor = glm::clamp(glm::abs(-angleDifference) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_left, blendFactor));
    }
    else {
        float blendFactor = glm::clamp(glm::abs(angleDifference) / 90.0f, 0.0f, 1.0f);
        m_forward = glm::normalize(glm::mix(m_forward, m_right, blendFactor));
    }
}

bool Shark::TargetIsOnLeft(glm::vec3 targetPosition) {
    glm::vec3 lineStart = GetHeadPosition2D();
    glm::vec3 lineEnd = GetCollisionLineEnd();
    glm::vec3 lineNormal = LineMath::GetLineNormal(lineStart, lineEnd);
    glm::vec3 midPoint = LineMath::GetLineMidPoint(lineStart, lineEnd);
    return LineMath::IsPointOnOtherSideOfLine(lineStart, lineEnd, lineNormal, targetPosition);
}
*/

AnimatedGameObject* Shark::GetAnimatedGameObject() {
   return World::GetAnimatedGameObjectByObjectId(g_animatedGameObjectObjectId);
}

void Shark::DrawSpinePoints() {
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; ++i) {
        Renderer::DrawPoint(m_spinePositions[i], RED);
    }
    for (int i = 0; i < 1; ++i) {
        Renderer::DrawPoint(m_spinePositions[i], WHITE);
    }
}

Ragdoll* Shark::GetRadoll() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (animatedGameObject) {
        return Physics::GetRagdollById(animatedGameObject->GetRagdollId());
    }
    else {
        return nullptr;
    }
}

/*

#include "Shark.h"
#include "SharkPathManager.h"
#include "../Game/Game.h"
#include "../Game/Scene.h"
#include "../Game/Water.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"
#include "../Input/Input.h"
#include "../Math/LineMath.hpp"

void Shark::Init() {
    glm::vec3 initialPosition = glm::vec3(7.0f, -0.1f, -15.7);

    m_animatedGameObjectIndex = Scene::CreateAnimatedGameObject();
    AnimatedGameObject* animatedGameObject = Scene::GetAnimatedGameObjectByIndex(m_animatedGameObjectIndex);
    animatedGameObject->SetFlag(AnimatedGameObject::Flag::NONE);
    animatedGameObject->SetPlayerIndex(1);
    animatedGameObject->SetSkinnedModel("SharkSkinned");
    animatedGameObject->SetName("Shark");
    animatedGameObject->SetAnimationModeToBindPose();
    animatedGameObject->SetAllMeshMaterials("Shark");
    animatedGameObject->PlayAndLoopAnimation("Shark_Swim", 1.0f);

    // Create ragdoll
    PxU32 raycastFlag = RaycastGroup::RAYCAST_ENABLED;
    PxU32 collsionGroupFlag = CollisionGroup::SHARK;
    PxU32 collidesWithGroupFlag = CollisionGroup::ENVIROMENT_OBSTACLE | CollisionGroup::GENERIC_BOUNCEABLE | CollisionGroup::RAGDOLL | CollisionGroup::PLAYER;
    animatedGameObject->LoadRagdoll("Shark.rag", raycastFlag, collsionGroupFlag, collidesWithGroupFlag);
    m_init = true;

    // Find head position
    SkinnedModel* skinnedModel = animatedGameObject->_skinnedModel;
    std::vector<Joint>& joints = skinnedModel->m_joints;
    std::map<std::string, unsigned int>& boneMapping = skinnedModel->m_BoneMapping;
    std::vector<BoneInfo> boneInfo = skinnedModel->m_BoneInfo;

    for (int i = 0; i < joints.size(); i++) {
        const char* nodeName = joints[i].m_name;
        glm::mat4 NodeTransformation = joints[i].m_inverseBindTransform;
        unsigned int parentIndex = joints[i].m_parentIndex;
        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : joints[parentIndex].m_currentFinalTransform;
        glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;
        joints[i].m_currentFinalTransform = GlobalTransformation;
        if (boneMapping.find(nodeName) != boneMapping.end()) {
            unsigned int BoneIndex = boneMapping[nodeName];
            boneInfo[BoneIndex].FinalTransformation = GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
            boneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;

            // No idea why this scale is required
            float scale = 0.01f;
            glm::vec3 position = Util::GetTranslationFromMatrix(GlobalTransformation) * scale;
            if (Util::StrCmp(nodeName, "BN_Head_00")) {
                m_spinePositions[0] = position;
                m_spineBoneNames[0] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Neck_01")) {
                m_spinePositions[1] = position;
                m_spineBoneNames[1] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Neck_00")) {
                m_spinePositions[2] = position;
                m_spineBoneNames[2] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "Spine_00")) {
                m_spinePositions[3] = position;
                m_spineBoneNames[3] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_01")) {
                m_spinePositions[4] = position;
                m_spineBoneNames[4] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_02")) {
                m_spinePositions[5] = position;
                m_spineBoneNames[5] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_03")) {
                m_spinePositions[6] = position;
                m_spineBoneNames[6] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_04")) {
                m_spinePositions[7] = position;
                m_spineBoneNames[7] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_05")) {
                m_spinePositions[8] = position;
                m_spineBoneNames[8] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_06")) {
                m_spinePositions[9] = position;
                m_spineBoneNames[9] = nodeName;
            }
            else if (Util::StrCmp(nodeName, "BN_Spine_07")) {
                m_spinePositions[10] = position;
                m_spineBoneNames[10] = nodeName;
            }
        }
    }
    m_spinePositions[0].y = 0.0f;

    // Reset height
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        m_spinePositions[i].y = m_spinePositions[0].y;
    }
    // Print names
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        //std::cout << i << ": " << m_spineBoneNames[i] << "\n";
    }
    // Calculate distances
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT - 1; i++) {
        m_spineSegmentLengths[i] = glm::distance(m_spinePositions[i], m_spinePositions[i+1]);
    }

    // Store PxRigidBody pointers
    Ragdoll* ragdoll = GetRadoll();
    for (int i = 0; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        for (int j = 0; j < ragdoll->m_rigidComponents.size(); j++) {
            RigidComponent& rigidComponent = ragdoll->m_rigidComponents[j];        
            if (rigidComponent.correspondingJointName == m_spineBoneNames[i]) {
                m_rigidComponents[i] = &rigidComponent;
                //std::cout << i << ": " << m_spineBoneNames[i] << " " << rigidComponent.name << "\n";
            }
        }
    }

    if (ragdoll) {
        for (RigidComponent& rigidComponent : ragdoll->m_rigidComponents) {
            if (rigidComponent.name == "rMarker_BN_Head_00") {
                m_headPxRigidDynamic = rigidComponent.pxRigidBody;
                break;
            }
        }
    }
    Reset();
}

void Shark::SetPosition(glm::vec3 position) {
    m_spinePositions[0] = position;
    for (int i = 1; i < SHARK_SPINE_SEGMENT_COUNT; i++) {
        m_spinePositions[i].x = m_spinePositions[0].x;
        m_spinePositions[i].y = m_spinePositions[0].y;
        m_spinePositions[i].z = m_spinePositions[i - 1].z - m_spineSegmentLengths[i - 1];
        m_rotation = 0;
    }
    m_forward = glm::vec3(0, 0, 1);
}



void Shark::CleanUp() {
    // to do: move this to ragdoll class, and also destroy the PxShape
    AnimatedGameObject* animatedGameObject = Scene::GetAnimatedGameObjectByIndex(m_animatedGameObjectIndex);
    if (animatedGameObject) {
        Ragdoll& ragdoll = animatedGameObject->m_ragdoll;
        for (RigidComponent& rigidComponent : ragdoll.m_rigidComponents) {
            Physics::Destroy(rigidComponent.pxRigidBody);
        }
        ragdoll.m_rigidComponents.clear();
    }
    m_animatedGameObjectIndex = -1;
}




// Helper functions




float CalculateAngle(const glm::vec3& from, const glm::vec3& to) {
    return atan2(to.x - from.x, to.z - from.z);

}

float NormalizeAngle(float angle) {
    angle = fmod(angle + HELL_PI, 2 * HELL_PI);
    if (angle < 0) angle += 2 * HELL_PI;
    return angle - HELL_PI;
}

void RotateYTowardsTarget(glm::vec3 objectPosition, float& objectYRotation, const glm::vec3& targetPosition, float rotationSpeed) {
    float desiredAngle = CalculateAngle(objectPosition, targetPosition);
    float angleDifference = NormalizeAngle(desiredAngle - objectYRotation);
    std::cout << "angleDifference: " << angleDifference << "\n";
    if (fabs(angleDifference) < rotationSpeed) {
        objectYRotation = desiredAngle;
    }
    else {
        if (angleDifference > 0) {
            objectYRotation += rotationSpeed;
        }
        else {
            objectYRotation -= rotationSpeed;
        }
    }
    objectYRotation = fmod(objectYRotation, 2 * HELL_PI);
    if (objectYRotation < 0) {
        objectYRotation += 2 * HELL_PI;
    }
}

float Shark::GetTurningRadius() const {
    float turningRadius = m_swimSpeed / m_rotationSpeed;
    return turningRadius;
}





void Shark::Reset() {
    SetPositionToBeginningOfPath();
    m_movementState = SharkMovementState::FOLLOWING_PATH;
    m_huntingState = SharkHuntingState::UNDEFINED;
    m_health = SHARK_HEALTH_MAX;
    m_forward = glm::vec3(0, 0, 1.0f);
    m_huntedPlayerIndex = -1;
    m_nextPathPointIndex = 1;
    PlayAndLoopAnimation("Shark_Swim", 1.0f);
}
*/