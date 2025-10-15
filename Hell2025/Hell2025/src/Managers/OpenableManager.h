#pragma once
#include "CreateInfo.h"
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Handlers/Openable.h"

namespace OpenableManager {
    uint64_t CreateOpenable(const OpenableCreateInfo& createInfo);
    void Update(float deltaTime);
    void TriggerInteract(uint64_t handlerId);

    bool OpenableExists(uint64_t handlerId);
    Openable* GetOpenableByOpenableId(uint64_t handlerId);
}