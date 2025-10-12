#include "Piano.h"
#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Audio/Synth.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Physics/Physics.h"
#include "World/World.h"
#include "Util.h"
#include "UniqueID.h"

void Piano::Init(PianoCreateInfo& createInfo) {
    m_createInfo = createInfo;

    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;

    m_pianoObjectId = UniqueID::GetNextGlobal();

    m_model = AssetManager::GetModelByName("Piano");
    m_meshNodes.InitFromModel(m_model);

    uint32_t materialIndex0 = AssetManager::GetMaterialIndexByName("Piano0");
    uint32_t materialIndex1 = AssetManager::GetMaterialIndexByName("Piano1");

    m_seatPosition = m_transform.to_mat4() * glm::vec4(0.75f, 0.0f, 0.75f, 1.0f);

    for (int i = 0; i < m_meshNodes.GetNodeCount(); i++) {
        uint32_t meshIndex = m_meshNodes.GetGlobalMeshIndex(i);

        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (!mesh) continue;

        uint64_t localObjectId = UniqueID::GetNextGlobal();

        if (mesh->GetName() == "Yamaha_Key_A0" ||
            mesh->GetName() == "Yamaha_Key_A0#" ||
            mesh->GetName() == "Yamaha_Key_B0" ||
            mesh->GetName() == "Yamaha_Key_C1" ||
            mesh->GetName() == "Yamaha_Key_C1#" ||
            mesh->GetName() == "Yamaha_Key_D1" ||
            mesh->GetName() == "Yamaha_Key_D1#" ||
            mesh->GetName() == "Yamaha_Key_E1" ||
            mesh->GetName() == "Yamaha_Key_F1" ||
            mesh->GetName() == "Yamaha_Key_F1#" ||
            mesh->GetName() == "Yamaha_Key_G1" ||
            mesh->GetName() == "Yamaha_Key_G1#" ||

            mesh->GetName() == "Yamaha_Key_A1" ||
            mesh->GetName() == "Yamaha_Key_A1#" ||
            mesh->GetName() == "Yamaha_Key_B1" ||
            mesh->GetName() == "Yamaha_Key_C2" ||
            mesh->GetName() == "Yamaha_Key_C2#" ||
            mesh->GetName() == "Yamaha_Key_D2" ||
            mesh->GetName() == "Yamaha_Key_D2#" ||
            mesh->GetName() == "Yamaha_Key_E2" ||
            mesh->GetName() == "Yamaha_Key_F2" ||
            mesh->GetName() == "Yamaha_Key_F2#" ||
            mesh->GetName() == "Yamaha_Key_G2" ||
            mesh->GetName() == "Yamaha_Key_G2#" ||

            mesh->GetName() == "Yamaha_Key_A2" ||
            mesh->GetName() == "Yamaha_Key_A2#" ||
            mesh->GetName() == "Yamaha_Key_B2" ||
            mesh->GetName() == "Yamaha_Key_C3" ||
            mesh->GetName() == "Yamaha_Key_C3#" ||
            mesh->GetName() == "Yamaha_Key_D3" ||
            mesh->GetName() == "Yamaha_Key_D3#" ||
            mesh->GetName() == "Yamaha_Key_E3" ||
            mesh->GetName() == "Yamaha_Key_F3" ||
            mesh->GetName() == "Yamaha_Key_F3#" ||
            mesh->GetName() == "Yamaha_Key_G3" ||
            mesh->GetName() == "Yamaha_Key_G3#" ||

            mesh->GetName() == "Yamaha_Key_A3" ||
            mesh->GetName() == "Yamaha_Key_A3#" ||
            mesh->GetName() == "Yamaha_Key_B3" ||
            mesh->GetName() == "Yamaha_Key_C4" ||
            mesh->GetName() == "Yamaha_Key_C4#" ||
            mesh->GetName() == "Yamaha_Key_D4" ||
            mesh->GetName() == "Yamaha_Key_D4#" ||
            mesh->GetName() == "Yamaha_Key_E4" ||
            mesh->GetName() == "Yamaha_Key_F4" ||
            mesh->GetName() == "Yamaha_Key_F4#" ||
            mesh->GetName() == "Yamaha_Key_G4" ||
            mesh->GetName() == "Yamaha_Key_G4#" ||

            mesh->GetName() == "Yamaha_Key_A4" ||
            mesh->GetName() == "Yamaha_Key_A4#" ||
            mesh->GetName() == "Yamaha_Key_B4" ||
            mesh->GetName() == "Yamaha_Key_C5" ||
            mesh->GetName() == "Yamaha_Key_C5#" ||
            mesh->GetName() == "Yamaha_Key_D5" ||
            mesh->GetName() == "Yamaha_Key_D5#" ||
            mesh->GetName() == "Yamaha_Key_E5" ||
            mesh->GetName() == "Yamaha_Key_F5" ||
            mesh->GetName() == "Yamaha_Key_F5#" ||
            mesh->GetName() == "Yamaha_Key_G5" ||
            mesh->GetName() == "Yamaha_Key_G5#" ||

            mesh->GetName() == "Yamaha_Key_A5" ||
            mesh->GetName() == "Yamaha_Key_A5#" ||
            mesh->GetName() == "Yamaha_Key_B5" ||
            mesh->GetName() == "Yamaha_Key_C6" ||
            mesh->GetName() == "Yamaha_Key_C6#" ||
            mesh->GetName() == "Yamaha_Key_D6" ||
            mesh->GetName() == "Yamaha_Key_D6#" ||
            mesh->GetName() == "Yamaha_Key_E6" ||
            mesh->GetName() == "Yamaha_Key_F6" ||
            mesh->GetName() == "Yamaha_Key_F6#" ||
            mesh->GetName() == "Yamaha_Key_G6" ||
            mesh->GetName() == "Yamaha_Key_G6#" ||

            mesh->GetName() == "Yamaha_Key_A6" ||
            mesh->GetName() == "Yamaha_Key_A6#" ||
            mesh->GetName() == "Yamaha_Key_B6" ||
            mesh->GetName() == "Yamaha_Key_C7" ||
            mesh->GetName() == "Yamaha_Key_C7#" ||
            mesh->GetName() == "Yamaha_Key_D7" ||
            mesh->GetName() == "Yamaha_Key_D7#" ||
            mesh->GetName() == "Yamaha_Key_E7" ||
            mesh->GetName() == "Yamaha_Key_F7" ||
            mesh->GetName() == "Yamaha_Key_F7#" ||
            mesh->GetName() == "Yamaha_Key_G7" ||
            mesh->GetName() == "Yamaha_Key_G7#" ||

            mesh->GetName() == "Yamaha_Key_A7" ||
            mesh->GetName() == "Yamaha_Key_A7#" ||
            mesh->GetName() == "Yamaha_Key_B7" ||
            mesh->GetName() == "Yamaha_Key_C8") {

            m_meshNodes.m_materialIndices[i] = materialIndex1;
            m_meshNodes.m_objectIds[i] = localObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO_KEY;

            m_keys[localObjectId] = PianoKey();
            PianoKey& pianoKey = m_keys[localObjectId];
            pianoKey.m_meshName = mesh->GetName();
            pianoKey.m_note = MeshNameToNote(mesh->GetName());
            pianoKey.m_isSharp = (mesh->GetName().find("#") != std::string::npos);
        }

        // Top cover 
        else if (mesh->GetName() == "Yamaha_Case.Top.Cover") {
            m_meshNodes.m_materialIndices[i] = materialIndex0;
            m_meshNodes.m_objectIds[i] = localObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO_TOP_COVER;
        }

        // Keyboard cover
        else if (mesh->GetName() == "Yamaha_Keyboard.Cover.Lock" || mesh->GetName() == "Yamaha_Keyboard.Cover") {
            m_meshNodes.m_materialIndices[i] = materialIndex0;
            m_meshNodes.m_objectIds[i] = localObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO_KEYBOARD_COVER;
        }

        // Music rest
        else if (mesh->GetName() == "Yamaha_Lyrics.Stand") {
            m_meshNodes.m_materialIndices[i] = materialIndex0;
            m_meshNodes.m_objectIds[i] = localObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO_SHEET_MUSIC_REST;
        }

        // Black body parts
        else if (mesh->GetName() == "Yamaha_Main") {
            m_meshNodes.m_materialIndices[i] = materialIndex0;
            m_meshNodes.m_objectIds[i] = m_pianoObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO;
        }

        // White body parts
        else {
            m_meshNodes.m_materialIndices[i] = materialIndex1;
            m_meshNodes.m_objectIds[i] = m_pianoObjectId;
            m_meshNodes.m_objectTypes[i] = ObjectType::PIANO;
        }        
    }

    // Create physics objects
    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(CHARACTER_CONTROLLER | BULLET_CASING | ITEM_PICK_UP | RAGDOLL_ENEMY);
    m_rigidStaticId = Physics::CreateRigidStaticConvexMeshFromModel(m_transform, "PianoConvexMesh", filterData);

    PhysicsUserData userData;
    userData.physicsId = m_rigidStaticId;
    userData.objectId = m_pianoObjectId;
    userData.physicsType = PhysicsType::RIGID_STATIC;
    userData.objectType = ObjectType::PIANO;
    Physics::SetRigidStaticUserData(m_rigidStaticId, userData);

    CalculatePianoKeyWorldspaceCenters();

    // Init handlers
    m_keyboardCoverOpenHandler.openState = OpenState::OPEN;
    m_keyboardCoverOpenHandler.minOpenValue = 0.0f;
    m_keyboardCoverOpenHandler.maxOpenValue = 2.1f;
    m_keyboardCoverOpenHandler.openSpeed = 6.825f;
    m_keyboardCoverOpenHandler.closeSpeed = 6.825f;
    m_keyboardCoverOpenHandler.openedAudio = "Piano_LidClose.wav";
    m_keyboardCoverOpenHandler.closedAudio = "Piano_LidClose.wav";
    m_keyboardCoverOpenHandler.Update(0.0f); // check u really want this???

    m_topCoverOpenHandler.openState = OpenState::CLOSED;
    m_topCoverOpenHandler.minOpenValue = 0.0f;
    m_topCoverOpenHandler.maxOpenValue = HELL_PI;
    m_topCoverOpenHandler.openSpeed = 7.25f;
    m_topCoverOpenHandler.closeSpeed = 7.25f;
    m_topCoverOpenHandler.openedAudio = "Piano_LidClose.wav";
    m_topCoverOpenHandler.closedAudio = "Piano_LidClose.wav";

    m_sheetMusicRestOpenHandler.openState = OpenState::CLOSED;
    m_sheetMusicRestOpenHandler.minOpenValue = 0.0f;
    m_sheetMusicRestOpenHandler.maxOpenValue = HELL_PI * 0.7f;
    m_sheetMusicRestOpenHandler.openSpeed = 8.5f;
    m_sheetMusicRestOpenHandler.closeSpeed = 8.5f;
    m_sheetMusicRestOpenHandler.openedAudio = "Piano_LidClose.wav";
    m_sheetMusicRestOpenHandler.closedAudio = "Piano_LidClose.wav";
}


void Piano::CalculatePianoKeyWorldspaceCenters() {
    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;
        uint32_t nodeIndex = m_meshNodes.m_localIndexMap[key.m_meshName];

        RenderItem* renderItem = m_meshNodes.GetRenderItemByNodeIndex(nodeIndex);
        AABB aabb = AABB(renderItem->aabbMin, renderItem->aabbMax);

        key.m_worldSpaceCenter = aabb.GetCenter();
    }
}

void Piano::SetPosition(glm::vec3 position) {
    m_createInfo.position = position;
    m_transform.position = position;

    // Update collision mesh position
    Physics::SetRigidStaticGlobalPose(m_rigidStaticId, m_transform.to_mat4());

    CalculatePianoKeyWorldspaceCenters();
}

void Piano::InteractWithKeyboardCover() {
    m_keyboardCoverOpenHandler.Interact();
}

void Piano::InteractWithTopCover() {
    m_topCoverOpenHandler.Interact();
}

void Piano::InteractWithSheetMusicRestCover() {
    m_sheetMusicRestOpenHandler.Interact();
}

void Piano::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_rigidStaticId);
}

void Piano::Update(float deltaTime) {
    m_modelMatrix = m_transform.to_mat4();

    // Update keys
    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;
        key.Update(deltaTime);

        int meshNodeIndex = m_meshNodes.m_localIndexMap[key.m_meshName];

        m_meshNodes.m_transforms[meshNodeIndex].position.y = key.m_yTranslation;
        m_meshNodes.m_transforms[meshNodeIndex].rotation.x = key.m_xRotation;
    }

    m_topCoverOpenHandler.Update(deltaTime);
    m_sheetMusicRestOpenHandler.Update(deltaTime);
    m_keyboardCoverOpenHandler.Update(deltaTime);
 
    
    //if (m_keyboardCoverOpenState == OpenState::OPEN) {
    //    m_keyboardCoverCurrentOpenValue = m_keyboardCoverMaxOpenValue;
    //}
    //if (m_keyboardCoverOpenState == OpenState::CLOSED) {
    //    m_keyboardCoverCurrentOpenValue = 0;
    //}

    Transform keyboardCoverTransform;
    keyboardCoverTransform.rotation.x = -m_keyboardCoverOpenHandler.currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Yamaha_Keyboard.Cover", keyboardCoverTransform);

    Transform topCoverTransform;
    topCoverTransform.rotation.x = -m_topCoverOpenHandler.currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Yamaha_Case.Top.Cover", topCoverTransform);

    Transform sheetMusicRestTransform;
    sheetMusicRestTransform.rotation.x = -m_sheetMusicRestOpenHandler.currentOpenValue;
    m_meshNodes.SetTransformByMeshName("Yamaha_Lyrics.Stand", sheetMusicRestTransform);



    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
}

void Piano::TriggerInternalNoteFromExternalBulletHit(glm::vec3 bulletHitPositon) {    
    std::cout << "TriggerInternalNoteFromExternalBulletHit()\n";

    float closetDistance = std::numeric_limits<float>::max();
    PianoKey* closetKey = nullptr;

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;
        float distance = glm::distance(key.m_worldSpaceCenter, bulletHitPositon);
        if (distance < closetDistance) {
            closetDistance = distance;
            closetKey = &key;
        }
    }

    if (closetKey) {
        closetKey->PressKey(127, 0.1f);
    }
}

bool Piano::PianoKeyExists(uint64_t pianoKeyId) {
    return (m_keys.find(pianoKeyId) != m_keys.end());
}

void Piano::PlayMajorFirstInversion(int rootNote) {

    int fifth = rootNote - 5;
    int majorThird = rootNote + 4;

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;

        if (key.m_note == rootNote) {
            key.PressKey();
        }
        if (key.m_note == majorThird) {
            key.PressKey();
        }
        if (key.m_note == fifth) {
            key.PressKey();
        }
    }
}

void Piano::PlayMajor7th(int rootNote) {

    int seventh = rootNote - 2;
    int majorThird = rootNote - 5 - 3;

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;

        if (key.m_note == rootNote) {
            key.PressKey();
        }
        if (key.m_note == seventh) {
            key.PressKey();
        }
        if (key.m_note == majorThird) {
            key.PressKey();
        }
    }
}

void Piano::PlayMinor(int rootNote) {

    int minorThird = rootNote + 3;
    int fifth = rootNote + 7;

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;

        if (key.m_note == rootNote) {
            key.PressKey();
        }
        if (key.m_note == minorThird) {
            key.PressKey();
        }
        if (key.m_note == fifth) {
            key.PressKey();
        }
    }
}


void Piano::PlayMajor(int rootNote) {

    int third = rootNote + 4;
    int fifth = rootNote + 7;

    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;

        if (key.m_note == rootNote) {
            key.PressKey();
        }
        if (key.m_note == third) {
            key.PressKey();
        }
        if (key.m_note == fifth) {
            key.PressKey();
        }
    }
}

void Piano::PlayKey(int note, int velocity, float duration) {
    for (auto& pair : m_keys) {
        const uint64_t& objectId = pair.first;
        PianoKey& key = pair.second;
        if (key.m_note == note) {
            key.PressKey(velocity, duration);
        }
    }
}

void Piano::SetSustain(bool value) {
    Synth::SetSustain(value);
}


PianoKey* Piano::GetPianoKey(uint64_t pianoKeyId) {
    if (!PianoKeyExists(pianoKeyId)) return nullptr;

    return &m_keys[pianoKeyId];
}

uint32_t Piano::MeshNameToNote(const std::string& meshName) {
    std::vector<std::string> meshNames(88);

    meshNames[0] = "Yamaha_Key_A0";
    meshNames[1] = "Yamaha_Key_A0#";
    meshNames[2] = "Yamaha_Key_B0";
    meshNames[3] = "Yamaha_Key_C1";
    meshNames[4] = "Yamaha_Key_C1#";
    meshNames[5] = "Yamaha_Key_D1";
    meshNames[6] = "Yamaha_Key_D1#";
    meshNames[7] = "Yamaha_Key_E1";
    meshNames[8] = "Yamaha_Key_F1";
    meshNames[9] = "Yamaha_Key_F1#";
    meshNames[10] = "Yamaha_Key_G1";
    meshNames[11] = "Yamaha_Key_G1#";
    meshNames[12] = "Yamaha_Key_A1";
    meshNames[13] = "Yamaha_Key_A1#";
    meshNames[14] = "Yamaha_Key_B1";
    meshNames[15] = "Yamaha_Key_C2";
    meshNames[16] = "Yamaha_Key_C2#";
    meshNames[17] = "Yamaha_Key_D2";
    meshNames[18] = "Yamaha_Key_D2#";
    meshNames[19] = "Yamaha_Key_E2";
    meshNames[20] = "Yamaha_Key_F2";
    meshNames[21] = "Yamaha_Key_F2#";
    meshNames[22] = "Yamaha_Key_G2";
    meshNames[23] = "Yamaha_Key_G2#";
    meshNames[24] = "Yamaha_Key_A2";
    meshNames[25] = "Yamaha_Key_A2#";
    meshNames[26] = "Yamaha_Key_B2";
    meshNames[27] = "Yamaha_Key_C3";
    meshNames[28] = "Yamaha_Key_C3#";
    meshNames[29] = "Yamaha_Key_D3";
    meshNames[30] = "Yamaha_Key_D3#";
    meshNames[31] = "Yamaha_Key_E3";
    meshNames[32] = "Yamaha_Key_F3";
    meshNames[33] = "Yamaha_Key_F3#";
    meshNames[34] = "Yamaha_Key_G3";
    meshNames[35] = "Yamaha_Key_G3#";
    meshNames[36] = "Yamaha_Key_A3";
    meshNames[37] = "Yamaha_Key_A3#";
    meshNames[38] = "Yamaha_Key_B3";
    meshNames[39] = "Yamaha_Key_C4";
    meshNames[40] = "Yamaha_Key_C4#";
    meshNames[41] = "Yamaha_Key_D4";
    meshNames[42] = "Yamaha_Key_D4#";
    meshNames[43] = "Yamaha_Key_E4";
    meshNames[44] = "Yamaha_Key_F4";
    meshNames[45] = "Yamaha_Key_F4#";
    meshNames[46] = "Yamaha_Key_G4";
    meshNames[47] = "Yamaha_Key_G4#";
    meshNames[48] = "Yamaha_Key_A4";
    meshNames[49] = "Yamaha_Key_A4#";
    meshNames[50] = "Yamaha_Key_B4";
    meshNames[51] = "Yamaha_Key_C5";
    meshNames[52] = "Yamaha_Key_C5#";
    meshNames[53] = "Yamaha_Key_D5";
    meshNames[54] = "Yamaha_Key_D5#";
    meshNames[55] = "Yamaha_Key_E5";
    meshNames[56] = "Yamaha_Key_F5";
    meshNames[57] = "Yamaha_Key_F5#";
    meshNames[58] = "Yamaha_Key_G5";
    meshNames[59] = "Yamaha_Key_G5#";
    meshNames[60] = "Yamaha_Key_A5";
    meshNames[61] = "Yamaha_Key_A5#";
    meshNames[62] = "Yamaha_Key_B5";
    meshNames[63] = "Yamaha_Key_C6";
    meshNames[64] = "Yamaha_Key_C6#";
    meshNames[65] = "Yamaha_Key_D6";
    meshNames[66] = "Yamaha_Key_D6#";
    meshNames[67] = "Yamaha_Key_E6";
    meshNames[68] = "Yamaha_Key_F6";
    meshNames[69] = "Yamaha_Key_F6#";
    meshNames[70] = "Yamaha_Key_G6";
    meshNames[71] = "Yamaha_Key_G6#";
    meshNames[72] = "Yamaha_Key_A6";
    meshNames[73] = "Yamaha_Key_A6#";
    meshNames[74] = "Yamaha_Key_B6";
    meshNames[75] = "Yamaha_Key_C7";
    meshNames[76] = "Yamaha_Key_C7#";
    meshNames[77] = "Yamaha_Key_D7";
    meshNames[78] = "Yamaha_Key_D7#";
    meshNames[79] = "Yamaha_Key_E7";
    meshNames[80] = "Yamaha_Key_F7";
    meshNames[81] = "Yamaha_Key_F7#";
    meshNames[82] = "Yamaha_Key_G7";
    meshNames[83] = "Yamaha_Key_G7#";
    meshNames[84] = "Yamaha_Key_A7";
    meshNames[85] = "Yamaha_Key_A7#";
    meshNames[86] = "Yamaha_Key_B7";
    meshNames[87] = "Yamaha_Key_C8";

    for (int i = 0; i < meshNames.size(); i++) {
        if (meshNames[i] == meshName) {
            return i + 21;
        }
    }

    return 0;
}

void PianoKey::Update(float deltaTime) {

    if (m_state == State::IDLE) {
        m_xRotation = 0.0f;
        m_yTranslation = 0.0f;
    }

    if (m_state == State::KEY_PRESSED) {
        m_timeRemaining -= deltaTime;

        if (m_timeRemaining <= 0) {
            m_timeRemaining = 0;
            m_state = State::IDLE;
            Synth::ReleaseNote(m_note);
        }

        if (!m_isSharp) {
            m_yTranslation = 0.0f;
            m_xRotation = 0.05f;
        }
        else {
            m_yTranslation = -0.01f;
            m_xRotation = 0.0f;
        }
    }

    //Mesh* mesh = AssetManager::GetMeshByIndex(m_meshIndex);
    //AABB aabb = AABB(mesh->aabbMin, mesh->aabbMax);

    //Transform orignOffsetTransform;
    //orignOffsetTransform.position = aabb.GetCenter();
    //orignOffsetTransform.position.y += aabb.GetExtents().y * 0.5f;
    //orignOffsetTransform.position.z -= aabb.GetExtents().z * 0.5f;
    //
    //Transform rotationTransform;
    //rotationTransform.position.y = m_yTranslation;
    //rotationTransform.rotation.x = m_xRotation;
    //
    //glm::mat4 originOffset = orignOffsetTransform.to_mat4();
    //glm::mat4 inverseOriginOffset = glm::inverse(originOffset);
    //glm::mat4 localRotation = rotationTransform.to_mat4();
    //
    //m_localOffsetMatrix = originOffset * localRotation * inverseOriginOffset;
}

//void PianoKey::UpdateWorldSpaceCenter(glm::mat4 parentPianoModelMatrix) {
//    Mesh* mesh = AssetManager::GetMeshByIndex(m_meshIndex);
//    if (!mesh) return;
//
//    AABB aabb = AABB(mesh->aabbMin, mesh->aabbMax);
//    m_worldSpaceCenter = parentPianoModelMatrix * glm::vec4(aabb.GetCenter(), 1.0f);
//}

void PianoKey::PressKey(int velocity, float duration) {
    // Play sound if you were not pressed already
    if (m_state == State::IDLE) {
        Synth::PlayNote(m_note, velocity);
    }

    m_state = State::KEY_PRESSED;
    m_timeRemaining = duration;
}