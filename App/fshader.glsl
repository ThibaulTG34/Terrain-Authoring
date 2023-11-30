#version 150
in vec3 v_position;
in vec3 v_normal;
in vec2 uvs;

in float height_;
out vec4 fragColor;

uniform vec3 light_position;
uniform vec3 viewPos;
uniform vec4 ambiant_color;
uniform vec3 lightColor;

uniform float radius;
uniform vec3 center;
uniform bool tool_active;

float color_factor;
uniform sampler2D biome;



uniform sampler2D desertB;
uniform sampler2D desertM;
uniform sampler2D desertT;

// uniform sampler2D canyonB;
// uniform sampler2D canyonM;
// uniform sampler2D canyonT;


vec4 canyon_rouge =vec4(0.341, 0.341, 0.341, 1.0);
vec4 desert_jaune =vec4(0.843, 0.843, 0.843, 1.0);

// float pourcentageDistanceAuSeuil(float valeurReelle, float seuil1, float seuil2) {
//     float distanceAuSeuil1 = abs(valeurReelle - seuil1);
//     float distanceAuSeuil2 = abs(valeurReelle - seuil2);

//     float distanceAuSeuilLePlusProche = min(distanceAuSeuil1, distanceAuSeuil2);
    
//     float plageEntreSeuils = abs(seuil2 - seuil1);

//     float pourcentage = (distanceAuSeuilLePlusProche / plageEntreSeuils);

//     return pourcentage;
// }
float pourcentageProximite(float valeurReelle, float seuil1, float seuil2) {
    float distanceAuSeuil1 = abs(valeurReelle - seuil1);
    float distanceAuSeuil2 = abs(valeurReelle - seuil2);

    float pourcentage1 = (1.0 - (distanceAuSeuil1 / abs(seuil1))) ;
    float pourcentage2 = (1.0 - (distanceAuSeuil2 / abs(seuil2))) ;
    float pourcentage2Alone = 1.0 - pourcentage1;

    return pourcentage2Alone;
}

vec4 getFromBiome() {
   if(distance(texture(biome,uvs),desert_jaune) < distance(texture(biome,uvs),canyon_rouge) ){
      
      if(height_<0.8){
         return texture(desertB, uvs);
      }else if(height_<1.5){
         return mix(texture(desertB, uvs), texture(desertM, uvs), pourcentageProximite(height_,0.8,1));
      }else{
          return texture(desertT, uvs);
      }
      

   }
}

void main() {
   
   // ambient
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   // diffuse 
   vec3 norm = normalize(v_normal);
   vec3 lightDir = normalize(light_position - v_position);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   //specular
   float specularStrenght = 0.5;
   vec3 viewDir = normalize(viewPos - v_position);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewPos, reflectDir), 0.0), 32);
   vec3 specular = specularStrenght * spec * lightColor;
         
   vec3 result = (ambient) * getFromBiome().xyz;

   // fragColor = textureEnFonctionProximite(height_,0.8,1.0,1.5);
   fragColor = vec4(result, 1.0);
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
