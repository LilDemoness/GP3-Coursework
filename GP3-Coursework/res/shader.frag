#version 400 core

// Inputs.
in vec2 texture_coordinate;
in vec3 normal_ws;

// Uniforms.
uniform sampler2D texture_diffused1;

// Output Colour.
out vec4 FragColor;


void main()
{
    vec3 lighting = normalize(normal_ws) * 0.3 + 0.5;
    vec4 albedo = texture(texture_diffused1, texture_coordinate);
    FragColor = albedo * vec4(lighting, 1.0);
}
