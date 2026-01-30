#version 330 core
layout (location = 0) in vec3 aPos;      // 顶点坐标
layout (location = 1) in vec3 aColor;    // 顶点颜色

out vec3 vertexColor;  // 传给 fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
