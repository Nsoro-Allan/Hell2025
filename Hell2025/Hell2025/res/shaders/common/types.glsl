
struct ViewportData {
    mat4 projection;
    mat4 inverseProjection;
    mat4 view;
    mat4 inverseView;
    mat4 projectionView;
    mat4 inverseProjectionView;
    mat4 skyboxProjectionView;
    mat4 flashlightProjectionView;
    int xOffset;
    int yOffset;
    int width;
    int height;
    float posX;  // 0 t0 1 range
    float posY;  // 0 t0 1 range
    float sizeX; // 0 t0 1 range
    float sizeY; // 0 t0 1 range
    vec4 frustumPlane0;
    vec4 frustumPlane1;
    vec4 frustumPlane2;
    vec4 frustumPlane3;
    vec4 frustumPlane4;
    vec4 frustumPlane5;
    vec4 flashlightDir;
    vec4 flashlightPosition;
    float flashlightModifer;
    bool isOrtho;
    float orthoSize;
    float fov;
    vec4 viewPos;
    vec4 cameraForward;
    vec4 colorTint;
    float colorContrast;
    float padding0;
    float padding1;
    float padding2;
};

struct RendererData {
    float nearPlane;
    float farPlane;
    float gBufferWidth;
    float gBufferHeight;
    float hairBufferWidth;
    float hairBufferHeight;
    float time;
    int splitscreenMode;
    int rendererOverrideState;
    float normalizedMouseX;
    float normalizedMouseY;
    int tileCountX;
    int tileCountY;
};

struct RenderItem {
    mat4 modelMatrix;
    mat4 inverseModelMatrix;
    vec4 aabbMin;
    vec4 aabbMax;

    int meshIndex;
    int baseColorTextureIndex;
    int normalMapTextureIndex;
    int rmaTextureIndex;

    int objectType;
    int woundMaskTextureIndex; 
    int baseSkinnedVertex;
    int ignoredViewportIndex;

    int exclusiveViewportIndex;
    int skinned; // True or false
    uint objectIdLowerBit;
    uint objectIdUpperBit;

    float emissiveR;
    float emissiveG;
    float emissiveB;
    int castShadows; // True or false

    float furLength;
    float furShellDistanceAttenuation;
    float furUVScale;
    int localMeshNodeIndex;

    int blockScreenSpaceBloodDecals;  // True or false
    int emissiveTextureIndex;         // -1 means nothing, anything else is a texture index
    int padding1;
    int padding2;

    int woundBaseColorTextureIndex;
    int woundNormalMapTextureIndex;
    int woundRmaTextureIndex;
    int padding3;
};

struct Light {
    float posX;
    float posY;
    float posZ;
    float colorR;

    float colorG;
    float colorB;
    float strength;
    float radius;

    int lightIndex;
    int shadowMapDirty; // true or false
    int padding0;
    int padding1;
};

struct TileLightData {
    uint lightCount;
    uint lightIndices[127];
};