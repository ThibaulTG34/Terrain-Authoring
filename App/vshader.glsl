#version 150
in vec4 vertex;
in vec3 normal;
in vec2 texture_coordonnees;
// in float hauteur;

out vec3 v_position;
out vec3 v_normal;
out vec2 uvs;

out float height_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

uniform bool tool_active;
uniform bool height_tool;

uniform vec3 mean;

uniform sampler2D heightmap;
uniform sampler2D heightmap_tool;
uniform sampler2D NEW_hmap_tool;
uniform float amplitudeMAX;
uniform float amplitudeMIN;

uniform vec2 textureSize;


out vec2 ampl;


void main() {
    uvs = texture_coordonnees;
    vec4 new_position;
    ampl = vec2(amplitudeMIN,amplitudeMAX);
    if(tool_active)
    {
        float height = texture(heightmap_tool, texture_coordonnees).r;

        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;

        height_ = height;
        new_position = vec4(vertex.x, height, vertex.z, 1.0);
    }
    else if(height_tool)
    {
        float height = texture(NEW_hmap_tool, texture_coordonnees).r;

        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;
        height_ = height;
        new_position = vec4(vertex.x, height, vertex.z, 1.0);
    }
    else{
        float height = texture(heightmap, texture_coordonnees).r;

        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;

        height_ = height;
        new_position = vec4(vertex.x, height, vertex.z, 1.0);
    }

    v_position = new_position.xyz;
    v_normal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * model * new_position;
}
