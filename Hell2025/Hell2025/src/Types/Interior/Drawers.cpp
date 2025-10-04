#include "Drawers.h"
#include "AssetManagement/AssetManager.h"
#include "Core/OpenStateHandlerManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util/Util.h"

Drawers::Drawers(DrawersCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    static Model* model = nullptr;

    if (createInfo.type == DrawersType::LARGE) {
        model = AssetManager::GetModelByName("DrawersLarge");
    }
    else if (createInfo.type == DrawersType::SMALL) {
        model = AssetManager::GetModelByName("DrawersSmall");
    }

    if (!model) {
        std::cout << "Drawers() constructor failed because your hack to load the model into a static Model pointer failed the first time around\n";
    }

    m_meshNodes.InitFromModel(model);

    uint32_t materialIndex = AssetManager::GetMaterialIndexByName("Drawers");

    // Init material setup
    for (int i = 0; i < m_meshNodes.GetNodeCount(); i++) {
        Mesh* mesh = AssetManager::GetMeshByIndex(m_meshNodes.m_globalMeshIndices[i]);
        if (!mesh) continue;

        if (mesh->GetName().find("Frame") != std::string::npos) {
            m_meshNodes.m_objectTypes[i] = ObjectType::DRAWERS_FRAME;
        }
        else {
            m_meshNodes.m_objectTypes[i] = ObjectType::DRAWER;

            // Make open handler
            m_meshNodes.m_openHandlerStateIds[i] = OpenStateHandlerManager::CreateOpenStateHandler();
            OpenStateHandler* openStateHandler = OpenStateHandlerManager::GetOpenStateHandlerByHandlerId(m_meshNodes.m_openHandlerStateIds[i]);
            openStateHandler->m_openAxis = TRANSLATE_Z;
            openStateHandler->currentOpenValue = 0.0f;
            openStateHandler->minOpenValue = 0.0f;
            openStateHandler->maxOpenValue = 0.175f;
            openStateHandler->openSpeed = 1.5f;
            openStateHandler->closeSpeed = 1.5f;
            openStateHandler->openState = OpenState::CLOSED;
            openStateHandler->openingAudio = "DrawerOpen.wav";
            openStateHandler->closingAudio = "DrawerClose.wav";

            if (mesh->GetName() == "DrawersLarge_TopL") {
                openStateHandler->maxOpenValue = 0.125f;
                openStateHandler->m_locked = true;
            }

            if (mesh->GetName() == "DrawersLarge_TopR") {
                openStateHandler->maxOpenValue = 0.125f;
            }
        }

        m_meshNodes.m_objectIds[i] = UniqueID::GetNext();
        m_meshNodes.m_materialIndices[i] = materialIndex;
    }
}

void Drawers::Update(float deltaTime) {
   m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
   m_movedThisFrame = false; // WANRING I AM ONLY TEMPORATYIY HERE
}

void Drawers::CleanUp() {

}