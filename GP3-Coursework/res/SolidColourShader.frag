#version 400 core

// Uniforms.
uniform vec3 color;

// Output Colour.
out vec4 FragColor;


void main()
{
    FragColor = vec4(color, 1.0);
}
