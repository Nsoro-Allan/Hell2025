#include "Mermaid.h"
#include "Audio/Audio.h"
#include "Input/Input.h"

void Mermaid::Init(MermaidCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_spawnOffset = spawnOffset;

    m_transform.position = m_createInfo.position + m_spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, m_spawnOffset.yRotation, 0.0f);

    std::vector<MeshNodeCreateInfo> emptyMeshNodeCreateInfoSet;
    m_meshNodes.Init(NO_ID, "Mermaid", emptyMeshNodeCreateInfoSet);
    m_meshNodes.SetMaterialByMeshName("Arms", "MermaidArms");
    m_meshNodes.SetMaterialByMeshName("Body", "MermaidBody");
    m_meshNodes.SetMaterialByMeshName("BoobTube", "BoobTube");
    m_meshNodes.SetMaterialByMeshName("EyelashLower", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyelashUpper", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyeLeft", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("EyeRight", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("Face", "MermaidFace");
    m_meshNodes.SetMaterialByMeshName("HairInner", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairOutta", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairScalp", "MermaidScalp");
    m_meshNodes.SetMaterialByMeshName("Nails", "Nails");
    m_meshNodes.SetMaterialByMeshName("Rock", "Rock");
    m_meshNodes.SetMaterialByMeshName("Tail", "MermaidTail");
    m_meshNodes.SetMaterialByMeshName("TailFin", "MermaidTail");

    m_meshNodes.SetBlendingModeByMeshName("EyelashLower", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("EyelashUpper", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairScalp", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairOutta", BlendingMode::HAIR_TOP_LAYER);
    m_meshNodes.SetBlendingModeByMeshName("HairInner", BlendingMode::HAIR_UNDER_LAYER);
}

void Mermaid::Update(float deltaTime) {
    UpdateRenderItems();

    static bool titties = false;
    if (Input::KeyPressed(HELL_KEY_I)) {
        titties = !titties;
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
    }
    if (titties) {
        m_meshNodes.SetBlendingModeByMeshName("BoobTube", BlendingMode::DO_NOT_RENDER);
    }
    else {
        m_meshNodes.SetBlendingModeByMeshName("BoobTube", BlendingMode::DEFAULT);
    }
}

void Mermaid::UpdateRenderItems() {
    m_meshNodes.UpdateHierarchy();
    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
}

void Mermaid::CleanUp() {
    // Nothing as of yet
}