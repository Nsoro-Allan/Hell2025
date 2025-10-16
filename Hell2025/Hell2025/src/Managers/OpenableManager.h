#pragma once
#include "CreateInfo.h"
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Handlers/Openable.h"

namespace OpenableManager {
    uint32_t CreateOpenable(const OpenableCreateInfo& createInfo, uint64_t parentObjectId);
    void Update(float deltaTime);
    void TriggerInteract(uint32_t handlerId);

    bool OpenableExists(uint32_t handlerId);
    Openable* GetOpenableByOpenableId(uint32_t handlerId);
}