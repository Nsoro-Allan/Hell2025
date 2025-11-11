#include "Editor.h"

namespace Editor {

    bool backfaceCulling = true;

    void SetBackfaceCulling(bool value) {
        backfaceCulling = value;
    }

    bool BackfaceCullingEnabled() {
        return backfaceCulling;
    }

    bool BackfaceCullingDisabled() {
        return !backfaceCulling;
    }

}