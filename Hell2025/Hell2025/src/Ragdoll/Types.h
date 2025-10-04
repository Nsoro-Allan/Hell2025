#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>
#include "magnum/MagnumMath.hpp"

using RdMatrix3 = Magnum::Math::Matrix3<double>;
using RdMatrix = Magnum::Math::Matrix4<double>;
using RdQuaternion = Magnum::Math::Quaternion<double>;
using RdVector2f = Magnum::Math::Vector2<float>;
using RdVector2i = Magnum::Math::Vector2<int>;
using RdVector = Magnum::Math::Vector3<double>;
using RdVectorF = Magnum::Math::Vector3<float>;
using RdVector2 = Magnum::Math::Vector2<double>;
using RdVector2F = Magnum::Math::Vector2<float>;
using RdVector4 = Magnum::Math::Vector4<double>;
using RdVector4F = Magnum::Math::Vector4<float>;
using RdDegrees = Magnum::Math::Deg<double>;
using RdDegreesF = Magnum::Math::Deg<float>;
using RdRadians = Magnum::Math::Rad<double>;
using RdRadiansF = Magnum::Math::Rad<float>;
using RdColorHsv = Magnum::Math::ColorHsv<float>;
using RdColorHex = unsigned int;
using RdPlane = Magnum::Math::Vector4<double>;
using RdBoolean = bool;
using RdInteger = int;
using RdUint = unsigned int;
using RdPixels = int;
using RdEnum = short;
using RdTime = double;
using RdIndex = std::size_t;
using RdSize = std::size_t;
using RdFrame = int;
using RdSeconds = double;
using RdFloat3 = float[3];
using RdString = std::string;
using RdPath = std::filesystem::path;
using RdEntityId = unsigned;
using RdScalar = double;
using RdLinear = double;
using RdNode = std::size_t;
using RdEdge = std::pair<RdNode, RdNode>;
using RdNodes = std::vector<RdNode>;
using RdEdges = std::vector<RdEdge>;
using RdPoint = RdVector;
using RdReference = void*;

inline auto RdIdentityInit = Magnum::Math::IdentityInit;

enum class RdBehaviour : short {
    kInherit = 0,
    kKinematic = 2,
    kDynamic = 3,
};

enum RdStatus : RdEnum {
    kSuccess = 0,
    kFailure,
    kInvalid,
    kDisabled,
    kUninitialised,
    kIgnored,
    kSceneError,
    kClean,
};

struct RdEulerRotation : public RdVector {
    enum RotateOrder : RdEnum {
        eXYZ = 0,
        eOther
    } rotateOrder{ eXYZ };

    using RdVector::RdVector;

    /**
     * @brief Return the rotation as a quaternion
     *
     */
    RdQuaternion asQuaternion() const {
        //assert(rotateOrder == eXYZ && "Unsupported rotate order");
        return RdQuaternion::rotation((RdRadians)this->z(), RdVector::zAxis()) *
            RdQuaternion::rotation((RdRadians)this->y(), RdVector::yAxis()) *
            RdQuaternion::rotation((RdRadians)this->x(), RdVector::xAxis());
    }

    /**
     * @brief Return rotation as an RdMatrix
     *
     */
    RdMatrix asMatrix() const {
        return RdMatrix(this->asQuaternion().toMatrix());
    }
};

// Alias
using RdEuler = RdEulerRotation;

class RdColor : public Magnum::Math::Color4<float> {
public:
    // Inherit the base Color4<float> constructors
    using Magnum::Math::Color4<float>::Color4;
    static RdColor fromHex(RdColorHex color) {
        static const float s = 1.0f / 255.0f;
        return {
            ((color >> 0) & 0xFF) * s,
            ((color >> 8) & 0xFF) * s,
            ((color >> 16) & 0xFF) * s,
            ((color >> 24) & 0xFF) * s
        };
    }

    static RdColorHex toHex(RdColor color) {
        // Ensure the values are within the range [0.0, 1.0]
        auto rf = std::min(1.0f, std::max(0.0f, color.r()));
        auto gf = std::min(1.0f, std::max(0.0f, color.g()));
        auto bf = std::min(1.0f, std::max(0.0f, color.b()));
        auto af = std::min(1.0f, std::max(0.0f, color.a()));

        // Convert to the range [0, 255]
        uint8_t r = static_cast<uint8_t>(rf * 255.0f);
        uint8_t g = static_cast<uint8_t>(gf * 255.0f);
        uint8_t b = static_cast<uint8_t>(bf * 255.0f);
        uint8_t a = static_cast<uint8_t>(af * 255.0f);

        RdColorHex out = (a << 24) | (b << 16) | (g << 8) | r;
        return out;
    }

    RdColor operator*(const float other) const {
        return _multHelper(other);
    }

    RdColor operator*(const RdScalar other) const {
        return *this * static_cast<float>(other);
    }

    RdColor& operator*=(const float other) {
        *this = _multHelper(other);
        return *this;
    }

    RdColor& operator*=(const RdScalar other) {
        *this *= static_cast<float>(other);
        return *this;
    }

private:
    inline RdColor _multHelper(float factor) const {
        const auto [hue, sat, val] = toHsv();
        const RdColorHsv hsv{ hue, sat, std::min(1.0f, val * factor) };
        return RdColor::fromHsv(hsv);
    }
};

inline RdEulerRotation toEulerRotation(const RdQuaternion& quat) {
    const auto euler = quat.toEuler();
    return RdVector(
        static_cast<RdScalar>(euler.x()),
        static_cast<RdScalar>(euler.y()),
        static_cast<RdScalar>(euler.z())
    );
}

inline RdEulerRotation RdToEulerRotation(const RdQuaternion& quat) { return toEulerRotation(quat); }

#include <type_traits>

template<typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator~(E e) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(e));
}
template<typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator|(E a, E b) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) | static_cast<U>(b));
}
template<typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator&(E a, E b) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) & static_cast<U>(b));
}
template<typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator|=(E& a, E b) { return a = (a | b); }
template<typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator&=(E& a, E b) { return a = (a & b); }

template<typename T>
void setBit(T& value, T bit, bool state = true) {
    if (state) value = static_cast<T>(value | bit);
    else value = static_cast<T>(value & (~bit));
}

template<typename T>
bool hasBit(const T value, const T bit) {
    return (static_cast<int>(value) & static_cast<int>(bit)) != 0;
}

using RdPoints = std::vector<RdPoint>;
using RdVectors = std::vector<RdVector>;
using RdIntegers = std::vector<RdInteger>;
using RdIndices = std::vector<RdIndex>;
using RdColors = std::vector<RdColor>;
using RdFrames = std::vector<RdFrame>;
using RdUints = std::vector<RdUint>;
using RdPaths = std::vector<RdPath>;
using RdStrings = std::vector<RdString>;
using RdPlanes = std::vector<RdPlane>;
using RdInts = RdIntegers; // Shorthand, unused?

struct RdMesh {
    RdPoints      vertices;
    RdUints       indices;
    RdVectors     normals;
    RdColors      colors;

    // A unqiue and persistent ID for each index,
    // used for skinning whereby existing vertices retain
    // an original ID when other vertices change. Unlike
    // the `indices` vector which is more or less random
    RdIntegers    vertexIds;
};

constexpr float FLT_MAX_{ 1e+37f };
constexpr int   INT_MAX_{ 2147483647 };

enum RdMotion : RdEnum {
    RdMotionInherit = -1,
    RdMotionLocked,
    RdMotionLimited,
    RdMotionFree
};

enum RdGeometryType : short {
    kBox = 0,
    kSphere = 1,
    kCapsule = 2,
    kCylinder = 3,
    kConvexHull = 4,
    kMesh = 4,        // Alias for convex hull
    kPlane = 5,
    kTaperedCapsule = 6,
    kCSphere = 7,
    kLast
};

struct RdBoundingBox {
    RdPoint minPos{ 0, 0, 0 };
    RdPoint maxPos{ 0, 0, 0 };

    bool operator==(const RdBoundingBox& other) const {
        return (minPos == other.minPos) && (maxPos == other.maxPos);
    }

    bool operator!=(const RdBoundingBox& other) const {
        return !(*this == other);
    }
};

inline RdMotion StringToMotion(const RdString& s) {
    if (s == "Inherit") return RdMotionInherit;
    if (s == "Locked")  return RdMotionLocked;
    if (s == "Limited") return RdMotionLimited;
    if (s == "Free")    return RdMotionFree;
    // sensible default for ragdolls
    return RdMotionLocked;
}
