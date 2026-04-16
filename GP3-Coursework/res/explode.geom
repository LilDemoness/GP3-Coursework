#version 330 core

// Layout qualfiers.
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


// Passing in data.
in VertexData
{
	vec3 normal_ws;
	vec2 texture_coordinate;
} gs_in[];

// Passing out data.
out VertexData
{
	vec3 normal_ws;
	vec2 texture_coordinate;
} gs_out;

// Uniform variables.
//uniform float time;
uniform float explosion_percentage;

layout(std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
};


vec4 explode(vec4 position, vec3 normal)
{
    // Amount of explosion.
    const float magnitude = 0.6;
    const float ExplosionsPerSecond = 1.0 / 6.0;    // How many explosions per second.
    const float DegreesToRadians = 3.1415 / 180.0;
    const float ExplosionRate = ExplosionsPerSecond * 360.0 * DegreesToRadians; // Multiplied by time to get the radians value for sin/cos.
	
    // Direction of the explosion (Going out along normal).
    float explosion_strength = explosion_percentage * magnitude;//((-cos(time * ExplosionRate) + 1.0) / 2.0) * magnitude;
    vec3 direction = -normal * explosion_strength;
	
    // Return desired position.
    return position + vec4(direction, 0.0);
}

vec3 CalculatePrimitiveNormal()
{
    // Getting the normal vector of each vertex.
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

    // Return the cross product between the two vectors calculated.
    return normalize(cross(a, b));
}

void PassData(int vertex_index)
{
    gs_out.normal_ws = gs_in[vertex_index].normal_ws;
    gs_out.texture_coordinate = gs_in[vertex_index].texture_coordinate;
}

void main()
{
    // Calculate our normal for this primitive.
    vec3 primitive_normal = CalculatePrimitiveNormal();

    //Setting current vertex position.
    gl_Position = projection * view * explode(gl_in[0].gl_Position, primitive_normal);
    PassData(0);
    EmitVertex();

    gl_Position = projection * view * explode(gl_in[1].gl_Position, primitive_normal);
    PassData(1);
    EmitVertex();

    gl_Position = projection * view * explode(gl_in[2].gl_Position, primitive_normal);
    PassData(2);
    EmitVertex();
    EndPrimitive();
}  