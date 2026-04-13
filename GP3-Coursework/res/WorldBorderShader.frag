#version 400 core

// Uniforms.
uniform vec3 player_position_ws;
uniform float world_radius = 25.0f;

// Settings.
const float FADE_START_DISTANCE = 5.0f;
const float MAX_ALPHA = 0.8f;

// Output Colour.
out vec4 FragColor;


void main()
{
    float distance_to_edge = max(world_radius - length(player_position_ws), 0);
    float alpha = MAX_ALPHA * (FADE_START_DISTANCE - distance_to_edge) / FADE_START_DISTANCE;
    FragColor = vec4(0.0f, 0.0f, 0.0f, alpha);
}
