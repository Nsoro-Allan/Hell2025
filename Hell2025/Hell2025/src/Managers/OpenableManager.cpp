#include "OpenableManager.h"
#include "World/World.h"
#include <atomic>
#include <cstdint>

namespace OpenableManager {
    std::unordered_map<uint32_t, Openable> g_openables;
    std::atomic<uint32_t> g_global = 1;
    
    uint32_t GetNextID() {
        return g_global++;
    }

    uint32_t CreateOpenable(const OpenableCreateInfo& createInfo, uint64_t parentObjectId) {
        uint64_t openableId = GetNextID();
        Openable& openable = g_openables[openableId];
        openable.Init(createInfo, parentObjectId);

        return openableId;
    }

    void Update(float deltaTime) {
        for (auto& pair : g_openables) {
            Openable& openable = pair.second;
            openable.Update(deltaTime);
        }
    }

    void TriggerInteract(uint32_t openableId) {
        if (!OpenableExists(openableId)) return;

        Openable* openStateHandler = GetOpenableByOpenableId(openableId);
        openStateHandler->Interact();
    }

    bool OpenableExists(uint32_t openableId) {
        return (g_openables.find(openableId) != g_openables.end());
    }

    Openable* GetOpenableByOpenableId(uint32_t openableId) {
        if (OpenableExists(openableId)) {
            return &g_openables[openableId];
        }
        else {
            return nullptr;
        }
    }
}