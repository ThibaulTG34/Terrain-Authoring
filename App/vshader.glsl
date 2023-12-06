#version 150
in vec4 vertex;
in vec3 normal;
in vec2 texture_coordonnees;

out vec3 v_position;
out vec3 v_normal;
out vec2 uvs;

out float height_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

uniform float radius;
uniform vec3 center;
uniform bool tool_active;

uniform vec3 mean;

uniform sampler2D heightmap;
uniform float amplitudeMAX;
uniform float amplitudeMIN;

out float ampl;


void main() {
    uvs = texture_coordonnees;
    vec4 new_position;
    if(tool_active)
    {
        new_position = vertex;
        height_ = vertex.y;
    }
    else{

        float height = texture(heightmap, texture_coordonnees).r;
        ampl = amplitudeMAX;

        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;

        height_ = height;
        new_position = vec4(vertex.x, height, vertex.z, 1.0);
    
    }
    
    v_position = new_position.xyz;
    v_normal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * model * new_position;
}
