#version 150
in vec4 vertex;
in vec3 normal;
in vec2 texture_coordonnees;


out vec3 v_position;
out vec3 v_normal;
out vec4 texture_color;



uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;

uniform sampler2D heightmap;

void main() {

    texture_color = texture(heightmap, texture_coordonnees);

    float height = texture(heightmap, texture_coordonnees).r;
    vec4 new_position = vec4(vertex.x, height, vertex.z, 1.0);
    
    v_position = new_position.xyz;
    v_normal = normal_matrix * normal;

    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * new_position;
}
