#include "Map.h"
#include "API/OpenGL/GL_util.h"
#include "BackEnd/BackEnd.h"
#include "HellLogging.h"
#include "Util.h"
#include "World/World.h"

void Map::CreateNew(const std::string& filename, int chunkCountX, int chunkCountZ, float initialHeight) {
    m_filename = filename;
    m_chunkCountX = chunkCountX;
    m_chunkCountZ = chunkCountZ;

    if (BackEnd::GetAPI() == API::OPENGL) {
        m_heightMapGLTexture.Create(GetTextureWidth(), GetTextureWidth(), GL_R16F, 1);
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        Logging::ToDo() << "Vulkan TODO: HeightMap::CreateNew()";
    }

    ClearToHeight(initialHeight);
    Logging::Debug() << "Created map: '" << filename << "' with height map texture size " << GetTextureWidth() << "x" << std::to_string(GetTextureWidth());
}

void Map::ClearToHeight(float height) {
    if (BackEnd::GetAPI() == API::OPENGL) {
        int internalFormat = GL_R16F;   
        m_heightMapGLTexture.ClearR(height / HEIGHTMAP_SCALE_Y);
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        std::cout << "Vulkan TODO: Map::ClearToHeight()\n";
    }
}

void Map::SetFilename(const std::string& filename) {
    m_filename = filename;
}

void Map::SetChunkCountX(int32_t count) {
    m_chunkCountX = count;
}

void Map::SetChunkCountZ(int32_t count) {
    m_chunkCountZ = count;
}

void Map::SetHeightMapData(int32_t chunkCountX, int32_t chunkCountZ, const std::vector<float>& data) {
    m_chunkCountX = chunkCountX;
    m_chunkCountZ = chunkCountZ;
    m_heightMapData = data;

    //Logging::Debug() << "m_chunkCountX: " << m_chunkCountX;
    //Logging::Debug() << "m_chunkCountZ: " << m_chunkCountZ;

    if (BackEnd::GetAPI() == API::OPENGL) {
        m_heightMapGLTexture.Create(GetTextureWidth(), GetTextureHeight(), GL_R16F, 1);
        m_heightMapGLTexture.UploadR16FData(m_heightMapData.data(), GetTextureWidth(), GetTextureHeight(), 0, 0, 0);
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        Logging::ToDo() << "Vulkan TODO: Map::SetHeightMapData()";
    }
}

void Map::SetCreateInfoCollection(CreateInfoCollection& createInfoCollection) {
    m_createInfoCollection = createInfoCollection;
}

void Map::SetAdditionalMapData(AdditionalMapData& additionalMapData) {
    m_additionalMapData = additionalMapData;
}

const glm::ivec2 Map::GetHeightMapTextureSize() {
    if (BackEnd::GetAPI() == API::OPENGL) {
        return glm::ivec2(m_heightMapGLTexture.GetWidth(), m_heightMapGLTexture.GetHeight());
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        std::cout << "Vulkan TODO: Map::GetHeightMapTextureSize()\n";
        return glm::ivec2(0, 0);
    }
}

void Map::AddPlayerCampaignSpawn(glm::vec3 position) {
    SpawnPoint& spawnPoint = m_additionalMapData.playerCampaignSpawns.emplace_back();
    spawnPoint = SpawnPoint(position, glm::vec3(0.0f));
}

void Map::AddPlayerDeathmatchSpawn(glm::vec3 position) {
    SpawnPoint& spawnPoint = m_additionalMapData.playerDeathmatchSpawns.emplace_back();
    spawnPoint = SpawnPoint(position, glm::vec3(0.0f));
}