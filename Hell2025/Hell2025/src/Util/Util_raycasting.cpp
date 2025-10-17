#include "Util.h"
#include "AssetManagement/AssetManager.h"

namespace Util {
    CubeRayResult CastCubeRay(const glm::vec3& rayOrigin, const glm::vec3 rayDir, std::vector<Transform>& cubeTransforms, float maxDistance) {
        CubeRayResult rayResult;
        rayResult.distanceToHit = std::numeric_limits<float>::max();

        Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Cube");
        if (!mesh) return rayResult;

        std::vector<Vertex>& vertices = AssetManager::GetVertices();
        std::vector<uint32_t>& indices = AssetManager::GetIndices();

        for (Transform& cubeTransform : cubeTransforms) {
            const glm::mat4& modelMatrix = cubeTransform.to_mat4();

            for (int i = mesh->baseIndex; i < mesh->baseIndex + mesh->indexCount; i += 3) {
                uint32_t idx0 = indices[i + 0];
                uint32_t idx1 = indices[i + 1];
                uint32_t idx2 = indices[i + 2];
                const glm::vec3& vert0 = modelMatrix * glm::vec4(vertices[idx0 + mesh->baseVertex].position, 1.0f);
                const glm::vec3& vert1 = modelMatrix * glm::vec4(vertices[idx1 + mesh->baseVertex].position, 1.0f);
                const glm::vec3& vert2 = modelMatrix * glm::vec4(vertices[idx2 + mesh->baseVertex].position, 1.0f);
                float t = 0;

                if (Util::RayIntersectsTriangle(rayOrigin, rayDir, vert0, vert1, vert2, t) && t < maxDistance && t < rayResult.distanceToHit) {
                    rayResult.distanceToHit = t;
                    rayResult.hitFound = true;
                    rayResult.hitPosition = rayOrigin + (rayDir * t); 
                    rayResult.cubeTransform = cubeTransform;
                    rayResult.hitNormal = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
                }
            }
        }

        return rayResult;
    }

    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
        const float EPSILON = 1e-8f;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(rayDir, edge2);
        float a = glm::dot(edge1, h);
        if (fabs(a) < EPSILON) {
            return false; // Ray is parallel to the triangle.
        }
        float f = 1.0f / a;
        glm::vec3 s = rayOrigin - v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) {
            return false;
        }
        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(rayDir, q);
        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }
        t = f * glm::dot(edge2, q); // Distance along the ray to the intersection.
        return t > EPSILON;
    }

    glm::vec3 GetMouseRayDir(glm::mat4 projection, glm::mat4 view, int windowWidth, int windowHeight, int mouseX, int mouseY) {
        float x = (2.0f * mouseX) / (float)windowWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / (float)windowHeight;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3(x, y, z);
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, ray_nds.z, 1.0f);
        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0f);
        glm::vec4 inv_ray_wor = (inverse(view) * ray_eye);
        glm::vec3 ray_wor = glm::vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
        ray_wor = normalize(ray_wor);
        return ray_wor;
    }

    bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& spherePosition, float sphereRadius) {
        glm::dvec3 oc = glm::dvec3(rayOrigin) - glm::dvec3(spherePosition);
        double b = glm::dot(oc, glm::dvec3(rayDir));
        double c = glm::dot(oc, oc) - (double)sphereRadius * (double)sphereRadius;
        double discriminant = b * b - c;

        if (discriminant < 0.0) return false;
        return (-b + glm::sqrt(discriminant)) >= 0.0;
    }
}