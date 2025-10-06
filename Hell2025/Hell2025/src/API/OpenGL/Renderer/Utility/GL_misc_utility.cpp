#include "../GL_renderer.h"

namespace OpenGLRenderer {
    
    void ClearAllWoundMasks() {
        OpenGLTextureArray* woundMaskArray = GetTextureArray("WoundMasks");
        if (!woundMaskArray) return;

        for (int i = 0; i < WOUND_MASK_TEXTURE_ARRAY_SIZE; i++) {
            woundMaskArray->Clear(0, 0, 0, 0, i);
        }
    }
}