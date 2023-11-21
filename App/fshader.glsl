#version 150
in vec3 v_position;
in vec3 v_normal;

out vec4 fragColor;
in vec4 texture_color;

uniform vec3 light_position;
uniform vec4 ambiant_color;

float color_factor;

void main() {
   
   color_factor = max(dot(v_normal, light_position), 0.0);

   vec3 L = normalize(light_position - v_position);
   float NL = max(dot(normalize(v_normal), L), 0.0);
   vec3 color = vec3(0.0, 1.0, 1.0);
   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);

   fragColor = ambiant_color * texture_color;
}
