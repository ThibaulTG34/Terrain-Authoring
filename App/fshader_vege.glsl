#version 150

out vec4 fragColor;
in vec2 uvs_;
in vec3 normal_;
in vec3 position_;
uniform sampler2D text_tree;
uniform float amplitudeMAX;
uniform vec3 light_position;
uniform vec3 lightColor;

void main()
{

    vec3 light_pos = light_position + vec3(0, amplitudeMAX, 0);

   // ambient
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   // diffuse 
   vec3 norm = normalize(normal_*-1);
   vec3 lightDir = normalize(light_position - position_);
   float diff = max(dot(norm, lightDir), 0.3);
   vec3 diffuse = diff * lightColor;

    fragColor =vec4((ambient + diffuse) * texture(text_tree, uvs_).xyz,1.0);

}