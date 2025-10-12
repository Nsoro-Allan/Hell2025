#pragma once
#include "Physics/Physics.h"
#include "HellTypes.h"
#include "Types.h"

namespace RagdollUtil {

    inline RdVector toPhysicalScale(const RdVector value, RdScalar scale) {
        return value * scale;
    }

    inline RdQuaternion toQuaternion(const RdEulerRotation& euler) {
        // XYZ rotation order
        return RdQuaternion::rotation((RdRadians)euler.z(), RdVector::zAxis()) *
               RdQuaternion::rotation((RdRadians)euler.y(), RdVector::yAxis()) *
               RdQuaternion::rotation((RdRadians)euler.x(), RdVector::xAxis());
    }

    inline PxQuat toPxQuat(RdEulerRotation euler) {
        const RdQuaternion quat = toQuaternion(euler);
        const RdVector vec = quat.vector();

        return PxQuat(
            static_cast<float>(vec.x()),
            static_cast<float>(vec.y()),
            static_cast<float>(vec.z()),
            static_cast<float>(quat.scalar())
        );
    }

    inline PxVec3 toPxVec3(RdPoint point) {
        return PxVec3(
            static_cast<float>(point.x()),
            static_cast<float>(point.y()),
            static_cast<float>(point.z())
        );
    }


    inline PxShape* CreateShape(RagdollMarker& marker, RagdollSolver& solver) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxShape* pxShape = nullptr;

        PxMaterial* material = pxPhysics->createMaterial(marker.friction, marker.friction, marker.restitution );
        if (marker.contactStiffness > 0) {
            material->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
            // TODO requires newer PhysX: material->setFlag(PxMaterialFlag::eCOMPLIANT_ACCELERATION_SPRING, true);
            auto stiffness = marker.contactStiffness * 1e4f;
            auto damping = marker.contactDamping * 0.01f;
            material->setRestitution(-stiffness);
            material->setDamping(stiffness * damping);
        }

        RdGeometryDescriptionComponent desc = marker.geometryDescriptionComponent;
        RdScaleComponent scale = marker.scaleComponent;
        float sceneScale = solver.sceneScale;

        if (sceneScale != 1.0f) {
            desc.length *= sceneScale;
            desc.radius *= sceneScale;
            desc.radiusEnd *= sceneScale;
            desc.extents *= sceneScale;
        }

        // Box
        if (desc.type == RdGeometryType::kBox) {
            const auto geometry = PxBoxGeometry{
                std::max(0.001f, static_cast<float>(desc.extents.x() * scale.absolute.x()) * 0.5f),
                std::max(0.001f, static_cast<float>(desc.extents.y() * scale.absolute.y()) * 0.5f),
                std::max(0.001f, static_cast<float>(desc.extents.z() * scale.absolute.z()) * 0.5f)
            };
            pxShape = pxPhysics->createShape(geometry, *material);
        }

        // Sphere
        if (desc.type == RdGeometryType::kSphere) {
            const double radiusScale = scale.absolute.x();
            const auto geometry = PxSphereGeometry{ static_cast<float>(desc.radius * radiusScale) };
            pxShape = pxPhysics->createShape(geometry, *material);
        }

        // Capsule
        if (desc.type == RdGeometryType::kCapsule) {
            RdVector absScale = scale.absolute;

            if (desc.capsuleLengthAlongY) {
                absScale.z() = absScale.x();
                absScale.x() = absScale.y();
                absScale.y() = absScale.z();
            }

            const float halfHeight = desc.length * 0.5f * static_cast<float>(absScale.x());
            const float radius = desc.radius * static_cast<float>(absScale.y());

            PxCapsuleGeometry geometry{ radius, halfHeight };
            pxShape = pxPhysics->createShape(geometry, *material);
        }

        // Convex Mesh
        else if (desc.type == RdGeometryType::kConvexHull) {
            std::vector<Vertex> vertices;
            vertices.reserve(marker.convexMeshVertices.size());
            for (RdPoint& point : marker.convexMeshVertices) {
                Vertex& vertex = vertices.emplace_back();
                vertex.position.x = point.x();
                vertex.position.y = point.y();
                vertex.position.z = point.z();
            }
            std::span<Vertex> vertexSpan(vertices.data(), vertices.size());
            pxShape = Physics::CreateConvexShapeFromVertexList(vertexSpan);
            PxMaterial* materials[] = { material };
            pxShape->setMaterials(materials, 1);

            PxGeometryHolder gh = pxShape->getGeometry();
            PxConvexMeshGeometry g = gh.convexMesh();
            g.scale = PxMeshScale(sceneScale);
            pxShape->setGeometry(g);
        }

        if (desc.type != RdGeometryType::kConvexHull) {
            const RdVector localOffset = desc.offset * scale.value;
            const PxTransform pxtm{ toPxVec3(toPhysicalScale(localOffset, sceneScale)), toPxQuat(desc.rotation) };
            pxShape->setLocalPose(pxShape->getLocalPose().transform(pxtm));
        }
        return pxShape;
    }

}