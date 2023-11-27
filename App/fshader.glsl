#version 150
in vec3 v_position;
in vec3 v_normal;
in vec2 uvs;
in vec2 biome_f;


out vec4 fragColor;

uniform vec3 light_position;
uniform vec4 ambiant_color;

uniform float radius;
uniform vec3 center;
uniform bool tool_active;

float color_factor;


vec4 getFromBiome() {
   //  if(biome_f==1){
      return vec4(0,1,1,1);
   //  }else{
   //    return vec4(1,1,0,1);
   //  }
}

void main() {
   
   color_factor = max(dot(v_normal, light_position), 0.0);

   vec3 L = normalize(light_position - v_position);
   float NL = max(dot(normalize(v_normal), L), 0.0);
   vec3 color = vec3(0.0, 1.0, 1.0);
   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);

   fragColor = getFromBiome();
   if(tool_active)
   {
      vec2 v_pos = v_position.xz;
      vec2 center_pos = center.xz;

      if(distance(center_pos, v_pos) < radius)
      {
         fragColor = vec4((fragColor.xyz + vec3(0, 0, 0))/2,0.1);
      }
      if(radius - 0.005 <= distance(center_pos, v_pos) && distance(center_pos, v_pos) <= radius)
      {
         fragColor = vec4(0, 0, 0, 1);
      }
   }
}
