#ifndef GEOMETRY_MODEL_H
#define GEOMETRY_MODEL_H

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct MaterialInfo {
    std::string name;
    double density = 0.0;
};

struct SourceInfo {
    std::string name;
    glm::vec3 position{0.0f};
};

struct DetectorInfo {
    std::string name;
    int id = -1;
};

enum class PrimitiveType {
    Sphere,
    Cylinder,
    Box,
    Plane,
    Cone,
    Unknown
};

enum class BooleanOperation {
    Union,
    Intersection,
    Difference
};

struct Transform {
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 toMatrix() const;
};

struct GeometryNode {
    int id = -1;
    std::string label;
    PrimitiveType primitive = PrimitiveType::Unknown;
    std::optional<BooleanOperation> booleanOp;
    Transform transform;
    std::optional<MaterialInfo> material;
    std::optional<SourceInfo> source;
    std::optional<DetectorInfo> detector;
    std::vector<std::shared_ptr<GeometryNode>> children;
};

#endif // GEOMETRY_MODEL_H
