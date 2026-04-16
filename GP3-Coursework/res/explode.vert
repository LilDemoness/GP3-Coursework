#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TextureCoordinate;
layout(location = 3) in mat4 InstanceMatrix;    // Technically has locations 3-6 due to mat4 effectively being 4 vec4s in size.

out VertexData
{
    vec3 normal_ws;
    vec2 texture_coordinate;
} v_out;


void main() {
    v_out.texture_coordinate = TextureCoordinate;
    v_out.normal_ws = VertexNormal;

    // Leave the position in World Space to allow for proper calculations in the geometry shader.
    // The geometry shader will then convert gl_Position to clip space.
    gl_Position = InstanceMatrix * vec4(VertexPosition, 1.0);
}
