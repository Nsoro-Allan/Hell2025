#include "Toilet.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util/Util.h"

Toilet::Toilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    static Model* model = AssetManager::GetModelByName("Toilet");
    if (!model) {
        std::cout << "Toilet() constructor failed because your hack to load the model into a static Model pointer failed the first time around\n";
    }

    m_meshNodes.InitFromModel(model);


    uint32_t materialIndex = AssetManager::GetMaterialIndexByName("Toilet");

    // Init material setup
    for (int i = 0; i < m_meshNodes.GetNodeCount(); i++) {
        Mesh* mesh = AssetManager::GetMeshByIndex(m_meshNodes.m_globalMeshIndices[i]);
        if (!mesh) continue;

        m_meshNodes.m_objectIds[i] = UniqueID::GetNextGlobal();

        if (mesh->GetName() == "Body") {
            m_meshNodes.m_materialIndices[i] = materialIndex;
            m_meshNodes.m_objectTypes[i] = ObjectType::TOILET;
        }
        if (mesh->GetName() == "Lid") {
            m_meshNodes.m_materialIndices[i] = materialIndex;
            m_meshNodes.m_objectTypes[i] = ObjectType::TOILET_LID;
        }
        if (mesh->GetName() == "Seat") {
            m_meshNodes.m_materialIndices[i] = materialIndex;
            m_meshNodes.m_objectTypes[i] = ObjectType::TOILET_SEAT;
        }
        if (mesh->GetName() == "ToiletWater") {
            // TODO
        }
    }

     // Init handlers
    m_lidOpenHandler.openState = OpenState::OPEN;
    m_lidOpenHandler.minOpenValue = 0.0f;
    m_lidOpenHandler.maxOpenValue = 1.7f;
    m_lidOpenHandler.openSpeed = 6.825f;
    m_lidOpenHandler.closeSpeed = 6.825f;
    m_lidOpenHandler.openedAudio = "Piano_LidClose.wav";
    m_lidOpenHandler.closedAudio = "Piano_LidClose.wav";

    m_seatOpenHandler.openState = OpenState::CLOSED;
    m_seatOpenHandler.minOpenValue = 0.0f;
    m_seatOpenHandler.maxOpenValue = 1.7;
    m_seatOpenHandler.openSpeed = 7.25f;
    m_seatOpenHandler.closeSpeed = 7.25f;
    m_seatOpenHandler.openedAudio = "Piano_LidClose.wav";
    m_seatOpenHandler.closedAudio = "Piano_LidClose.wav";
}

void Toilet::Update(float deltaTime) {
    m_lidOpenHandler.Update(deltaTime);
    m_seatOpenHandler.Update(deltaTime);

    Transform lidNodeTransform;
    lidNodeTransform.rotation.x = -m_lidOpenHandler.currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Lid", lidNodeTransform);

    Transform seatNodeTransform;
    seatNodeTransform.rotation.x = m_seatOpenHandler.currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Seat", seatNodeTransform);

    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());

    m_movedThisFrame = m_lidOpenHandler.movedThisFrame || m_seatOpenHandler.movedThisFrame;

}

void Toilet::CleanUp() {

}

void Toilet::InteractWithSeat() {
    if (m_lidOpenHandler.openState == OpenState::CLOSED) {
        m_seatOpenHandler.Interact();
    }
}

void Toilet::InteractWithLid() {
    if (m_seatOpenHandler.openState == OpenState::CLOSED) {
        m_lidOpenHandler.Interact();
    }
}
