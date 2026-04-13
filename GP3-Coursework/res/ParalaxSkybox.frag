#version 400 core
const int SKYBOX_COUNT = 1;

// Inputs.
in vec3 texture_direction;

// Uniforms.
uniform samplerCube[SKYBOX_COUNT] skyboxes;
uniform float[SKYBOX_COUNT] skybox_rotations;	// Radians.

// Outputs.
out vec4 FragColor; // 'out' is used for fragment output

// Forward Declerations.
vec3 rotate(vec3 direction, float rotation_radians);
vec4 blend_colours(vec4 a, vec4 b);
float convert_to_greyscale(vec3 color_to_convert);

void main()
{
	vec4 skyboxColor = texture(skyboxes[0], rotate(texture_direction, skybox_rotations[0]));
	for(int i = 1; i < SKYBOX_COUNT; ++i)
	{
		skyboxColor = blend_colours(skyboxColor, texture(skyboxes[i], rotate(texture_direction, skybox_rotations[i])));
	}

	FragColor = skyboxColor;
}


vec3 rotate(vec3 direction, float rotation_radians)
{
	return vec3(
		direction.x * cos(rotation_radians) - direction.z * sin(rotation_radians),
		direction.y,
		direction.x * sin(rotation_radians) + direction.z * cos(rotation_radians));
}
vec4 blend_colours(vec4 a, vec4 b)
{
	//float brightness_b = convert_to_greyscale(b.rgb) * b.a;
	//float brightness_b = length(b.rgb) * b.a;

	return 1.0f - ((1.0f - a) * (1.0f - b));
	/*if (brightness_b > 0.5)
	{
		// Mix using "Screen" (Invert, Multiply, Re-Invert).
		return 1.0f - ((1.0f - a) * (1.0f - b));
	}
	else if (brightness_b < 0.5)
	{
		// Mix using "Multiply".
		return a * b;
	}
	else
	{
		// B should be Transparent here.
		return a;
	}*/
}

float convert_to_greyscale(vec3 color_to_convert)
{
	const vec3 luminosity = vec3(0.21, 0.71, 0.07);
	return dot(color_to_convert, luminosity);
}