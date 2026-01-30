#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void updateCameraFromState(const glm::vec2& cameraAngles,
                           float cameraDistance,
                           const glm::vec3& cameraTarget,
                           glm::vec3& outCameraPosition,
                           glm::mat4& outViewMatrix);

#endif // CAMERA_H
