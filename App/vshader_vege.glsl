#version 150

in vec3 vertex;
in vec2 uvs;
in vec3 normal;
// in vec2 texture_coordonnees;

uniform vec2 uvLocalPlan;

out vec2 uvs_;
out vec3 normal_;
out vec3 position_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmap;
uniform sampler2D heightmap_tool;
uniform float amplitudeMAX;
uniform float amplitudeMIN;
uniform bool tool_active;

void main()
{
    uvs_ =uvs ;
    normal_= mat3(transpose(inverse(model))) * normal;

    vec4 new_position;
    if(tool_active)
    {
        float height = texture(heightmap_tool, uvLocalPlan).r;
        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;
        new_position = vec4(vertex.x, vertex.y+height, vertex.z, 1.0);
    }
    else{
        float height = texture(heightmap, uvLocalPlan).r;
        height = (height * (amplitudeMAX - amplitudeMIN)) + amplitudeMIN;
        new_position = vec4(vertex.x, height+vertex.y, vertex.z, 1.0);  
    }
    position_=new_position.xyz;
    gl_Position = projection * view * model * new_position;
}