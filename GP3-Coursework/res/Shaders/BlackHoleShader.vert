#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TextureCoordinate;
layout(location = 3) in mat4 InstanceMatrix;    // Technically has locations 3-6 due to mat4 effectively being 4 vec4s in size.

layout(std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
};


out VertexData
{
    vec3 position_ws;
    vec4 position_cs;

    vec3 normal_ws;
    vec2 texture_coordinate;
} v_out;


void main()
{
    gl_Position = (projection * view * InstanceMatrix) * vec4(VertexPosition, 1.0);

    v_out.position_ws = (InstanceMatrix * vec4(VertexPosition, 1.0)).xyz;
    v_out.position_cs = gl_Position;
    v_out.normal_ws = VertexNormal;
    v_out.texture_coordinate = TextureCoordinate;
}