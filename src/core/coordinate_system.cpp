#include "coordinate_system.h"

// 全局变量定义
bool g_isCoordSystemActive = false;
int g_selectedAxis = -1; // -1: none, 0: X, 1: Y, 2: Z

// 检测坐标系轴的选择
float getCoordinateAxisLength(float cameraDistance) {
    float length = cameraDistance * 0.2f;
    if (length < 0.5f) {
        length = 0.5f;
    } else if (length > 5.0f) {
        length = 5.0f;
    }
    return length;
}

static bool projectToNdc(const glm::vec3& worldPos, const glm::mat4& view,
                         const glm::mat4& projection, glm::vec2& outNdc) {
    glm::vec4 clip = projection * view * glm::vec4(worldPos, 1.0f);
    if (clip.w <= 0.0001f) {
        return false;
    }
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    outNdc = glm::vec2(ndc.x, ndc.y);
    return true;
}

static float distancePointToSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) {
    glm::vec2 ab = b - a;
    float denom = glm::dot(ab, ab);
    if (denom <= 0.000001f) {
        return glm::length(p - a);
    }
    float t = glm::dot(p - a, ab) / denom;
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    glm::vec2 proj = a + ab * t;
    return glm::length(p - proj);
}

bool checkCoordinateAxisSelection(double mouseX, double mouseY, int windowWidth, int windowHeight,
                                  const glm::mat4& view, const glm::mat4& projection, float cameraDistance) {
    float ndcX = (2.0f * static_cast<float>(mouseX)) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * static_cast<float>(mouseY)) / windowHeight;

    float axisLength = getCoordinateAxisLength(cameraDistance);

    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 xEnd(axisLength, 0.0f, 0.0f);
    glm::vec3 yEnd(0.0f, axisLength, 0.0f);
    glm::vec3 zEnd(0.0f, 0.0f, axisLength);

    glm::vec2 originNdc;
    glm::vec2 xEndNdc;
    glm::vec2 yEndNdc;
    glm::vec2 zEndNdc;

    if (!projectToNdc(origin, view, projection, originNdc)) {
        return false;
    }
    if (!projectToNdc(xEnd, view, projection, xEndNdc) ||
        !projectToNdc(yEnd, view, projection, yEndNdc) ||
        !projectToNdc(zEnd, view, projection, zEndNdc)) {
        return false;
    }

    glm::vec2 mouseNdc(ndcX, ndcY);
    float threshold = 0.03f;

    float distX = distancePointToSegment(mouseNdc, originNdc, xEndNdc);
    float distY = distancePointToSegment(mouseNdc, originNdc, yEndNdc);
    float distZ = distancePointToSegment(mouseNdc, originNdc, zEndNdc);

    float minDist = distX;
    int axis = 0;
    if (distY < minDist) {
        minDist = distY;
        axis = 1;
    }
    if (distZ < minDist) {
        minDist = distZ;
        axis = 2;
    }

    if (minDist < threshold) {
        g_selectedAxis = axis;
        g_isCoordSystemActive = true;
        return true;
    }

    return false;
}

void rotateSceneAroundAxis(float deltaX, float deltaY, glm::vec2& cameraAngles) {
    if (!g_isCoordSystemActive || g_selectedAxis == -1) return;
    
    float rotationSpeed = 0.01f;
    
    switch(g_selectedAxis) {
        case 0: // X轴
            cameraAngles.y -= deltaY * rotationSpeed;
            break;
        case 1: // Y轴
            cameraAngles.x += deltaX * rotationSpeed;
            break;
        case 2: // Z轴
            // 对于Z轴，我们可以改变相机的旋转角度
            cameraAngles.x -= deltaX * rotationSpeed;
            cameraAngles.y -= deltaY * rotationSpeed;
            break;
    }
    
    // 确保角度在合理范围内
    cameraAngles.x = fmod(cameraAngles.x, glm::two_pi<float>());
    cameraAngles.y = fmod(cameraAngles.y, glm::two_pi<float>());
}