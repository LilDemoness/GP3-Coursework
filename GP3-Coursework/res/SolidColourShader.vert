#version 400 core

layout(location = 0) in vec3 VertexPosition;

layout(std140) uniform Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
};


void main() {
    gl_Position = (projection * view * model) * vec4(VertexPosition, 1.0);
}
