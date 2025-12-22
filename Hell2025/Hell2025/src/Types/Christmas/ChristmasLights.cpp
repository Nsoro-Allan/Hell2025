#include "ChristmasLights.h"
#include "AssetManagement/AssetManager.h"
#include "Util.h"
#include "Renderer/Renderer.h"
#include <array>
#include "HellLogging.h"

ChristmasLights::ChristmasLights(uint64_t id, ChristmasLightsCreateInfo& createInfo, SpawnOffset& spawnOffset) {
    m_objectId = id;
    m_createInfo = createInfo;

    m_position = createInfo.position + spawnOffset.translation;

    if (m_createInfo.points.empty()) {
        m_createInfo.points.push_back(createInfo.position); // store without spawn offset
        m_createInfo.sagHeights.push_back(0);
        return;
    }

    std::vector<glm::vec3> runtimePoints = m_createInfo.points;
    for (glm::vec3& p : runtimePoints) {
        p += spawnOffset.translation;
    }

    m_wires.clear();

    for (size_t i = 1; i < runtimePoints.size(); i++) {
        const glm::vec3& begin = runtimePoints[i - 1];
        const glm::vec3& end = runtimePoints[i];
        const float& sag = m_createInfo.sagHeights[i];
        Wire& wire = m_wires.emplace_back();
        wire.Init(begin, end, sag, m_createInfo.wireRadius, m_createInfo.spacing);
    }

    RecreateLightRenderItems();
}

void ChristmasLights::AddSegementFromLastPoint(const glm::vec3& nextPoint, float sag) {
    if (m_createInfo.points.empty()) {
        return;
    }

    m_createInfo.points.push_back(nextPoint);
    m_createInfo.sagHeights.push_back(sag);

    const glm::vec3& begin = m_createInfo.points[m_createInfo.points.size() - 2];
    const glm::vec3& end = m_createInfo.points[m_createInfo.points.size() - 1];

    Wire& wire = m_wires.emplace_back();
    wire.Init(begin, end, sag, m_createInfo.wireRadius, m_createInfo.spacing);

    RecreateLightRenderItems();
}

void ChristmasLights::RecreateLightRenderItems() {
    // TODO but something like this...
    static Model* model = AssetManager::GetModelByName("ChristmasLight");
    static int whiteMaterialIndex = AssetManager::GetMaterialIndexByName("ChristmasLightWhite");
    static int blackMaterialIndex = AssetManager::GetMaterialIndexByName("Black");

    std::vector<glm::mat4> modelMatrices;
    m_renderItems.clear();

    for (Wire& wire : m_wires) {
        for (const glm::vec3& position : wire.GetSegmentPoints()) {
            Transform transform;
            transform.position = position;
            transform.rotation = glm::vec3(Util::RandomFloat(-1, 1), Util::RandomFloat(-1, 1), Util::RandomFloat(-1, 1));
            transform.rotation.x += HELL_PI * -0.5f;
            transform.scale = glm::vec3(0.325f);
            modelMatrices.push_back(transform.to_mat4());
        }
    }

    // Light
    for (const glm::mat4& modelMatrix : modelMatrices) {
        Material* material = AssetManager::GetMaterialByIndex(whiteMaterialIndex);
        RenderItem renderItem;
        renderItem.modelMatrix = modelMatrix;
        renderItem.meshIndex = model->GetMeshIndices()[1];;
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        //renderItem.useEmissiveMask = 1.0f;                            // CHECK IM NOT IMPORTANT
        renderItem.castShadows = false;
        renderItem.emissiveR = 1.0f;
        renderItem.emissiveG = 0.0f;
        renderItem.emissiveB = 0.0f;
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        Util::UpdateRenderItemAABB(renderItem);
        m_renderItems.push_back(renderItem);
    }
    // Plastic
    for (const glm::mat4& modelMatrix : modelMatrices) {
        Material* material = AssetManager::GetMaterialByIndex(blackMaterialIndex);
        RenderItem renderItem;
        renderItem.modelMatrix = modelMatrix;
        renderItem.meshIndex = model->GetMeshIndices()[0];
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        renderItem.castShadows = false;
        Util::PackUint64(m_objectId, renderItem.objectIdLowerBit, renderItem.objectIdUpperBit);
        Util::UpdateRenderItemAABB(renderItem);
        m_renderItems.push_back(renderItem);
    }
}

void ChristmasLights::Update(float deltaTime) {
    // Define the patterns
    std::vector<std::array<bool, 4>> patterns = {
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},

        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
    glm::vec3 colors[] = {
        /* red    */ glm::vec3(1.0f, 0.0f, 0.0f),
        /* blue   */ glm::vec3(0.0f, 0.025f, 1.0f),
        /* yellow */ glm::vec3(1.0f, 0.5f, 0.0f),
        /* green  */ glm::vec3(0.05f, 0.9f, 0.05f)
    };

    if (m_time == 0) {
        m_time = Util::RandomFloat(0.0, 5.0f);
    }

    m_time += deltaTime;
    float flashSpeed = 0.09f;
    int currentPatternIndex = static_cast<int>(m_time / flashSpeed) % patterns.size();
    const auto& currentPattern = patterns[currentPatternIndex];

    //for (auto& p : m_createInfo.points) {
    //    Renderer::DrawPoint(p, RED);
    //}

    for (size_t i = 0; i < m_renderItems.size() / 2; i++) {
        int colorIndex = i % 4;
        glm::vec3 color = currentPattern[colorIndex] ? colors[colorIndex] : BLACK;
        m_renderItems[i].emissiveR = color.r;
        m_renderItems[i].emissiveG = color.g;
        m_renderItems[i].emissiveB = color.b;

        //Renderer::DrawPoint(m_renderItems[i].modelMatrix[3], glm::vec4(color, 1.0f));
    }
}

void ChristmasLights::CleanUp() {
    for (Wire& wire : m_wires) {
        wire.GetMeshBuffer().Reset();
    }
}