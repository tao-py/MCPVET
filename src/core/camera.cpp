#include "camera.h"
#include <cmath>

void updateCameraFromState(const glm::vec2& cameraAngles,
                           float cameraDistance,
                           const glm::vec3& cameraTarget,
                           glm::vec3& outCameraPosition,
                           glm::mat4& outViewMatrix)
{
    const float yaw = cameraAngles.x;
    const float pitch = cameraAngles.y;

    glm::vec3 offset;
    offset.x = cameraDistance * std::cos(pitch) * std::sin(yaw);
    offset.y = cameraDistance * std::sin(pitch);
    offset.z = cameraDistance * std::cos(pitch) * std::cos(yaw);

    outCameraPosition = cameraTarget + offset;
    outViewMatrix = glm::lookAt(outCameraPosition,
                                cameraTarget,
                                glm::vec3(0.0f, 1.0f, 0.0f));
}
