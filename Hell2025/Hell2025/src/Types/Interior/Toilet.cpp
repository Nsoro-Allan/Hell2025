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


    std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;
    m_meshNodes.Init(NO_ID, "Toilet", emptyMeshNodeCreateInfoSet);


    uint32_t materialIndex = AssetManager::GetMaterialIndexByName("Toilet");

    // Init material setup
    m_meshNodes.SetMaterialByMeshName("Body", "Toilet");
    m_meshNodes.SetMaterialByMeshName("Lid", "Toilet");
    m_meshNodes.SetMaterialByMeshName("Seat", "Toilet");

     // Init handlers
    m_lidOpenHandler.m_currentOpenState = OpenState::OPEN;
    m_lidOpenHandler.m_minOpenValue = 0.0f;
    m_lidOpenHandler.m_maxOpenValue = 1.7f;
    m_lidOpenHandler.m_openSpeed = 6.825f;
    m_lidOpenHandler.m_closeSpeed = 6.825f;
    m_lidOpenHandler.m_openedAudio = "Piano_LidClose.wav";
    m_lidOpenHandler.m_closedAudio = "Piano_LidClose.wav";

    m_seatOpenHandler.m_currentOpenState = OpenState::CLOSED;
    m_seatOpenHandler.m_minOpenValue = 0.0f;
    m_seatOpenHandler.m_maxOpenValue = 1.7;
    m_seatOpenHandler.m_openSpeed = 7.25f;
    m_seatOpenHandler.m_closeSpeed = 7.25f;
    m_seatOpenHandler.m_openedAudio = "Piano_LidClose.wav";
    m_seatOpenHandler.m_closedAudio = "Piano_LidClose.wav";
}

void Toilet::Update(float deltaTime) {
    m_lidOpenHandler.Update(deltaTime);
    m_seatOpenHandler.Update(deltaTime);

    Transform lidNodeTransform;
    lidNodeTransform.rotation.x = -m_lidOpenHandler.m_currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Lid", lidNodeTransform);

    Transform seatNodeTransform;
    seatNodeTransform.rotation.x = m_seatOpenHandler.m_currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Seat", seatNodeTransform);

    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());

    m_movedThisFrame = m_lidOpenHandler.IsDirty() || m_seatOpenHandler.IsDirty();

}

void Toilet::CleanUp() {

}

void Toilet::InteractWithSeat() {
    if (m_lidOpenHandler.m_currentOpenState == OpenState::CLOSED) {
        m_seatOpenHandler.Interact();
    }
}

void Toilet::InteractWithLid() {
    if (m_seatOpenHandler.m_currentOpenState == OpenState::CLOSED) {
        m_lidOpenHandler.Interact();
    }
}
