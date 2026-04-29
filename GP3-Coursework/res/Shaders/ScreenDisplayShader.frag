#version 400 core

in VertexData
{
    vec2 texture_coordinate;
} v_in;

uniform sampler2D screen_texture;

out vec4 FragColor;


void main()
{
    vec3 color = texture(screen_texture, v_in.texture_coordinate).rgb;
    FragColor = vec4(color, 1.0);
}