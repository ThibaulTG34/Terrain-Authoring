#version 150
in vec4 vertex;
in vec3 normal;
in vec2 texture_coordonnees;
in vec2 biome;

out vec3 v_position;
out vec3 v_normal;
out vec2 uvs;
out vec2 biome_f;


uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;

uniform sampler2D heightmap;

void main() {
    uvs = texture_coordonnees;
    biome_f = biome;
    float height = texture(heightmap, texture_coordonnees).r;
    vec4 new_position = vec4(vertex.x, height, vertex.z, 1.0);
    
    v_position = new_position.xyz;
    v_normal = normal_matrix * normal;

    gl_Position = mvp_matrix * new_position;
}
