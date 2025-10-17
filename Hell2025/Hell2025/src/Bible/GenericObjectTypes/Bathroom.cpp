#include "Bible/Bible.h"

namespace Bible {
    void ConfigureMeshNodesToilet(uint64_t id, MeshNodes& meshNodes) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& largeFrame = meshNodeCreateInfoSet.emplace_back();
        largeFrame.meshName = "Body";
        largeFrame.materialName = "Toilet";

        MeshNodeCreateInfo& drawer1st = meshNodeCreateInfoSet.emplace_back();
        drawer1st.type = MeshNodeType::OPENABLE;
        drawer1st.materialName = "Toilet";
        drawer1st.meshName = "Lid";
        drawer1st.openable.openAxis = OpenAxis::ROTATE_X;
        drawer1st.openable.initialOpenState = OpenState::OPEN;
        drawer1st.openable.minOpenValue = 0.0f;
        drawer1st.openable.maxOpenValue = 1.7f;
        drawer1st.openable.openSpeed = 6.825f;
        drawer1st.openable.closeSpeed = 6.825f;
        drawer1st.openable.openingAudio = "DrawerOpen.wav";
        drawer1st.openable.closingAudio = "DrawerClose.wav";

        MeshNodeCreateInfo& drawer2nd = meshNodeCreateInfoSet.emplace_back();
        drawer2nd.type = MeshNodeType::OPENABLE;
        drawer2nd.materialName = "Toilet";
        drawer2nd.meshName = "Seat";
        drawer2nd.openable.openAxis = OpenAxis::ROTATE_X;
        drawer2nd.openable.initialOpenState = OpenState::CLOSED;
        drawer2nd.openable.minOpenValue = 0.0f;
        drawer2nd.openable.maxOpenValue = 1.7;
        drawer2nd.openable.openSpeed = 7.25f;
        drawer2nd.openable.closeSpeed = 7.25f;
        drawer2nd.openable.openingAudio = "DrawerOpen.wav";
        drawer2nd.openable.closingAudio = "DrawerClose.wav";

        meshNodes.Init(id, "Toilet", meshNodeCreateInfoSet);
    }

}