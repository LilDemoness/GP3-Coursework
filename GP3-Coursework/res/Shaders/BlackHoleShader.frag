#version 400 core

// Constants.
const float max_float = 999999;

// Inputs.
in VertexData
{
    vec3 position_ws;
    vec4 position_cs;

    vec3 normal_ws;
    vec2 texture_coordinate;
} v_in;

// Uniforms.
uniform sampler2D screen_texture;
uniform vec3 camera_pos_ws;

uniform vec3 black_hole_centre;
uniform float black_hole_radius;

uniform vec2 screen_size;

// Output Colour.
out vec4 FragColor;

// Function Predeclerations.
vec2 intersect_sphere(vec3 ray_origin, vec3 ray_dir, vec3 centre, float radius);


void main()
{
    vec3 ray_dir = normalize(v_in.position_ws - camera_pos_ws);
    vec2 outer_sphere_intersection = intersect_sphere(camera_pos_ws, ray_dir, black_hole_centre, black_hole_radius);

    // Raymarching information.
    float transmittance = 0;

    // Ray intersects with the outer sphere.
    if (outer_sphere_intersection.x < max_float)
    {
        transmittance = 1;
    }

    vec2 screen_uv = (v_in.position_cs.xy / v_in.position_cs.w) * 0.5 + 0.5;
    vec3 background_color = texture(screen_texture, screen_uv).rgb;

    vec3 color = mix(background_color, vec3(1.0, 0.0, 0.0), transmittance);
    FragColor = vec4(color, 1.0);
}


// ----- Raycast Functions -----
// Based upon https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/#:~:text=When%20the%20ray%20and%20sphere,equations%20and%20solving%20for%20t.
// Returns dstToSphere, dstThroughSphere
vec2 intersect_sphere(vec3 ray_origin, vec3 ray_dir, vec3 centre, float radius)
{
    vec3 offset = ray_origin - centre;
    const float a = 1.0;
    float b = 2 * dot(offset, ray_dir);
    float c = dot(offset, offset) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0)
    {
        // Discriminant > 0 = 2 Intersections = Intersection.
        float sqrt_d = sqrt(discriminant);
        float dst_to_sphere_near = max(0, (-b, -sqrt_d) / (2 * a));
        float dst_to_sphere_far = (-b + sqrt_d) / (2 * a);

        if (dst_to_sphere_far >= 0.0)
        {
            return vec2(dst_to_sphere_near, dst_to_sphere_far - dst_to_sphere_near);
        }
    }
    // Discriminant < 0: No Intersections || Discriminant == 0: 1 Intersection (Glancing).
    return vec2(max_float);
}