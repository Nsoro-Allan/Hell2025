#include "Util.h"
#include "AssetManagement/AssetManager.h"

namespace Util {


    bool RayIntersectAABB(glm::vec3 rayOrigin, glm::vec3 inverseRayDir, float minDistance, float maxDistance, const glm::vec3& aabbBoundsMin, const glm::vec3& aabbBoundsMax, float& t, glm::vec3& localNormal) {
        // Compute t values for the slabs defined by the AABB
        glm::vec3 t1(
            (aabbBoundsMin[0] - rayOrigin[0]) * inverseRayDir[0],
            (aabbBoundsMin[1] - rayOrigin[1]) * inverseRayDir[1],
            (aabbBoundsMin[2] - rayOrigin[2]) * inverseRayDir[2]
        );

        glm::vec3 t2(
            (aabbBoundsMax[0] - rayOrigin[0]) * inverseRayDir[0],
            (aabbBoundsMax[1] - rayOrigin[1]) * inverseRayDir[1],
            (aabbBoundsMax[2] - rayOrigin[2]) * inverseRayDir[2]
        );

        // For each axis, tmin is the minimum and tmax is the maximum of t1 and t2
        glm::vec3 tminVec = glm::min(t1, t2);
        glm::vec3 tmaxVec = glm::max(t1, t2);

        // Compute the overall tmin and tmax
        float tmin = std::max({ tminVec.x, tminVec.y, tminVec.z, minDistance });
        float tmax = std::min({ tmaxVec.x, tmaxVec.y, tmaxVec.z, maxDistance });

		t = tmin;

		if (tmin > tmax) {
			return false;
		}

		// Determine the Local Normal based on which axis contributed the largest tminVec value
		localNormal = glm::vec3(0.0f);
		if (tmin == tminVec.x) {
			localNormal.x = (t1.x < t2.x) ? -1.0f : 1.0f;
		}
		else if (tmin == tminVec.y) {
			localNormal.y = (t1.y < t2.y) ? -1.0f : 1.0f;
		}
		else if (tmin == tminVec.z) {
			localNormal.z = (t1.z < t2.z) ? -1.0f : 1.0f;
		}

		return true; // tmin <= tmax
    }

    AABBRayResult RayIntersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDistance, const AABB& aabb, const glm::mat4& worldTransform) {
        AABBRayResult result;

		glm::mat4 inverseWorldTransform = glm::inverse(worldTransform);
		glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseWorldTransform));

        const float globalMinDistance = 0.001f;
        glm::vec3 localOrigin = glm::vec3(inverseWorldTransform * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localEnd = glm::vec3(inverseWorldTransform * glm::vec4(rayOrigin + rayDir * maxDistance, 1.0f));
        glm::vec3 localDir = glm::normalize(localEnd - localOrigin);
        float localMaxDistance = glm::length(localEnd - localOrigin);
        float localMinDistance = globalMinDistance * localMaxDistance / maxDistance;

        glm::vec3 inverseLocalRayDir = 1.0f / localDir;
        glm::vec3 boundsMin = aabb.GetBoundsMin();
        glm::vec3 boundsMax = aabb.GetBoundsMax();

        float t = 0.0f;
        glm::vec3 hitNormalLocal = glm::vec3(0.0f);

		if (RayIntersectAABB(localOrigin, inverseLocalRayDir, localMinDistance, localMaxDistance, boundsMin, boundsMax, t, hitNormalLocal)) {
			result.hitFound = true;
			result.hitPositionLocal = localOrigin + (localDir * t);
			result.hitPositionWorld = worldTransform * glm::vec4(result.hitPositionLocal, 1.0f);
			result.hitNormalLocal = hitNormalLocal;
			result.hitNormalWorld = glm::normalize(normalMatrix * result.hitNormalLocal);
        }

        return result;
    }

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