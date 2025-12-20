#pragma once
#include "Math/AABB.h"
#include "Math/OBB.h"
#include <string>
#include "API/OpenGL/Types/GL_cubemapView.h"
#include "API/OpenGL/Types/GL_mesh_buffer.h"
#include "API/OpenGL/Types/GL_mesh_patch.h"
#include "API/OpenGL/Types/GL_frameBuffer.h"
#include "API/OpenGL/Types/GL_shader.h"
#include "API/OpenGL/Types/GL_shadow_map.h"
#include "API/OpenGL/Types/GL_shadow_cube_map_array.h"
#include "API/OpenGL/Types/GL_shadow_map_array.h"
#include "API/OpenGL/Types/GL_texture_array.h"
#include "API/OpenGL/Types/GL_texture_3d.h"
#include "API/OpenGL/Types/GL_ssbo.hpp"
#include "Types/Map/Map.h"
#include "Viewport/Viewport.h"

struct OpenGLRasterizerState {
    GLboolean depthTestEnabled = true;
    GLboolean blendEnable = false;
    GLboolean cullfaceEnable = true;
    GLboolean depthMask = true;
    GLfloat pointSize = 1.0f;
    GLenum blendFuncSrcfactor = GL_SRC_ALPHA;
    GLenum blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
    GLenum depthFunc = GL_LESS;
};

namespace OpenGLRenderer {
    void Init();
    void InitMain();
    void RenderLoadingScreen();

    void PreGameLogicComputePasses();
    void RenderGame();

    // Compute passes
    void BlitRoads();
    void ComputeLightVolumeMask();
    void ComputeProbeLighting();
    void ComputeOceanFFTPass();
    void ComputeSkinningPass();
    void OceanHeightReadback();
    void PaintHeightMap();
    void UpdateGlobalIllumintation();
    void PointCloudDirectLighting();

    // Init passes
    void InitGrass();
    void InitOceanHeightReadback();

    // Render passes
    void DebugPass();
    void DebugViewPass();
    void DecalPass();
    void DecalPaintingPass();
    void EditorPass();
    void EmissivePass();
    void FurPass();
    void GeometryPass();
    void MirrorGeometryPass();
    void GlassPass();
    void GrassPass();
    void HairPass();
    void HeightMapPass(); 
    void HouseGeometryPass();
    void ImGuiPass();
    void InventoryGaussianPass();
    void LightCullingPass();
    void LightingPass();
    void OceanGeometryPass();
    void OceanSurfaceCompositePass();
    void OceanUnderwaterCompositePass();
    void OutlinePass();
    void PostProcessingPass();
    void WinstonPass();
    void ScreenSpaceDecalsPass();
    void SkyBoxPass();
    void SpriteSheetPass();
    void StainedGlassPass();
    void TextureReadBackPass();
    void UIPass();
    void VatBloodPass();
    void WeatherBoardsPass();
    void ChristmasLightsPass();
    void ExamineItemPass();

    // Debug passes
    void DrawPointCloud();
    void DrawLightVolume();

    // Utility passes
    void RecalculateAllHeightMapData(bool blitWorldMap);
    void ReadBackHeightMapData(Map* map);
    void ClearAllWoundMasks();

    // Render tasks
    void RenderShadowMaps();

    //void RenderHairLayer(const DrawCommands& drawCommands, int peelCount);
    void RenderHairLayer(const std::vector<DrawIndexedIndirectCommand>(*drawCommands)[4], int peelCount);
    
    void InitFog();
    void BlitFog();
    void RayMarchFog();

    // Debug
    void UpdateDebugMesh();
    void DrawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color, bool depthEnabled = false, int exclusiveViewportIndex = -1, int ignoredViewportIndex = -1);
    void DrawLine2D(const glm::ivec2& begin, const glm::ivec2& end, const glm::vec4& color);
    void DrawPoint(const glm::vec3& position, const glm::vec4& color, bool depthEnabled = false, int exclusiveViewportIndex = -1);
    void DrawPoint2D(const glm::ivec2& position, const glm::vec4& color);
    void DrawAABB(const AABB& aabb, const glm::vec4& color);
    void DrawAABB(const AABB& aabb, const glm::vec4& color, const glm::mat4& worldTransform);
    void DrawOBB(const OBB& obb, const glm::vec4& color);
    void DrawFrustum(const Frustum& frustum, const glm::vec4& color);
    void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color);
    void DebugBlitFrameBufferTexture(const std::string& frameBufferName, const std::string& attachmentName, GLint dstX, GLint dstY, GLint width, GLint height);
    void DebugBlitOpenGLTexture(GLuint textureHandle, float scale);
    void BlitDebugTextures();

    void DrawItemExamineLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color);
    void DrawItemExamineAABB(const AABB& aabb, const glm::vec4& color);

    inline std::vector<DebugVertex2D> g_points2D;
    inline std::vector<DebugVertex3D> g_points3D;
    inline std::vector<DebugVertex2D> g_lines2D;
    inline std::vector<DebugVertex3D> g_lines3D;
    inline std::vector<DebugVertex3D> g_itemExaminelines;

    void HotloadShaders();
    void CreateBlurBuffers();
    void DrawQuad();

    OpenGLCubemapView* GetCubemapView(const std::string& name);
    OpenGLFrameBuffer* GetBlurBuffer(int viewportIndex, int bufferIndex);
    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);
    OpenGLShader* GetShader(const std::string& name);
    OpenGLShadowMap* GetShadowMap(const std::string& name);
    OpenGLShadowCubeMapArray* GetShadowCubeMapArray(const std::string& name);
    OpenGLShadowMapArray* GetShadowMapArray(const std::string& name);
    OpenGLTextureArray* GetTextureArray(const std::string& name);
    OpenGLTexture3D* GetTexture3D(const std::string& name);
    OpenGLMeshPatch* GetOceanMeshPatch();

    std::vector<float>& GetShadowCascadeLevels();

    // SSBOs
    void CreateSSBO(const std::string& name, size_t size, GLbitfield flags);
    OpenGLSSBO* GetSSBO(const std::string& name);
    void UpdateSSBO(const std::string& name, size_t size, const void* data);

    // Misc
    void CreateGrassGeometry();
    void EditorRasterizerStateOverride();

    // Texture readback
    bool IsMouseRayWorldPositionReadBackReady();
    glm::vec3 GetMouseRayWorldPostion();
    bool IsPlayerRayWorldPositionReadBackReady(int playerIndex);
    glm::vec3 GetPlayerRayWorldPostion(int playerIndex);

    // Rasterizer State
    void InitRasterizerStates();
    OpenGLRasterizerState* CreateRasterizerState(const std::string& name);
    OpenGLRasterizerState* GetRasterizerState(const std::string& name);
    void SetRasterizerState(const std::string& name);

    // Drawing
    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands);
    void SplitMultiDrawIndirect(OpenGLShader* shader, const std::vector<DrawIndexedIndirectCommand>& commands, bool bindMaterial, bool bindWoundMaterial);

    // Util
    void SetViewport(OpenGLFrameBuffer* framebuffer, Viewport* viewport);
    void ClearFrameBufferByViewport(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLfloat r, GLfloat g = 0.0f, GLfloat b = 0.0f, GLfloat a = 0.0f);
    void ClearFrameBufferByViewportInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLint r, GLint g = 0.0f, GLint b = 0.0f, GLint a = 0.0f);
    void ClearFrameBufferByViewportUInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLuint r, GLuint g = 0.0f, GLuint b = 0.0f, GLuint a = 0.0f);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const Viewport* viewport);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, BlitRect srcRect, BlitRect dstRect);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, GLbitfield mask, GLenum filter);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    RenderItem2D CreateRenderItem2D(const std::string& textureName, glm::ivec2 location, glm::ivec2 viewportSize, Alignment alignment, glm::vec3 colorTint = WHITE, glm::ivec2 size = glm::ivec2(-1, -1));
    BlitRect BlitRectFromFrameBufferViewport(OpenGLFrameBuffer* framebuffer, Viewport* viewport);
    GLint CreateQuadVAO();
    void CopyDepthBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer);
    void GaussianBlur(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const std::string& srcAttachmentName, const std::string& dstAttachmentName, BlitRect srcRect, BlitRect dstRect, int blurRadius, int passCount);
    int GetFftDisplayMode();

    // TIDY ME
    inline bool g_flipNormalMapY = false;
    inline void FlipNormalMapY() {
        g_flipNormalMapY = !g_flipNormalMapY;
    }
    inline bool ShouldFlipNormalMapY() {
        return g_flipNormalMapY;
    }
    // TIDY ME
}