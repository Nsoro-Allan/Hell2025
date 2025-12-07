#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"
#include <unordered_map>
#include "CreateInfo.h"

struct PianoKey {

    enum struct State {
        IDLE,
        KEY_DOWN
    };

    int32_t m_note = 0;
    glm::vec3 m_worldSpaceCenter = glm::vec3(0.0f);
    float m_xRotation = 0;
    float m_yTranslation = 0;
    bool m_isSharp = false;
    std::string m_meshName = "";
    State m_state = State::IDLE;

    void Update(float deltaTime);
    void PressKey(int velocity = 127, float duration = 0.05f);

    bool IsDirty() const { return m_dirty; }

private:
    float m_timeRemaining = 0.0f;
    bool m_dirty = true;
    bool m_sustain = false;
};

struct Piano {
    Piano() = default;
    void Init(PianoCreateInfo& createInfo);
    void SetPosition(glm::vec3 position);
    void SetSustain(bool value);
    void Update(float deltaTime);
    void CleanUp();
    void TriggerInternalNoteFromExternalBulletHit(glm::vec3 bulletHitPositon);
    void CalculatePianoKeyWorldspaceCenters();
    void PressKey(uint32_t customId);

    void PlayMajorFirstInversion(int rootNote); 
    void PlayMajor7th(int rootNote);
    void PlayMinor(int rootNote);
    void PlayMajor(int rootNote);
    void PlayKey(int note, int velocity = 127, float duration = 0.05f);

    bool PianoKeyExists(uint64_t pianoKeyId);
	PianoKey* GetPianoKey(uint64_t pianoKeyId);

	MeshNodes& GetMeshNodes()                               { return m_meshNodes; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_meshNodes.GetRenderItems(); }
    const uint64_t& GetObjectId() const                     { return m_pianoObjectId; };
    const glm::vec3 GetPosition() const                     { return m_createInfo.position; }
    const glm::vec3& GetSeatPosition() const                { return m_seatPosition; }
    const PianoCreateInfo GetCreateInfo() const             { return m_createInfo; }

    static uint32_t MeshNameToNote(const std::string& meshName);
        
private:
    glm::vec3 m_seatPosition = glm::vec3(0.0f);
    glm::mat4 m_worldMatrix;
    uint64_t m_pianoObjectId = 0;
    uint64_t m_rigidStaticId = 0;
    Transform m_transform;
    PianoCreateInfo m_createInfo;
    MeshNodes m_meshNodes;
    std::unordered_map<uint32_t, PianoKey> m_keys;
};