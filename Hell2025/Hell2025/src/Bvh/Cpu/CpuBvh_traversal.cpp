#include "CpuBvh.h"
#include "Renderer/Renderer.h"
#include "HellConstants.h"
#include "HellTypes.h"
#include "UniqueID.h"
#include "Util.h"

namespace Bvh::Cpu {

    bool IntersectNodeGpu(const RayData& rayData, const glm::vec3& aabbBoundsMin, const glm::vec3& aabbBoundsMax, float& t) {
        // Compute t values for the slabs defined by the AABB
        glm::vec3 t1(
            (aabbBoundsMin[0] - rayData.origin[0]) * rayData.invDir[0],
            (aabbBoundsMin[1] - rayData.origin[1]) * rayData.invDir[1],
            (aabbBoundsMin[2] - rayData.origin[2]) * rayData.invDir[2]
        );

        glm::vec3 t2(
            (aabbBoundsMax[0] - rayData.origin[0]) * rayData.invDir[0],
            (aabbBoundsMax[1] - rayData.origin[1]) * rayData.invDir[1],
            (aabbBoundsMax[2] - rayData.origin[2]) * rayData.invDir[2]
        );

        // For each axis, tmin is the minimum and tmax is the maximum of t1 and t2
        glm::vec3 tminVec = glm::min(t1, t2);
        glm::vec3 tmaxVec = glm::max(t1, t2);

        // Compute the overall tmin and tmax
        float tmin = std::max({ tminVec.x, tminVec.y, tminVec.z, rayData.minDistance });
        float tmax = std::min({ tmaxVec.x, tmaxVec.y, tmaxVec.z, rayData.maxDistance });

        t = tmin;
        return tmin <= tmax;
    }

    bool IntersectTri(glm::vec3 rayOrigin, glm::vec3 rayDir, float minDistance, float maxDistance, glm::vec3 p0, glm::vec3 e1, glm::vec3 e2, glm::vec3 normal, float& t) {
        // Compute the vector from the ray origin to p0
        glm::vec3 c = p0 - rayOrigin;

        // Compute the cross product of the ray direction and c
        glm::vec3 r = glm::cross(rayDir, c);

        // Calculate the determinant
        float det = glm::dot(normal, rayDir);

        // Avoid division by zero when ray is parallel to the triangle
        if (det == 0.0f) {
            return false;
        }
        float invDet = 1.0f / det;

        // Compute barycentric coordinates
        float u = glm::dot(r, e2) * invDet;
        float v = glm::dot(r, e1) * invDet;
        float w = 1.0f - u - v; // The third barycentric coordinate

        // Check if the intersection lies inside the triangle
        if (u >= 0.0f && v >= 0.0f && w >= 0.0f) {

            // Calculate distance to the hit
            t = glm::dot(normal, c) * invDet;
            if (t >= minDistance && t < maxDistance) {
                return true;
            }
        }
        return false;
    }

    BvhRayResult MeshAnyHit(GpuPrimitiveInstance instance, glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDistance) {
        uint32_t stack[MAX_BVH_STACK_SIZE];
        size_t currentStackSize = 0;

        // Calculate world ray in mesh space
        const float globalMinDistance = 0.001f;
        glm::vec3 localOrigin = glm::vec3(instance.inverseWorldTransform * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localEnd = glm::vec3(instance.inverseWorldTransform * glm::vec4(rayOrigin + rayDir * maxDistance, 1.0f));
        glm::vec3 localDir = glm::normalize(localEnd - localOrigin);
        float localMaxDistance = glm::length(localEnd - localOrigin);
        float localMinDistance = globalMinDistance * localMaxDistance / maxDistance;

        RayData rayData = ComputeRayData(localOrigin, localDir, localMinDistance, localMaxDistance);

        const std::vector<BvhNode>& meshBvhNodes = GetMeshGpuBvhNodes();
        const std::vector<float>& triangleData = GetTriangleData();

        stack[currentStackSize++] = instance.rootNodeIndex;
        while (currentStackSize != 0) {
            currentStackSize--;
            const BvhNode& node = meshBvhNodes[stack[currentStackSize]];

            // Test the ray against the node's bounding box
            float t;
            if (!IntersectNodeGpu(rayData, node.boundsMin, node.boundsMax, t)) {
                continue;
            }

            // Intersect primitives
            if (node.primitiveCount > 0) {
                for (uint32_t i = 0; i < node.primitiveCount; i++) {
                    int index = node.firstChildOrPrimitive + i * 12;

                    glm::vec3 p0, e1, e2, normal;
                    p0.x = triangleData[index + 0];
                    p0.y = triangleData[index + 1];
                    p0.z = triangleData[index + 2];
                    e1.x = triangleData[index + 3];
                    e1.y = triangleData[index + 4];
                    e1.z = triangleData[index + 5];
                    e2.x = triangleData[index + 6];
                    e2.y = triangleData[index + 7];
                    e2.z = triangleData[index + 8];
                    normal.x = triangleData[index + 9];
                    normal.y = triangleData[index + 10];
                    normal.z = triangleData[index + 11];

                    glm::vec3 p1 = p0 - e1;
                    glm::vec3 p2 = p0 + e2;

                    // Test ray-triangle intersection
                    float localT;
                    if (IntersectTri(localOrigin, localDir, localMinDistance, localMaxDistance, p0, e1, e2, normal, localT)) {
                        glm::vec3 hitPositionLocal = localOrigin + (localDir * localT);
                        glm::vec3 hitPositionWorld = instance.worldTransform * glm::vec4(hitPositionLocal, 1.0f);
                        float distanceToHit = glm::length(hitPositionWorld - rayOrigin);

                        BvhRayResult rayResult;
                        rayResult.hitFound = true;
                        rayResult.hitPosition = hitPositionWorld;
                        rayResult.distanceToHit = distanceToHit;
                        rayResult.primtiviveId = index;
                        rayResult.primitiveTransform = instance.inverseWorldTransform;
                        rayResult.nodeBoundsMin = node.boundsMin;
                        rayResult.nodeBoundsMax = node.boundsMax;
                        rayResult.openableId = instance.openableId;
                        rayResult.customId = instance.customId;
                        rayResult.globalMeshIndex = instance.globalMeshIndex;
                        Util::UnpackUint64(instance.objectIdLowerBit, instance.objectIdUpperBit, rayResult.objectId);
                        return rayResult;
                    }
                }
            } 
            // The node is internal, so recurse
            else {
                stack[currentStackSize++] = node.firstChildOrPrimitive + 0;
                stack[currentStackSize++] = node.firstChildOrPrimitive + 1;
            }
        }

        return BvhRayResult(); // returns a failed hit
    }

    BvhRayResult AnyHit(uint64_t sceneBvhId, glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDistance) {
        BvhRayResult rayResult;
        rayResult.hitFound = false;
        rayResult.distanceToHit = maxDistance;

        if (Util::IsNan(rayOrigin) || Util::IsNan(rayOrigin) || !SceneBvhExists(sceneBvhId)) return rayResult;

        SceneBvh* sceneBvh = GetSceneBvhById(sceneBvhId);

        const std::vector<BvhNode>& sceneNodes = sceneBvh->m_nodes;
        const std::vector<GpuPrimitiveInstance>& instances = sceneBvh->m_instances;
        const std::vector<BvhNode>& meshBvhNodes = GetMeshGpuBvhNodes();
        
        RayData rayData = ComputeRayData(rayOrigin, rayDir, 0.0001f, maxDistance);
        uint32_t stack[MAX_BVH_STACK_SIZE];
        size_t currentStackSize = 0;
        stack[currentStackSize++] = 0; // start at the root node

        while (currentStackSize != 0) {
            currentStackSize--;
            const BvhNode& node = sceneNodes[stack[currentStackSize]];

            // Test the ray against the node's bounding box
            float t;
            if (!IntersectNodeGpu(rayData, node.boundsMin, node.boundsMax, t)) {
                continue;
            }

            // We have a ray hit! Is it a leaf node?
            if (node.primitiveCount > 0) {

                // It is, so now iterate the instances and check for a ray hit on those
                for (uint32_t i = 0; i < node.primitiveCount; i++) {
                    const GpuPrimitiveInstance& instance = instances[node.firstChildOrPrimitive + i];
                    BvhRayResult localRayResult = MeshAnyHit(instance, rayOrigin, rayDir, maxDistance);
                        
                    if (localRayResult.hitFound) {
                        return localRayResult;
                    }
                }
            }
            // The node is internal, so recurse
            else {
                stack[currentStackSize++] = node.firstChildOrPrimitive + 0;
                stack[currentStackSize++] = node.firstChildOrPrimitive + 1;
            }
        }

        return rayResult;
    }

    BvhRayResult MeshClosestHit(GpuPrimitiveInstance instance, glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDistance) {
        uint32_t stack[MAX_BVH_STACK_SIZE];
        size_t currentStackSize = 0;

        // Calculate world ray in mesh space
        const float globalMinDistance = 0.001f;
        glm::vec3 localOrigin = glm::vec3(instance.inverseWorldTransform * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localEnd = glm::vec3(instance.inverseWorldTransform * glm::vec4(rayOrigin + rayDir * maxDistance, 1.0f));
        glm::vec3 localDir = glm::normalize(localEnd - localOrigin);
        float localMaxDistance = glm::length(localEnd - localOrigin);
        float localMinDistance = globalMinDistance * localMaxDistance / maxDistance;

        RayData rayData = ComputeRayData(localOrigin, localDir, localMinDistance, localMaxDistance);


        BvhRayResult closestRayResult;
        closestRayResult.hitFound = false;
        closestRayResult.distanceToHit = maxDistance;

        const std::vector<BvhNode>& meshBvhNodes = GetMeshGpuBvhNodes();
        const std::vector<float>& triangleData = GetTriangleData();

        // Bail early if the mesh BVHs did not load for some reason
        if (meshBvhNodes.empty() || triangleData.empty()) return closestRayResult;

        // Walk the bvh
        stack[currentStackSize++] = instance.rootNodeIndex;
        while (currentStackSize != 0) {
            currentStackSize--;
            const BvhNode& node = meshBvhNodes[stack[currentStackSize]];

            // Test the ray against the node's bounding box
            float t;
            if (!IntersectNodeGpu(rayData, node.boundsMin, node.boundsMax, t)) {
                continue;
            }

            // Hit was found! Is it a leaf node?
            if (node.primitiveCount > 0) {

                // IF so, then check intersections with the triangles within it
                for (uint32_t i = 0; i < node.primitiveCount; i++) {
                    uint32_t index = node.firstChildOrPrimitive + i * 12;

                    glm::vec3 p0, e1, e2, normal;
                    p0.x = triangleData[index + 0];
                    p0.y = triangleData[index + 1];
                    p0.z = triangleData[index + 2];
                    e1.x = triangleData[index + 3];
                    e1.y = triangleData[index + 4];
                    e1.z = triangleData[index + 5];
                    e2.x = triangleData[index + 6];
                    e2.y = triangleData[index + 7];
                    e2.z = triangleData[index + 8];
                    normal.x = triangleData[index + 9];
                    normal.y = triangleData[index + 10];
                    normal.z = triangleData[index + 11];

                    glm::vec3 p1 = p0 - e1;
                    glm::vec3 p2 = p0 + e2;

                    float localT;
                    if (IntersectTri(localOrigin, localDir, rayData.minDistance, rayData.maxDistance, p0, e1, e2, normal, localT)) {

                        // Transform hit position to world space
                        glm::vec3 hitPositionLocal = localOrigin + (localDir * localT);
                        glm::vec3 hitPositionWorld = instance.worldTransform * glm::vec4(hitPositionLocal, 1.0f);
                        float distanceToHit = glm::length(hitPositionWorld - rayOrigin);

                        if (distanceToHit < closestRayResult.distanceToHit) {
                            closestRayResult.hitFound = true;
                            closestRayResult.hitPosition = hitPositionWorld;
                            closestRayResult.distanceToHit = distanceToHit;
                            closestRayResult.primtiviveId = index;
                            closestRayResult.primitiveTransform = instance.worldTransform;
                            closestRayResult.nodeBoundsMin = node.boundsMin;
                            closestRayResult.nodeBoundsMax = node.boundsMax;
                            closestRayResult.openableId = instance.openableId;
                            closestRayResult.customId = instance.customId;
                            closestRayResult.globalMeshIndex = instance.globalMeshIndex;
                            Util::UnpackUint64(instance.objectIdLowerBit, instance.objectIdUpperBit, closestRayResult.objectId);

                            // Update the ray's max distance to prune further bvh nodes
                            rayData.maxDistance = localT;
                        }
                    }
                }
            }
            // The node is internal, so recurse
            else {
                stack[currentStackSize++] = node.firstChildOrPrimitive + 0;
                stack[currentStackSize++] = node.firstChildOrPrimitive + 1;
            }
        }

        return closestRayResult;
    }
        
    BvhRayResult ClosestHit(uint64_t sceneBvhId, glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDistance) {
        BvhRayResult rayResult;
        rayResult.hitFound = false;
        rayResult.distanceToHit = maxDistance; 

        /// Bail if invalid ray or non-existant scene
        if (Util::IsNan(rayOrigin) || Util::IsNan(rayOrigin) || !SceneBvhExists(sceneBvhId)) return rayResult;

        SceneBvh* sceneBvh = GetSceneBvhById(sceneBvhId);

        const std::vector<BvhNode>& sceneNodes = sceneBvh->m_nodes;
        const std::vector<GpuPrimitiveInstance>& instances = sceneBvh->m_instances;

        // Bail if scene is empty
        if (sceneNodes.empty()) return rayResult;
        
        RayData rayData = ComputeRayData(rayOrigin, rayDir, 0.0001f, maxDistance);
        uint32_t stack[MAX_BVH_STACK_SIZE];
        size_t currentStackSize = 0;
        stack[currentStackSize++] = 0; // start at the root node

        while (currentStackSize != 0) {
            currentStackSize--;
            const BvhNode& node = sceneNodes[stack[currentStackSize]];

            // Test the ray against the node's bounding box
            float t;
            if (!IntersectNodeGpu(rayData, node.boundsMin, node.boundsMax, t)) {
                continue;
            }
            
            // Don't bother checking primitives if the node is further than the closest found already
            if (t >= rayData.maxDistance) {
                continue;
            }

            // We have a ray hit! Is it a leaf node?
            if (node.primitiveCount > 0) {

                // It is, so now iterate the instances and check for a ray hit on those
                for (int i = 0; i < node.primitiveCount; i++) {
                    const GpuPrimitiveInstance& instance = instances[node.firstChildOrPrimitive + i];
                    BvhRayResult localRayResult = MeshClosestHit(instance, rayOrigin, rayDir, rayResult.distanceToHit);
                    
                    if (localRayResult.hitFound && localRayResult.distanceToHit < rayResult.distanceToHit) {
                        rayResult = localRayResult;
                        rayData.maxDistance = rayResult.distanceToHit;
                    }
                }
            }
            // The node is internal, so recurse
            else {
                stack[currentStackSize++] = node.firstChildOrPrimitive + 0;
                stack[currentStackSize++] = node.firstChildOrPrimitive + 1;
            }
        }

        return rayResult;
    }

    void RenderRayResultTriangle(BvhRayResult& rayResult, glm::vec4 color) {
        if (!rayResult.hitFound) return;

        const std::vector<float>& triangleData = GetTriangleData();
        int index = rayResult.primtiviveId;

        if (index + 12 >= triangleData.size()) return;

        glm::vec3 p0, e1, e2, normal;
        p0.x = triangleData[index + 0];
        p0.y = triangleData[index + 1];
        p0.z = triangleData[index + 2];
        e1.x = triangleData[index + 3];
        e1.y = triangleData[index + 4];
        e1.z = triangleData[index + 5];
        e2.x = triangleData[index + 6];
        e2.y = triangleData[index + 7];
        e2.z = triangleData[index + 8];
        normal.x = triangleData[index + 9];
        normal.y = triangleData[index + 10];
        normal.z = triangleData[index + 11];

        glm::vec3 p1 = p0 - e1;
        glm::vec3 p2 = p0 + e2;

        p0 = rayResult.primitiveTransform * glm::vec4(p0, 1.0f);
        p1 = rayResult.primitiveTransform * glm::vec4(p1, 1.0f);
        p2 = rayResult.primitiveTransform * glm::vec4(p2, 1.0f);

        Renderer::DrawPoint(p0, color);
        Renderer::DrawPoint(p1, color);
        Renderer::DrawPoint(p2, color);
        Renderer::DrawLine(p0, p1, color);
        Renderer::DrawLine(p2, p1, color);
        Renderer::DrawLine(p0, p2, color);
    }

    void RenderRayResultNode(BvhRayResult& rayResult, glm::vec4 color) {
        if (!rayResult.hitFound) return;

        AABB aabb(rayResult.nodeBoundsMin, rayResult.nodeBoundsMax);
        Renderer::DrawAABB(aabb, color, rayResult.primitiveTransform);
    }
}