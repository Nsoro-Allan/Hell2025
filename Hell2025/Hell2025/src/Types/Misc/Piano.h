#pragma once
#include "HellTypes.h"
#include "Handlers/Openable.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"
#include <unordered_map>
#include "CreateInfo.h"

struct PianoKey {
    int32_t m_note = 0;
    glm::vec3 m_worldSpaceCenter = glm::vec3(0.0f);
    float m_xRotation = 0;
    float m_yTranslation = 0;
    bool m_isSharp = false;
    float m_timeRemaining = 0.0f;
    bool m_sustain = false;
    std::string m_meshName = "";

    void Update(float deltaTime);
    void PressKey(int velocity = 127, float duration = 0.05f);

    enum struct State {
        IDLE,
        KEY_PRESSED,
    } m_state = State::IDLE;
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

    Openable m_keyboardCoverOpenHandler;
    Openable m_topCoverOpenHandler;
    Openable m_sheetMusicRestOpenHandler;

    void InteractWithKeyboardCover();
    void InteractWithTopCover();
    void InteractWithSheetMusicRestCover();

    void PlayMajorFirstInversion(int rootNote); 
    void PlayMajor7th(int rootNote);
    void PlayMinor(int rootNote);
    void PlayMajor(int rootNote);
    void PlayKey(int note, int velocity = 127, float duration = 0.05f);

    bool PianoKeyExists(uint64_t pianoKeyId);
    //bool PianoBodyPartKeyExists(uint64_t pianoBodyPartId);
    PianoKey* GetPianoKey(uint64_t pianoKeyId);

    const std::vector<RenderItem>& GetRenderItems() const   { return m_meshNodes.GetRenderItems(); }
    const uint64_t& GetObjectId() const                     { return m_pianoObjectId; };
    const glm::vec3 GetPosition() const                     { return m_createInfo.position; }
    const glm::vec3& GetSeatPosition() const                { return m_seatPosition; }
    const PianoCreateInfo GetCreateInfo() const             { return m_createInfo; }
    const MeshNodes& GetMeshNodes() const                   { return m_meshNodes;  }

    static uint32_t MeshNameToNote(const std::string& meshName);
        
private:
    glm::vec3 m_seatPosition = glm::vec3(0.0f);
    uint64_t m_pianoObjectId = 0;
    uint64_t m_rigidStaticId = 0;
    Model* m_model = nullptr;
    Transform m_transform;
    glm::mat4 m_modelMatrix;                    // rename to world matrix
    PianoCreateInfo m_createInfo;
    MeshNodes m_meshNodes;

    std::unordered_map<uint64_t, PianoKey> m_keys;           // Mapped to object Id
};