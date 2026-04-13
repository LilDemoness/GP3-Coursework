#version 400 core
in vec2 texture_coordinate;

uniform sampler2D text;
uniform vec3 text_color;

out vec4 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texture_coordinate).r);
    if (sampled.a < 0.3f)
        discard;
    color = vec4(text_color, 1.0) * sampled;
} 