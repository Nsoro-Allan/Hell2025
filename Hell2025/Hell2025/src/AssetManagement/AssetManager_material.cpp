#include "AssetManager.h"

namespace AssetManager {

    bool FileInfoIsAlbedoTexture(const FileInfo& fileInfo) {
        if (fileInfo.name.size() >= 4 && fileInfo.name.substr(fileInfo.name.size() - 4) == "_ALB") {
            return true;
        }
        return false;
    }

    std::string GetMaterialNameFromFileInfo(const FileInfo& fileInfo) {
        const std::string suffix = "_ALB";
        if (fileInfo.name.size() > suffix.size() && fileInfo.name.substr(fileInfo.name.size() - suffix.size()) == suffix) {
            return fileInfo.name.substr(0, fileInfo.name.size() - suffix.size());
        }
        return "";
    }

    void CreateGoldenVariant(const std::string& srcName, const std::string& dstName) {
        Material* material = AssetManager::GetMaterialByName(srcName);
        if (material) {
            std::vector<Material>& materials = GetMaterials();
            Material& goldenVariant = materials.emplace_back(Material());
            goldenVariant.m_name = dstName;
            goldenVariant.m_basecolor = GetTextureIndexByName("Gold_ALB", true);
            goldenVariant.m_normal = material->m_normal;
            goldenVariant.m_rma = GetTextureIndexByName("Gold_RMA", true);
        }
    }

    void BuildMaterials() {
        std::vector<Material>& materials = GetMaterials();
        std::vector<Texture>& textures = GetTextures();

        // Defaults
		int defaultNormal = -1;
		int defaultRma = -1;
		int defaultEmi = -1;
        for (int i = 0; i < textures.size(); i++) {
            if (textures[i].GetFileName() == "DefaultNRM") {
                defaultNormal = i;
			}
			if (textures[i].GetFileName() == "DefaultRMA") {
				defaultRma = i;
			}
			if (textures[i].GetFileName() == "Black") {
                defaultEmi = i;
			}
        }

        // Look for textures with _ALB suffix, create a material, and search for accompanying _NRM & _RMA textures
        materials.clear();

        for (int i = 0; i < textures.size(); i++) {
            Texture& texture = textures[i];

            if (FileInfoIsAlbedoTexture(texture.GetFileInfo())) {
                Material& material = materials.emplace_back(Material());

                material.m_name = GetMaterialNameFromFileInfo(texture.GetFileInfo());
                material.m_basecolor = i;
				material.m_normal = defaultNormal;
				material.m_rma = defaultRma;
				material.m_emissive = defaultEmi;

                for (int j = 0; j < textures.size(); j++) {
                    if (textures[j].GetFileName() == material.m_name + "_ALB") {
                        material.m_basecolor = j;
                        break;
                    }
                }

                for (int j = 0; j < textures.size(); j++) {
                    if (textures[j].GetFileName() == material.m_name + "_NRM") {
                        material.m_normal = j;
                        break;
                    }
                }

				for (int j = 0; j < textures.size(); j++) {
					if (textures[j].GetFileName() == material.m_name + "_RMA") {
						material.m_rma = j;
						break;
					}
				}

				for (int j = 0; j < textures.size(); j++) {
					if (textures[j].GetFileName() == material.m_name + "_EMI") {
						material.m_emissive = j;
						break;
					}
				}
            }
        }
    }

    Material* GetDefaultMaterial() {
        int index = GetMaterialIndexByName(DEFAULT_MATERIAL_NAME);
        return GetMaterialByIndex(index);
    }

    Material* GetMaterialByName(const std::string& name) {
        int index = GetMaterialIndexByName(name);
        return GetMaterialByIndex(index);
    }

    Material* GetMaterialByIndex(int index) {
        std::vector<Material>& materials = GetMaterials();
        if (index >= 0 && index < materials.size()) {
            Material* material = &materials[index];
            Texture* baseColor = AssetManager::GetTextureByIndex(material->m_basecolor);
            Texture* normal = AssetManager::GetTextureByIndex(material->m_normal);
            Texture* rma = AssetManager::GetTextureByIndex(material->m_rma);
            if (baseColor && baseColor->BakeComplete() &&
                normal && normal->BakeComplete() &&
                rma && rma->BakeComplete()) {
                return &materials[index];
            }
            else {
                return GetDefaultMaterial();
            }
        }
        else {
            //std::cout << "AssetManager::GetMaterialByIndex(int index) failed because index '" << index << "' is out of range. Size is " << g_materials.size() << "!\n";
            return GetDefaultMaterial();
        }
    }

    std::string GetMaterialNameByIndex(int index) {
        std::vector<Material>& materials = GetMaterials();
        if (index >= 0 && index < materials.size()) {
            return materials[index].m_name;
        }
        std::cout << "AssetManager::GetMaterialNameByIndex(int index) failed because index " << index << " was out of range of size << " << materials.size() << "\n";
        return "UNDEFINED_MATERIAL_NAME";
    }

    int GetMaterialIndexByName(const std::string& name) {
        std::unordered_map<std::string, int>& indexMap = GetMaterialIndexMap();
        auto it = indexMap.find(name);
        if (it != indexMap.end()) {
            return it->second;
        }
        else {
            //std::cout << "AssetManager::GetMaterialIndexByName(const std::string& name) failed because '" << name << "' does not exist\n";
            return -1;
        }
    }
}