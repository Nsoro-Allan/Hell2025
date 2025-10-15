#include "OpenableManager.h"
#include "UniqueID.h"
#include "World/World.h"

namespace OpenableManager {
    std::unordered_map<uint64_t, Openable> g_openables;

    uint64_t CreateOpenable(const OpenableCreateInfo& createInfo) {
        uint64_t openableId = UniqueID::GetNext(ObjectType::OPENABLE);
        Openable& openable = g_openables[openableId];
        openable.Init(createInfo);

        return openableId;
    }

    void Update(float deltaTime) {
        for (auto& pair : g_openables) {
            Openable& openable = pair.second;
            uint64_t openableId = pair.first;
            uint64_t parentId = openable.m_parentObjectId;

            openable.Update(deltaTime);

            if (openable.m_currentOpenState == OpenState::OPENING ||
                openable.m_currentOpenState == OpenState::CLOSING) {
                ObjectType parentType = UniqueID::GetType(parentId);

                std::cout << "TRANSLATE_Z ";
                std::cout << "val: " << openable.m_currentOpenValue << " ";
                std::cout << "parentId: " << parentId << " ";
                std::cout << "parentType: " << Util::ObjectTypeToString(parentType) << " ";
                std::cout << "\n";
            }

            if (GenericObject* parent = World::GetGenericObjects().get(parentId)) {
                parent->GetMeshNodes().SetTransformByMeshName(openable.m_meshName, openable.m_transform);
                continue;
            }

            // More parent types here...
            // More parent types here...
            // More parent types here...
        }
    }

    void TriggerInteract(uint64_t openableId) {
        if (!OpenableExists(openableId)) return;

        Openable* openStateHandler = GetOpenableByOpenableId(openableId);
        openStateHandler->Interact();
    }

    bool OpenableExists(uint64_t openableId) {
        return (g_openables.find(openableId) != g_openables.end());
    }

    Openable* GetOpenableByOpenableId(uint64_t openableId) {
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