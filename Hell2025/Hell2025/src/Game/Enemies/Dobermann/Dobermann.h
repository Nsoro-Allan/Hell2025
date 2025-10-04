#pragma once
#include "HellTypes.h"'
#include "CreateInfo.h"
#include "Types/Game/AnimatedGameObject.h"

struct Dobermann {
    void Init(DobermannCreateInfo createInfo);
    void Update(float deltaTime);

    AnimatedGameObject* GetAnimatedGameObject();
    glm::vec3 GetPosition();

private:
    uint64_t g_animatedGameObjectObjectId = 0;
    uint64_t m_objectId= 0;
    uint64_t m_ragdollId = 0;
};