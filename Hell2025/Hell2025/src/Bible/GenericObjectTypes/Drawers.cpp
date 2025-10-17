#include "Bible/Bible.h"

namespace Bible {
    void ConfigureMeshNodesDrawersLarge(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& largeFrame = meshNodeCreateInfoSet.emplace_back();
        largeFrame.meshName = "DrawersLargeFrame";
        largeFrame.materialName = "T_Main_01a";

        MeshNodeCreateInfo& drawerTopL = meshNodeCreateInfoSet.emplace_back();
        drawerTopL.type = MeshNodeType::OPENABLE;
        drawerTopL.materialName = "T_Drawers_01a";
        drawerTopL.meshName = "DrawersLarge_TopL";
        drawerTopL.openable.additionalTriggerMeshNames = { "Handle_TopL" };
        drawerTopL.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawerTopL.openable.initialOpenState = OpenState::CLOSED;
        drawerTopL.openable.initialOpenValue = 0.0f;
        drawerTopL.openable.minOpenValue = 0.0f;
        drawerTopL.openable.maxOpenValue = 0.158f + 0.05f;
        drawerTopL.openable.openSpeed = 1.5f;
        drawerTopL.openable.closeSpeed = 1.5f;
        drawerTopL.openable.openingAudio = "DrawerOpen.wav";
        drawerTopL.openable.closingAudio = "DrawerClose.wav";
        drawerTopL.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& drawerTopR = meshNodeCreateInfoSet.emplace_back();
        drawerTopR.type = MeshNodeType::OPENABLE;
        drawerTopR.materialName = "T_Drawers_01a";
        drawerTopR.meshName = "DrawersLarge_TopR";
        drawerTopR.openable.additionalTriggerMeshNames = { "Handle_TopR" };
        drawerTopR.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawerTopR.openable.initialOpenState = OpenState::CLOSED;
        drawerTopR.openable.initialOpenValue = 0.0f;
        drawerTopR.openable.minOpenValue = 0.0f;
        drawerTopR.openable.maxOpenValue = 0.155f + 0.05f;
        drawerTopR.openable.openSpeed = 1.5f;
        drawerTopR.openable.closeSpeed = 1.5f;
        drawerTopR.openable.openingAudio = "DrawerOpen.wav";
        drawerTopR.openable.closingAudio = "DrawerClose.wav";
        drawerTopR.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& drawer2nd = meshNodeCreateInfoSet.emplace_back();
        drawer2nd.type = MeshNodeType::OPENABLE;
        drawer2nd.materialName = "T_Drawers_01a";
        drawer2nd.meshName = "DrawersLarge_2nd";
        drawer2nd.openable.additionalTriggerMeshNames = { "Handle_2nd" };
        drawer2nd.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawer2nd.openable.initialOpenState = OpenState::CLOSED;
        drawer2nd.openable.initialOpenValue = 0.0f;
        drawer2nd.openable.minOpenValue = 0.0f;
        drawer2nd.openable.maxOpenValue = 0.175f + 0.05f;
        drawer2nd.openable.openSpeed = 1.5f;
        drawer2nd.openable.closeSpeed = 1.5f;
        drawer2nd.openable.openingAudio = "DrawerOpen.wav";
        drawer2nd.openable.closingAudio = "DrawerClose.wav";
        drawer2nd.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& drawer3rd = meshNodeCreateInfoSet.emplace_back();
        drawer3rd.type = MeshNodeType::OPENABLE;
        drawer3rd.materialName = "T_Drawers_01a";
        drawer3rd.meshName = "DrawersLarge_3rd";
        drawer3rd.openable.additionalTriggerMeshNames = { "Handle_3rd" };
        drawer3rd.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawer3rd.openable.initialOpenState = OpenState::CLOSED;
        drawer3rd.openable.initialOpenValue = 0.0f;
        drawer3rd.openable.minOpenValue = 0.0f;
        drawer3rd.openable.maxOpenValue = 0.170f + 0.05f;
        drawer3rd.openable.openSpeed = 1.5f;
        drawer3rd.openable.closeSpeed = 1.5f;
        drawer3rd.openable.openingAudio = "DrawerOpen.wav";
        drawer3rd.openable.closingAudio = "DrawerClose.wav";
        drawer3rd.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& drawer4th = meshNodeCreateInfoSet.emplace_back();
        drawer4th.type = MeshNodeType::OPENABLE;
        drawer4th.materialName = "T_Drawers_01a";
        drawer4th.meshName = "DrawersLarge_4th";
        drawer4th.openable.additionalTriggerMeshNames = { "Handle_4th" };
        drawer4th.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawer4th.openable.initialOpenState = OpenState::CLOSED;
        drawer4th.openable.initialOpenValue = 0.0f;
        drawer4th.openable.minOpenValue = 0.0f;
        drawer4th.openable.maxOpenValue = 0.180f + 0.05f;
        drawer4th.openable.openSpeed = 1.5f;
        drawer4th.openable.closeSpeed = 1.5f;
        drawer4th.openable.openingAudio = "DrawerOpen.wav";
        drawer4th.openable.closingAudio = "DrawerClose.wav";
        drawer4th.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& handleTopL = meshNodeCreateInfoSet.emplace_back();
        handleTopL.materialName = "T_Handles_01a";
        handleTopL.meshName = "Handle_TopL";

        MeshNodeCreateInfo& handleTopR = meshNodeCreateInfoSet.emplace_back();
        handleTopR.materialName = "T_Handles_01a";
        handleTopR.meshName = "Handle_TopR";

        MeshNodeCreateInfo& handle2nd = meshNodeCreateInfoSet.emplace_back();
        handle2nd.materialName = "T_Handles_01a";
        handle2nd.meshName = "Handle_2nd";

        MeshNodeCreateInfo& handle3rd = meshNodeCreateInfoSet.emplace_back();
        handle3rd.materialName = "T_Handles_01a";
        handle3rd.meshName = "Handle_3rd";

        MeshNodeCreateInfo& handle4th = meshNodeCreateInfoSet.emplace_back();
        handle4th.materialName = "T_Handles_01a";
        handle4th.meshName = "Handle_4th";

        meshNodes.Init(id, "DrawersLarge", meshNodeCreateInfoSet);
    }
}