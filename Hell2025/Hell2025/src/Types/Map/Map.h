#pragma once
#include "CreateInfo.h"
#include "HellTypes.h"
#include "Types/Renderer/Texture.h"

struct Map {
    void CreateNew(const std::string& filename, int chunkCountX, int chunkCountZ, float initialHeight);
    void ClearToHeight(float height);
    void SetChunkCountX(int32_t count);
    void SetChunkCountZ(int32_t count);
    void SetFilename(const std::string& filename);
    void SetHeightMapData(int32_t chunkCountX, int32_t chunkCountZ, const std::vector<float>& data);
    void SetCreateInfoCollection(CreateInfoCollection& createInfoCollection);
    void SetAdditionalMapData(AdditionalMapData& additionalMapData);
    void AddPlayerDeathmatchSpawn(glm::vec3 position);
    void AddPlayerCampaignSpawn(glm::vec3 position);

    const glm::ivec2 GetHeightMapTextureSize();

    const std::string& GetFilename() const              { return m_filename; }
    int32_t GetChunkCountX() const                      { return m_chunkCountX; }
    int32_t GetChunkCountZ() const                      { return m_chunkCountZ; }
    int32_t GetTextureWidth() const                     { return m_chunkCountX * HEIGHT_MAP_CHUNK_PIXEL_SIZE; }
    int32_t GetTextureHeight() const                    { return m_chunkCountZ * HEIGHT_MAP_CHUNK_PIXEL_SIZE; }
    OpenGLTexture& GetHeightMapGLTexture()              { return m_heightMapGLTexture; }
    VulkanTexture& GetHeightMapVKTexture()              { return m_heightMapVKTexture; }
    AdditionalMapData& GetAdditionalMapData()           { return m_additionalMapData; }
    CreateInfoCollection& GetCreateInfoCollection()     { return m_createInfoCollection; }
    std::vector<float>& GetHeightMapData()              { return m_heightMapData; }

private:
    std::string m_filename;
    int32_t m_chunkCountX = 8;
    int32_t m_chunkCountZ = 8;
    std::vector<float> m_heightMapData;
    OpenGLTexture m_heightMapGLTexture;
    VulkanTexture m_heightMapVKTexture;
    CreateInfoCollection m_createInfoCollection;
    AdditionalMapData m_additionalMapData;
};