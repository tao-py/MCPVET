#version 330 core
in vec3 vertexColor;    // 从顶点着色器来的颜色
out vec4 FragColor;     // 输出到屏幕的最终颜色

void main() {
    FragColor = vec4(vertexColor, 1.0); // 显示颜色
}
