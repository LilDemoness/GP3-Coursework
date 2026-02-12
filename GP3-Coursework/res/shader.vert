#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TextureCoordinate;

uniform mat4 transform;

out vec3 normal_ws;
out vec2 texture_coordinate;


void main() {
    texture_coordinate = TextureCoordinate;
    normal_ws = VertexNormal;

    gl_Position = transform * vec4(VertexPosition, 1.0);
}
