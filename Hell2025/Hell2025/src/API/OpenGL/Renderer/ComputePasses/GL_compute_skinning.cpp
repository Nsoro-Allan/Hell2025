#include "../GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"

namespace OpenGLRenderer {

    void ComputeSkinningPass() {
        OpenGLShader* shader = GetShader("ComputeSkinning");
        OpenGLSSBO* skinningTransforms = GetSSBO("SkinningTransforms");

        if (!shader) return;
        if (!skinningTransforms) return;

        // Append all animated transforms into a single buffer
        std::vector<AnimatedGameObject*> animatedGameObjects = RenderDataManager::GetAnimatedGameObjectToSkin();
        RenderDataManager::skinningTransforms.clear();
        for (int i = 0; i < animatedGameObjects.size(); i++) {
            AnimatedGameObject* animatedGameObject = animatedGameObjects[i];
            const std::vector<glm::mat4>& transforms = animatedGameObject->GetBoneSkinningMatrices();
            RenderDataManager::skinningTransforms.insert(RenderDataManager::skinningTransforms.end(), transforms.begin(), transforms.end());       
        }

        // Upload those transforms to the gpu
        skinningTransforms->Update(RenderDataManager::skinningTransforms.size() * sizeof(glm::mat4), &RenderDataManager::skinningTransforms[0]);
               
        // Calculate total amount of vertices to skin and allocate space
        uint32_t totalVertexCount = 0;
        for (AnimatedGameObject* animatedGameObject : animatedGameObjects) {
            SkinnedModel* skinnedModel = animatedGameObject->GetSkinnedModel();
            if (!skinnedModel) continue;
            for (uint32_t i = 0; i < skinnedModel->GetMeshCount(); i++) {
                int meshindex = skinnedModel->GetMeshIndices()[i];
                SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshindex);
                totalVertexCount += mesh->vertexCount;
            }
        }

        // Make sure there is enough space allocated on the gpu to store them all
        OpenGLBackEnd::AllocateSkinnedVertexBufferSpace(totalVertexCount);

        // Skin
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, OpenGLBackEnd::GetSkinnedVertexDataVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, OpenGLBackEnd::GetWeightedVertexDataVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, skinningTransforms->GetHandle());
        shader->Bind();
        int j = 0;
        uint32_t baseOutputVertex = 0;
        uint32_t baseTransformIndex = 0;
        for (AnimatedGameObject* animatedGameObject : animatedGameObjects) {
            SkinnedModel* skinnedModel = animatedGameObject->GetSkinnedModel();
            if (!skinnedModel) continue;

            for (uint32_t i = 0; i < skinnedModel->GetMeshCount(); i++) {
                uint32_t meshindex = skinnedModel->GetMeshIndices()[i];
                SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshindex);
                shader->SetInt("vertexCount", mesh->vertexCount);
                shader->SetInt("baseInputVertex", mesh->baseVertexGlobal);
                shader->SetInt("baseOutputVertex", baseOutputVertex);
                shader->SetInt("baseTransformIndex", baseTransformIndex);
                shader->SetInt("vertexCount", mesh->vertexCount);

                GLuint workgroupSize = 128;
                GLuint groupCountX = (mesh->vertexCount + workgroupSize - 1) / workgroupSize;
                glDispatchCompute(groupCountX, 1, 1);

                baseOutputVertex += mesh->vertexCount;
            }
            baseTransformIndex += (uint32_t)animatedGameObject->GetBoneSkinningMatrices().size();
            j++;
        }
    }
}