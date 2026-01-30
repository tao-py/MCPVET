#include "geometry_model.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transform::toMatrix() const {
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, translation);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, scale);
    return matrix;
}
