#pragma once
#include "HellTypes.h"'
#include "CreateInfo.h"
#include "Types/Game/AnimatedGameObject.h"
#include "Physics/Types/CharacterController.h"

enum struct DobermannState {
    LAY,
    GET_UP_FROM_LAY,
    WALK_TO_TARGET,
    SIT_FROM_LAY
};

struct Dobermann {
	void Init(DobermannCreateInfo createInfo);
	void Update(float deltaTime);

    void ResetToInitialState();
    void SetPosition(const glm::vec3& position);

	void EnableRagdollRender();
	void DisableRagdollRender();
	void TakeDamage(uint32_t damage);

    void DebugDraw();

    AnimatedGameObject* GetAnimatedGameObject(); 
    CharacterController* GetCharacterController();
    glm::vec3 GetPosition();

    const glm::vec3& GetForward() const             { return m_forward; }
	uint64_t GetRagdollV2Id()                       { return m_ragdollV2Id; }
    const DobermannState GetDobermannState() const  { return m_state; }

private:
    void CreateCharacterController(const glm::vec3& position);
    void UpdateAnimatedGameObjectRotation();

    DobermannCreateInfo m_createInfo;
	uint64_t g_animatedGameObjectObjectId = 0;
	uint64_t m_objectId = 0;
	uint64_t m_ragdollV2Id = 0;
    uint64_t m_characterControllerId = 0;
	float m_health = 0.0f;
    float m_initalHealth = 1.0f;
	bool m_renderRagdoll = false;
    DobermannState m_state = DobermannState::LAY;
    glm::vec3 m_target = glm::vec3(0.0f);
    std::vector<glm::vec3> m_path;

    glm::vec3 m_initalForward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_forward = glm::vec3(0.0f);
};