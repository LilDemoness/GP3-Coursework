#version 400 core

layout (location = 0) in vec3 VertexPosition;  

layout(std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
};

out vec3 texture_direction;

void main()
{
	vec4 pos = projection * view * vec4(VertexPosition, 1.0);
	gl_Position = pos.xyww;
	texture_direction = VertexPosition;
}