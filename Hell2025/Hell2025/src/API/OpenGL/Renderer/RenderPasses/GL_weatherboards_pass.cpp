#include "API/OpenGL/Renderer/GL_renderer.h"
#include "Types/House/WeatherBoards.h"
#include "AssetManagement/AssetManager.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"

#include "World/World.h"
#include "Util/Util.h"

namespace OpenGLRenderer {

    void WeatherBoardsPass() {


        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        //OpenGLShader* shader = GetShader("GBuffer");
        OpenGLShader* editorMeshShader = GetShader("EditorMesh");
        OpenGLShader* debugShader = GetShader("DebugTextured");

        if (!gBuffer) return;
        //if (!shader) return;
        if (!editorMeshShader) return;
        if (!debugShader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "WorldPosition", "Emissive" });



        Transform transform;
        transform.position = glm::vec3(0.0, 0.0, 0.0f);
        glm::mat4 modelMatrix = transform.to_mat4();

        // Render house
        debugShader->Bind();
        debugShader->SetMat4("u_model", modelMatrix);

        MeshBuffer weatherboardMeshBuffer = World::GetWeatherBoardMeshBuffer();


        glBindVertexArray(weatherboardMeshBuffer.GetGLMeshBuffer().GetVAO());

       //for (Vertex& vertex : vertices) {
       //
       //    if (vertex.position.z > 0.5f) {
       //        //DrawPoint(vertex.position, RED);
       //    }
       //    else {
       //        //DrawPoint(vertex.position, YELLOW);
       //    }
       //
       //}

        Material* material = AssetManager::GetMaterialByName("WeatherBoards0");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_basecolor)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_normal)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_rma)->GetGLTexture().GetHandle());

        SetRasterizerState("GeometryPass_Default");
        EditorRasterizerStateOverride();

        for (int i = 0; i < 4; i++) {
            int indexCount = weatherboardMeshBuffer.GetGLMeshBuffer().GetIndexCount();

            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;
            if (indexCount <= 0) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            debugShader->SetInt("u_viewportIndex", i);
                       
            int baseIndex = 0;
            int baseVertex = 0;
            glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * baseIndex), baseVertex);

        }


        glBindVertexArray(0);

        
    }
}
