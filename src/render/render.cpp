#include "render.h"
#include "coordinate_system.h"
#include <cmath>
#include <imgui.h>
#include <iostream>

// OpenGL错误检查函数
#ifdef DEBUG
void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL ERROR " << err << " at " << fname << ":" << line << " - " << stmt << std::endl;
    }
}
#define GL_CHECK(stmt) do { \
    stmt; \
    checkGLError(#stmt, __FILE__, __LINE__); \
} while(0)
#else
#define GL_CHECK(stmt) stmt
#endif

// 全局渲染资源
unsigned int gridVAO, gridVBO;
unsigned int coordVAO, coordVBO;
unsigned int shaderProgram;
float coordAxisSize = 1.0f;
static size_t gridVertexCount = 0;

// 着色器位置缓存
static GLint modelLocation = -1;
static GLint viewLocation = -1;
static GLint projectionLocation = -1;
static GLint viewPosLocation = -1;
static GLint lightPosLocation = -1;
static GLint lightColorLocation = -1;

extern bool g_isCoordSystemActive;
extern int g_selectedAxis;

// 显示OpenGL版本和设备信息
void printOpenGLInfo() {
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;
}

// 顶点着色器
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aColor;
    
    out vec3 FragPos;
    out vec3 Normal;
    out vec3 Color;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        Color = aColor;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

// 片段着色器
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    in vec3 Color;
    
    out vec4 FragColor;
    
    uniform vec3 lightPos = vec3(2.0, 5.0, 2.0);
    uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
    uniform vec3 viewPos;
    
    void main()
    {
        // 法线
        vec3 norm = normalize(Normal);
        
        // 光照方向
        vec3 lightDir = normalize(lightPos - FragPos);
        
        // 漫反射
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // 镜面反射
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
        vec3 specular = 0.3 * spec * lightColor;
        
        // 环境光
        vec3 ambient = 0.15 * lightColor;
        
        // 菲涅尔效应模拟
        float fresnel = pow(1.0 - dot(norm, viewDir), 2);
        
        // 最终颜色计算
        vec3 lighting = (ambient + diffuse + specular) * Color;
        
        // 添加菲涅尔效果
        vec3 result = mix(lighting, vec3(1.0), fresnel * 0.1);
        
        FragColor = vec4(result, 1.0);
    }
)";

// 设置着色器
void setupShaders()
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GL_CHECK(glCompileShader(vertexShader));
    
    // 检查顶点着色器编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GL_CHECK(glCompileShader(fragmentShader));
    
    // 检查片段着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GL_CHECK(glLinkProgram(shaderProgram));
    
    // 检查着色器程序链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // 初始化uniform位置缓存
    modelLocation = glGetUniformLocation(shaderProgram, "model");
    viewLocation = glGetUniformLocation(shaderProgram, "view");
    projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos");
    lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos");
    lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
}

// 设置网格
void setupGrid()
{
    GL_CHECK(glGenVertexArrays(1, &gridVAO));
    GL_CHECK(glGenBuffers(1, &gridVBO));

    GL_CHECK(glBindVertexArray(gridVAO));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, gridVBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

static float chooseGridStep(float cameraDistance)
{
    float target = cameraDistance / 8.0f;
    if (target < 0.1f) {
        target = 0.1f;
    }

    float power = std::pow(10.0f, std::floor(std::log10(target)));
    float mantissa = target / power;
    float step = 1.0f;
    if (mantissa < 1.5f) {
        step = 1.0f;
    } else if (mantissa < 3.5f) {
        step = 2.0f;
    } else if (mantissa < 7.5f) {
        step = 5.0f;
    } else {
        step = 10.0f;
    }

    return step * power;
}

static float chooseGridExtent(float cameraDistance, float step)
{
    float base = cameraDistance * 2.5f;
    if (base < 10.0f) {
        base = 10.0f;
    }
    float lines = std::ceil(base / step);
    return lines * step;
}

static void buildGridVertices(float extent, float step, const glm::vec3& baseColor,
                              std::vector<Vertex>& out)
{
    int lines = static_cast<int>(extent / step);
    int majorEvery = 5;
    glm::vec3 minorColor = baseColor;
    glm::vec3 majorColor = glm::clamp(baseColor * 1.4f, glm::vec3(0.0f), glm::vec3(1.0f));
    glm::vec3 xAxisColor(0.9f, 0.2f, 0.2f);
    glm::vec3 zAxisColor(0.2f, 0.4f, 0.95f);

    out.clear();
    out.reserve(static_cast<size_t>(lines * 2 + 1) * 4);

    for (int i = -lines; i <= lines; ++i) {
        float pos = i * step;
        bool isMajor = (i % majorEvery) == 0;

        glm::vec3 colorX = isMajor ? majorColor : minorColor;
        glm::vec3 colorZ = isMajor ? majorColor : minorColor;

        if (i == 0) {
            colorX = zAxisColor;
            colorZ = xAxisColor;
        }

        out.push_back(Vertex({pos, 0.0f, -extent}, {0.0f, 1.0f, 0.0f}, colorX));
        out.push_back(Vertex({pos, 0.0f, extent}, {0.0f, 1.0f, 0.0f}, colorX));
        out.push_back(Vertex({-extent, 0.0f, pos}, {0.0f, 1.0f, 0.0f}, colorZ));
        out.push_back(Vertex({extent, 0.0f, pos}, {0.0f, 1.0f, 0.0f}, colorZ));
    }
}


void renderMesh(const Mesh& mesh)
{
    // 使用缓存的VAO/VBO/EBO而不是每次都重新创建
    unsigned int VAO;
    
    // 检查是否已有缓存的VAO，如果没有则创建
    if (mesh.VAO == 0) {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(0);
        
        // 保存到网格中
        const_cast<Mesh&>(mesh).VAO = VAO;
        const_cast<Mesh&>(mesh).VBO = VBO;
        const_cast<Mesh&>(mesh).EBO = EBO;
    } else {
        VAO = mesh.VAO;
        glBindVertexArray(VAO);
    }
    
    // 使用缓存的uniform位置设置模型矩阵
    if (modelLocation != -1) {
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mesh.transform));
    } else {
        modelLocation = glGetUniformLocation(shaderProgram, "model");
        if (modelLocation != -1) {
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mesh.transform));
        }
    }
    
    // 临时修改顶点数据以反映选中状态
    if (mesh.selected) {
        // 创建临时顶点数据，将颜色变淡
        std::vector<Vertex> tempVertices = mesh.vertices;
        for (auto& vertex : tempVertices) {
            // 将颜色混合到白色以达到变淡效果
            vertex.color = vertex.color * 0.5f + glm::vec3(1.0f, 1.0f, 1.0f) * 0.5f;
        }
        
        // 更新VBO数据
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, tempVertices.size() * sizeof(Vertex), tempVertices.data());
    }
    
    // 绘制网格
    if (mesh.indices.size() > 0) {
        glDrawElements(mesh.indices.size() == 1 ? GL_POINTS : 
                      (mesh.indices.size() == 2 ? GL_LINES : GL_TRIANGLES), 
                      mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
    
    // 如果是选中状态，恢复原始顶点数据
    if (mesh.selected) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data());
    }
    
    glBindVertexArray(0);
}

// 设置坐标系
void setupCoordinateSystem()
{
    // 定义坐标轴顶点数据 (起点 -> 终点, 颜色)
    float coordData[] = {
        // X轴 (红色)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 起点
        coordAxisSize, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 终点
        
        // Y轴 (绿色)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 起点
        0.0f, coordAxisSize, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 终点
        
        // Z轴 (蓝色)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // 起点
        0.0f, 0.0f, coordAxisSize, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f   // 终点
    };
    
    glGenVertexArrays(1, &coordVAO);
    glGenBuffers(1, &coordVBO);
    
    glBindVertexArray(coordVAO);
    glBindBuffer(GL_ARRAY_BUFFER, coordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordData), coordData, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 颜色属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

// 渲染坐标系
static bool projectToScreen(const glm::vec3& worldPos, const glm::mat4& view,
                            const glm::mat4& projection, ImVec2& outPos)
{
    glm::vec4 clip = projection * view * glm::vec4(worldPos, 1.0f);
    if (clip.w <= 0.0001f) {
        return false;
    }
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    
    // 统一获取视口信息，避免宏分支差异
    ImVec2 vpPos, vpSize;
    
#ifdef IMGUI_HAS_VIEWPORT
    // 如果支持多视口，则获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (viewport) {
        vpPos = viewport->Pos;
        vpSize = viewport->Size;
    } else {
        // 回退到 DisplaySize
        ImGuiIO& io = ImGui::GetIO();
        vpPos = ImVec2(0.0f, 0.0f);
        vpSize = ImVec2(io.DisplaySize.x, io.DisplaySize.y);
    }
#else
    // 在较旧版本的ImGui中，使用IO结构体获取显示尺寸
    ImGuiIO& io = ImGui::GetIO();
    vpPos = ImVec2(0.0f, 0.0f);
    vpSize = ImVec2(io.DisplaySize.x, io.DisplaySize.y);
#endif
    
    outPos.x = vpPos.x + (ndc.x * 0.5f + 0.5f) * vpSize.x;
    outPos.y = vpPos.y + (0.5f - ndc.y * 0.5f) * vpSize.y;
    return true;
}

static float distancePointToSegment2D(const ImVec2& p, const ImVec2& a, const ImVec2& b)
{
    ImVec2 ab(b.x - a.x, b.y - a.y);
    float denom = ab.x * ab.x + ab.y * ab.y;
    if (denom <= 0.0001f) {
        float dx = p.x - a.x;
        float dy = p.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    float t = ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / denom;
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    ImVec2 proj(a.x + ab.x * t, a.y + ab.y * t);
    float dx = p.x - proj.x;
    float dy = p.y - proj.y;
    return std::sqrt(dx * dx + dy * dy);
}

static int getHoveredAxis(const glm::mat4& view, const glm::mat4& projection, float axisLength)
{
    ImVec2 origin;
    ImVec2 xEnd;
    ImVec2 yEnd;
    ImVec2 zEnd;

    if (!projectToScreen(glm::vec3(0.0f, 0.0f, 0.0f), view, projection, origin)) {
        return -1;
    }
    if (!projectToScreen(glm::vec3(axisLength, 0.0f, 0.0f), view, projection, xEnd)) {
        return -1;
    }
    if (!projectToScreen(glm::vec3(0.0f, axisLength, 0.0f), view, projection, yEnd)) {
        return -1;
    }
    if (!projectToScreen(glm::vec3(0.0f, 0.0f, axisLength), view, projection, zEnd)) {
        return -1;
    }

    ImVec2 mouse = ImGui::GetIO().MousePos;
    float distX = distancePointToSegment2D(mouse, origin, xEnd);
    float distY = distancePointToSegment2D(mouse, origin, yEnd);
    float distZ = distancePointToSegment2D(mouse, origin, zEnd);

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

    float threshold = 8.0f;
    return (minDist <= threshold) ? axis : -1;
}

void renderCoordinateSystem(const glm::mat4& view, const glm::mat4& projection, float cameraDistance)
{
    glUseProgram(shaderProgram);

    float axisLength = getCoordinateAxisLength(cameraDistance);
    int hoveredAxis = getHoveredAxis(view, projection, axisLength);
    int activeAxis = g_isCoordSystemActive ? g_selectedAxis : hoveredAxis;

    glm::vec3 xColor(0.9f, 0.2f, 0.2f);
    glm::vec3 yColor(0.2f, 0.9f, 0.2f);
    glm::vec3 zColor(0.2f, 0.4f, 0.95f);
    glm::vec3 highlight(1.0f, 0.85f, 0.2f);

    if (activeAxis == 0) {
        xColor = highlight;
    } else if (activeAxis == 1) {
        yColor = highlight;
    } else if (activeAxis == 2) {
        zColor = highlight;
    }

    float coordData[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, xColor.r, xColor.g, xColor.b,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, xColor.r, xColor.g, xColor.b,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, yColor.r, yColor.g, yColor.b,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, yColor.r, yColor.g, yColor.b,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, zColor.r, zColor.g, zColor.b,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, zColor.r, zColor.g, zColor.b
    };

    glBindBuffer(GL_ARRAY_BUFFER, coordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordData), coordData, GL_DYNAMIC_DRAW);

    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(axisLength));

    // 使用缓存的uniform位置设置变换矩阵
    if (modelLocation != -1) {
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    } else {
        modelLocation = glGetUniformLocation(shaderProgram, "model");
        if (modelLocation != -1) {
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        }
    }
    if (viewLocation != -1) {
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    } else {
        viewLocation = glGetUniformLocation(shaderProgram, "view");
        if (viewLocation != -1) {
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        }
    }
    if (projectionLocation != -1) {
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    } else {
        projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        if (projectionLocation != -1) {
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        }
    }

    glBindVertexArray(coordVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

#ifdef IMGUI_HAS_DOCK
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 xEnd;
    ImVec2 yEnd;
    ImVec2 zEnd;
    if (projectToScreen(glm::vec3(axisLength, 0.0f, 0.0f), view, projection, xEnd)) {
        drawList->AddText(ImVec2(xEnd.x + 4.0f, xEnd.y), ImGui::ColorConvertFloat4ToU32(ImVec4(xColor.r, xColor.g, xColor.b, 1.0f)), "X");
    }
    if (projectToScreen(glm::vec3(0.0f, axisLength, 0.0f), view, projection, yEnd)) {
        drawList->AddText(ImVec2(yEnd.x + 4.0f, yEnd.y), ImGui::ColorConvertFloat4ToU32(ImVec4(yColor.r, yColor.g, yColor.b, 1.0f)), "Y");
    }
    if (projectToScreen(glm::vec3(0.0f, 0.0f, axisLength), view, projection, zEnd)) {
        drawList->AddText(ImVec2(zEnd.x + 4.0f, zEnd.y), ImGui::ColorConvertFloat4ToU32(ImVec4(zColor.r, zColor.g, zColor.b, 1.0f)), "Z");
    }
#else
    // 不绘制坐标轴标签，因为当前版本的Dear ImGui不支持这些API
#endif
}


void renderGrid(float cameraDistance, const glm::vec3& gridColor)
{
    glUseProgram(shaderProgram);

    static float lastExtent = -1.0f;
    static float lastStep = -1.0f;
    static glm::vec3 lastColor(-1.0f, -1.0f, -1.0f);

    float step = chooseGridStep(cameraDistance);
    float extent = chooseGridExtent(cameraDistance, step);

    bool colorChanged = gridColor.x != lastColor.x || gridColor.y != lastColor.y || gridColor.z != lastColor.z;
    if (std::fabs(extent - lastExtent) > 0.001f || std::fabs(step - lastStep) > 0.0001f || colorChanged) {
        std::vector<Vertex> gridVertices;
        buildGridVertices(extent, step, gridColor, gridVertices);

        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(Vertex), gridVertices.data(), GL_DYNAMIC_DRAW);
        gridVertexCount = gridVertices.size();

        lastExtent = extent;
        lastStep = step;
        lastColor = gridColor;
    }

    // 使用缓存的uniform位置设置模型矩阵
    if (modelLocation != -1) {
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    } else {
        modelLocation = glGetUniformLocation(shaderProgram, "model");
        if (modelLocation != -1) {
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        }
    }
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertexCount));
    glBindVertexArray(0);
}
