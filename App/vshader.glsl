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


void main() {
    uvs = texture_coordonnees;
    float height = texture(heightmap, texture_coordonnees).r;
    if(tool_active)
    {
        vec2 v_pos = vertex.xz;
        vec2 center_pos = center.xz;

        // if(distance(center_pos, v_pos) <= radius && distance(center_pos, v_pos) > radius - 0.05)
        // {
        //     height *= 1;
        // }
        // if(radius - 0.05 <= distance(center_pos, v_pos) && distance(center_pos, v_pos) < radius - 0.005)
        // {
        //     height *= (mean.y * 0.01);
        // }
        // if(radius - 0.005 <= distance(center_pos, v_pos) && distance(center_pos, v_pos) <= radius)
        // {
        //     height *= (mean.y * 0.1);
        // }
        // float coeff = radius - distance(center_pos, v_pos);
        // if(coeff >= 0)
        //     height *= (mean.y * coeff * 0.1);
    }

    // height /= 255.0;
    height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;

    height_=height;

    vec4 new_position = vec4(vertex.x, height, vertex.z, 1.0);
    
    v_position =  vertex.xyz;
    v_normal =  normal;

    gl_Position = projection * view * model * new_position;
}
