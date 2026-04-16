#version 400 core

// Inputs.
in VertexData
{
    vec3 normal_ws;
    vec2 texture_coordinate;
} v_in;

// Uniforms.
uniform sampler2D texture_diffused1;

// Output Colour.
out vec4 FragColor;


void main()
{
    vec3 lighting = normalize(v_in.normal_ws) * 0.3 + 0.5;
    vec4 albedo = texture(texture_diffused1, v_in.texture_coordinate);
    FragColor = albedo * vec4(lighting, 1.0);
}
