#version 400 core

// Constants.
const float max_float = 999999.0;
const float PI = 3.1415926538;

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
uniform sampler2D disc_texture;
uniform vec3 camera_pos_ws;
uniform vec3 disc_normal;

uniform vec3 black_hole_centre;
uniform float black_hole_radius;


uniform float disc_width = 0.01;
uniform float disc_outer_radius = 0.75;
uniform float disc_inner_radius = 0.25;
uniform float disc_rotation_speed = 2;

uniform vec3 disc_color = vec3(0.75, 0.188, 0.0);
uniform float doppler_beaming_factor = 66.0;
uniform float hue_radius = 0.75;
uniform float hue_shift_factor = -0.03;


uniform vec2 screen_size;

uniform float time;


// Output Colour.
out vec4 FragColor;

// Function Predeclerations.
vec2 intersect_sphere(vec3 ray_origin, vec3 ray_dir, vec3 centre, float radius);
vec2 intersect_infinite_cylinder(vec3 ray_origin, vec3 ray_dir, vec3 cylinder_origin, vec3 cylinder_dir, float cylinder_radius);
float intersect_infinite_plane(vec3 ray_origin, vec3 ray_dir, vec3 plane_origin, vec3 plane_normal);
float intersect_disc(vec3 ray_origin, vec3 ray_dir, vec3 bottom_cap_pos, vec3 top_cap_pos, vec3 disc_normal, float disc_radius, float inner_radius);

float remap(float v, float min_old, float max_old, float min_new, float max_new);
vec2 disc_uv(vec3 planar_disc_pos, vec3 disc_normal, vec3 disc_centre, float radius);
vec3 calculate_disc_color(vec3 base_color, vec3 planar_disc_pos, vec3 disc_normal, vec3 camera_pos, float u, float radius);

vec3 linear_to_gamma_space(vec3 linear_rgb);
vec3 gamma_to_linear_space(vec3 s_rgb);
vec3 hdr_intensity(vec3 emmissive_color, float intensity);
vec3 rgb_to_hsv(vec3 color);
vec3 hsv_to_rgb(vec3 color);
vec3 rotate_about_axis(vec3 in_pos, vec3 axis, float rotation);


void main()
{
    vec3 ray_origin = camera_pos_ws;
    vec3 ray_dir = normalize(v_in.position_ws - ray_origin);
    vec2 outer_sphere_intersection = intersect_sphere(ray_origin, ray_dir, black_hole_centre, black_hole_radius);

    // Accretion Disc Information.
    vec3 bottom_cap_pos = black_hole_centre - (0.5 * disc_width * disc_normal);
    vec3 top_cap_pos = black_hole_centre + (0.5 * disc_width * disc_normal);
    float disc_radius = black_hole_radius * disc_outer_radius;
    float inner_radius = black_hole_radius * disc_inner_radius;


    // Ray information.
    float transmittance = 0;
    vec3 sample_pos = vec3(max_float, 0, 0);

    // Ray intersects with the outer sphere.
    if (outer_sphere_intersection.x < max_float)
    {
        float dst_to_disc = intersect_disc(ray_origin, ray_dir, bottom_cap_pos, top_cap_pos, disc_normal, disc_radius, inner_radius);
        if (dst_to_disc < max_float)
        {
            transmittance = 1;
            sample_pos = ray_origin + (ray_dir * dst_to_disc);
        }
    }

    // Calculate disc UVs.
    vec2 uv = vec2(0.0,0.0);
    vec3 planar_disc_pos = vec3(0.0,0.0,0.0);
    if (sample_pos.x < max_float)
    {
        planar_disc_pos = sample_pos - dot(sample_pos - black_hole_centre, disc_normal) * disc_normal - black_hole_centre;
        uv = disc_uv(planar_disc_pos, disc_normal, black_hole_centre, disc_radius);
        uv.y += time * disc_rotation_speed;
    }
    float disc_colour = texture(disc_texture, uv).r;


    // Sample background colour.
    vec2 screen_uv = (v_in.position_cs.xy / v_in.position_cs.w) * 0.5 + 0.5;
    vec3 background_color = texture(screen_texture, screen_uv).rgb;
    vec3 accretion_disc_color = calculate_disc_color(disc_color, planar_disc_pos, disc_normal, camera_pos_ws, uv.x, disc_radius);

    // Calculate and output final colour.
    transmittance *= disc_colour;
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
        float dst_to_sphere_near = max(0, (-b - sqrt_d) / (2 * a));
        float dst_to_sphere_far = (-b + sqrt_d) / (2 * a);

        if (dst_to_sphere_far >= 0.0)
        {
            return vec2(dst_to_sphere_near, dst_to_sphere_far - dst_to_sphere_near);
        }
    }
    // Discriminant < 0: No Intersections || Discriminant == 0: 1 Intersection (Glancing).
    return vec2(max_float);
}

// Based upon 'https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf'.
vec2 intersect_infinite_cylinder(vec3 ray_origin, vec3 ray_dir, vec3 cylinder_origin, vec3 cylinder_dir, float cylinder_radius)
{
    vec3 a0 = ray_dir - (dot(ray_dir, cylinder_dir) * cylinder_dir);
    float a = dot(a0, a0);

    vec3 distance_origin = ray_origin - cylinder_origin;
    vec3 c0 = distance_origin - (dot(distance_origin, cylinder_dir) * cylinder_dir);
    float c = dot(c0, c0) - (cylinder_radius * cylinder_radius);

    float b = 2 * dot(a0, c0);
    float discriminant = (b * b) - (4 * a * c);

    if (discriminant > 0)
    {
        // 2 Intersections.
        float sqrt_d = sqrt(discriminant);
        float dst_to_near = max(0, (-b - sqrt_d) / (2 * a));
        float dst_to_far = (-b + sqrt_d) / (2 * a);

        if (dst_to_far >= 0)
        {
            return vec2(dst_to_near, dst_to_far - dst_to_near);
        }
    }
    // Didn't intersect the cylinder (No intersections, only a glancing intersection, OR we're starting within the cylinder).
}

// Based upon 'https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf'.
float intersect_infinite_plane(vec3 ray_origin, vec3 ray_dir, vec3 plane_origin, vec3 plane_normal)
{
    float a = 0;
    float b = dot(ray_dir, plane_normal);
    float c = dot(ray_origin, plane_normal) - dot(plane_normal, plane_origin);

    float discriminant = (b * b) - (4 * a * c);

    return -c / b;
}

// Based upon 'https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf'.
float intersect_disc(vec3 ray_origin, vec3 ray_dir, vec3 bottom_cap_pos, vec3 top_cap_pos, vec3 disc_normal, float disc_radius, float inner_radius)
{
    float dst_to_disc = max_float;
    vec2 cylinder_intersection = intersect_infinite_cylinder(ray_origin, ray_dir, bottom_cap_pos, disc_normal, disc_radius);
    float dst_to_cylinder = cylinder_intersection.x;

    if (dst_to_cylinder < max_float)
    {
        float finite_c1 = dot(disc_normal, ray_origin + ray_dir * dst_to_cylinder - bottom_cap_pos);
        float finite_c2 = dot(disc_normal, ray_origin + ray_dir * dst_to_cylinder - top_cap_pos);

        if (finite_c1 > 0 && finite_c2 < 0 && dst_to_cylinder > 0)
        {
            // Ray intersects with edges of the cylinder/disc.
            dst_to_disc = dst_to_cylinder;
        }
        else
        {
            float sqr_radius = disc_radius * disc_radius;
            float sqr_inner_radius = inner_radius * inner_radius;

            float dst_bottom_cap = max(0, intersect_infinite_plane(ray_origin, ray_dir, bottom_cap_pos, disc_normal));
            vec3 q1 = ray_origin + ray_dir * dst_bottom_cap;
            float sqr_dst_bottom_q1 = dot(q1 - bottom_cap_pos, q1 - bottom_cap_pos);

            // Check if ray intersects with lower plane of cylinder/disc.
            if (dst_bottom_cap > 0 && sqr_dst_bottom_q1 < sqr_radius && sqr_dst_bottom_q1 > sqr_inner_radius)
            {
                if (dst_bottom_cap < dst_to_disc)
                {
                    dst_to_disc = dst_bottom_cap;
                }
            }

            float dst_top_cap = max(0, intersect_infinite_plane(ray_origin, ray_dir, top_cap_pos, disc_normal));
            vec3 q2 = ray_origin + ray_dir * dst_top_cap;
            float sqr_dst_top_q2 = dot(q2 - top_cap_pos, q2 - top_cap_pos);

            // Check if ray intersects with upper plane of cylinder/disc.
            if (dst_top_cap > 0 && sqr_dst_top_q2 < sqr_radius && sqr_dst_top_q2 > sqr_inner_radius)
            {
                if (dst_top_cap < dst_to_disc)
                {
                    dst_to_disc = dst_top_cap;
                }
            }
        }
    }

    return dst_to_disc;
}


// ----- Accretion Disc UV Calculation -----
// Remaps a float from a given range to a new range.
float remap(float v, float min_old, float max_old, float min_new, float max_new)
{
    return min_new + (v - min_old) * (max_new - min_new) / (max_old - min_old);
}

// Calculates polar coordinates and remaps them to a 0-1 range.
vec2 disc_uv(vec3 planar_disc_pos, vec3 disc_normal, vec3 disc_centre, float radius)
{
    vec3 normalized_planar_disc_pos = normalize(planar_disc_pos);
    float sample_dist01 = length(planar_disc_pos) / radius;

    vec3 tangent_test_vector = vec3(1.0, 0.0, 0.0);
    if (abs(dot(disc_normal, tangent_test_vector)) >= 1)
    {
        tangent_test_vector = vec3(0.0, 1.0, 0.0);
    }

    vec3 tangent = normalize(cross(disc_normal, tangent_test_vector));
    vec3 bi_tangent = cross(tangent, disc_normal);
    float phi = atan(dot(normalized_planar_disc_pos, bi_tangent), dot(normalized_planar_disc_pos, tangent)) / PI;
    phi = remap(phi, -1, 1, 0, 1);

    // Radial Distance.
    float u = sample_dist01;
    // Angular Distance.
    float v = phi;

    return vec2(u,v);
}


// ----- Accretion Disc Colour -----
// Calculates the colour of the accretion disc.
// Intensity fades towards edges, is affected by doppler beaming (Parts moving towards the camera are more intense), and has the hue affected by the radial distance.
vec3 calculate_disc_color(vec3 base_color, vec3 planar_disc_pos, vec3 disc_normal, vec3 camera_pos, float u, float radius)
{
    vec3 new_color = base_color;

    // Distance intensity falloff.
    float intensity = remap(u, 0, 1, 0.5, -1.2);
    intensity *= abs(intensity);

    // Doppler beaming intensity change.
    vec3 rotate_pos = rotate_about_axis(planar_disc_pos, disc_normal, 0.01);
    float doppler_distance = (length(rotate_pos - camera_pos) - length(planar_disc_pos - camera_pos)) / radius;
    intensity += doppler_distance * disc_rotation_speed * doppler_beaming_factor;


    new_color = hdr_intensity(base_color, intensity);

    // Distance hue shift.
    vec3 hue_color = rgb_to_hsv(new_color);
    float hue_shift = clamp(remap(u, hue_radius, 1, 0, 1), 0, 1);
    hue_color.r += hue_shift * hue_shift_factor;
    new_color = hsv_to_rgb(hue_color);

    return new_color;
}



// ----- Color Conversion Functions -----
vec3 linear_to_gamma_space(vec3 linear_rgb)
{
    linear_rgb = max(linear_rgb, vec3(0.0, 0.0, 0.0));
    return max(1.055 * pow(linear_rgb, 0.416666667) - 0.055, 0.0);
}

vec3 gamma_to_linear_space(vec3 s_rgb)
{
    return s_rgb * (s_rgb * (s_rgb * 0.305306011 + 0.682171111) + 0.012522878);
}

// Based upon 'https://forum.unity.com/threads/how-to-change-hdr-colors-intensity-via-shader.531861/'.
vec3 hdr_intensity(vec3 emissive_color, float intensity)
{
    emissive_color = linear_to_gamma_space(emissive_color);

    // Apply intensity exposure.
    emissive_color *= pow(2.0, intensity);

    emissive_color = gamma_to_linear_space(emissive_color);

    return emissive_color;
}

// Based upon Unity's shadergraph library functions.
vec3 rgb_to_hsv(vec3 color)
{
    vec4 k = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(color.bg, k.wz), vec4(color.gb, k.xy), step(color.b, color.g));
    vec4 q = mix(vec4(p.xyw, color.r), vec4(color.r, p.yzx), step(p.x, color.r));
    float d = q.x - min(q.w, q.y);
    float e = 1e-10;

    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// Based upon Unity's shadergraph library functions.
vec3 hsv_to_rgb(vec3 color)
{
    vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(frac(vec3(color.x) + k.xyz) * 6.0 - vec3(k.w));
    return color.z * mix(vec3(k.x), clamp(p - vec3(k.x), 0, 1), color.y);
}

// Based upon Unity's shadergraph library functions.
vec3 rotate_about_axis(vec3 in_pos, vec3 axis, float rotation)
{
    float sin_rot = sin(rotation);
    float cos_rot = cos(rotation);
    float one_minus_cos = 1.0 - cos_rot;

    axis = normalize(axis);
    mat3 rot_mat = mat3(
        vec3(one_minus_cos * axis.x * axis.x + cos_rot, one_minus_cos * axis.x * axis.y - axis.z * sin_rot, one_minus_cos * axis.z * axis.x + axis.y * sin_rot),
        vec3(one_minus_cos * axis.x * axis.y + axis.z * sin_rot, one_minus_cos * axis.y * axis.y + cos_rot, one_minus_cos * axis.y * axis.z - axis.x * sin_rot),
        vec3(one_minus_cos * axis.z * axis.x - axis.y * sin_rot, one_minus_cos * axis.y * axis.z + axis.x * sin_rot, one_minus_cos * axis.z * axis.z + cos_rot)
    );

    return rot_mat * in_pos;
}