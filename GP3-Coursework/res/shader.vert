#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TextureCoordinate;

layout(std140) uniform Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
};

out vec3 normal_ws;
out vec2 texture_coordinate;


void main() {
    texture_coordinate = TextureCoordinate;
    normal_ws = VertexNormal;

    gl_Position = (projection * view * model) * vec4(VertexPosition, 1.0);
}
