#include "OpenableManager.h"
#include "UniqueID.h"
#include "World/World.h"

namespace OpenableManager {
    std::unordered_map<uint64_t, Openable> g_openables;

    uint64_t CreateOpenable() {
        uint64_t openableId = UniqueID::GetNextGlobal();
        Openable& openable = g_openables[openableId];

        return openableId;
    }

    void Update(float deltaTime) {
        for (auto& pair : g_openables) {
            uint64_t openableId = pair.first;
            Openable& openable = pair.second;

            openable.Update(deltaTime);

            // This aint the fastest thing to be doing!!!
            for (Drawers& drawers : World::GetDrawers()) {
                for (uint64_t id : drawers.GetOpenableIds()) {
                    if (id == openableId) {
                        drawers.GetMeshNodes().SetTransformByMeshName(openable.meshName, openable.m_transform);
                        break;
                    }
                }
            }
        }
    }

    void TriggerInteract(uint64_t openableId) {
        if (!OpenableExists(openableId)) return;

        Openable* openStateHandler = GetOpeneableByOpenableId(openableId);
        openStateHandler->Interact();
    }

    bool OpenableExists(uint64_t openableId) {
        return (g_openables.find(openableId) != g_openables.end());
    }

    Openable* GetOpeneableByOpenableId(uint64_t openableId) {
        if (OpenableExists(openableId)) {
            return &g_openables[openableId];
        }
        else {
            return nullptr;
        }
    }

    //Transform GetOpenStateHandleTransformById(uint64_t openableId) {
    //    if (!OpenableExists(openableId)) return Transform();
    //
    //    Openable* openStateHandler = GetOpeneableByOpenableId(openableId);
    //    return openStateHandler->m_transform;
    //}
}