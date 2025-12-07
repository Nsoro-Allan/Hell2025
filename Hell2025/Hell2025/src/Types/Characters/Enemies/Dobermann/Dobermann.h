#pragma once
#include "HellTypes.h"'
#include "CreateInfo.h"
#include "Types/Game/AnimatedGameObject.h"

struct Dobermann {
	void Init(DobermannCreateInfo createInfo);
	void Update(float deltaTime);

	void EnableRagdollRender();
	void DisableRagdollRender();
	void TakeDamage(uint32_t damage);

	AnimatedGameObject* GetAnimatedGameObject();
	glm::vec3 GetPosition();
	uint64_t GetRagdollV2Id() { return m_ragdollV2Id; }

private:
	uint64_t g_animatedGameObjectObjectId = 0;
	uint64_t m_objectId = 0;
	uint64_t m_ragdollV2Id = 0;
	bool m_renderRagdoll = false;
};