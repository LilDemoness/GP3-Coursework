#version 400 core

// Uniforms.
uniform vec3 color;
uniform float alpha = 1.0f;

// Output Colour.
out vec4 FragColor;


void main()
{
    FragColor = vec4(color, alpha);
}
