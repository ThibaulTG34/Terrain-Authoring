#version 150
in vec3 v_position;
in vec3 v_normal;
in vec2 uvs;

in float height_;
out vec4 fragColor;

uniform vec3 light_position;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform float radius;
uniform vec3 center;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float color_factor;
uniform sampler2D biome;
uniform sampler2D water;

in vec2 ampl;

uniform sampler2D desertB;
uniform sampler2D desertM;
uniform sampler2D desertT;

uniform sampler2D canyonB;
uniform sampler2D canyonM;
uniform sampler2D canyonT;

uniform sampler2D montagneB;
uniform sampler2D montagneM;
uniform sampler2D montagneT;

uniform sampler2D glacialB;
uniform sampler2D glacialM;
uniform sampler2D glacialT;

uniform sampler2D waterText;

uniform vec2 textureSize;

uniform bool tool_active;
uniform bool height_tool;
uniform bool tree_active;
uniform bool biome_edit_active;
uniform bool water_active;
uniform bool tree_active_delete;

uniform float alt_min;
uniform float alt_max;

uniform sampler2D heightmap_tool;


vec4 canyon_rouge =vec4(0.341, 0.341, 0.341, 1.0);
vec4 desert_jaune =vec4(0.843, 0.843, 0.843, 1.0);
vec4 montagne_vert =vec4(0.5, 0.5, 0.5, 1.0);
vec4 glacial_blanc =vec4(1.0, 1.0, 1.0, 1.0);
#define NB_BIOMES 4
vec4 heightsBiomes[NB_BIOMES];


float percentages[NB_BIOMES];

float pourcentageProximite(float valeurReelle, float seuil1, float seuil2) {
    float distanceAuSeuil1 = abs(valeurReelle - seuil1);
    float distanceAuSeuil2 = abs(valeurReelle - seuil2);

    float transitionWidth = seuil2 - seuil1;
    float pourcentage1 = 1.0 - clamp(distanceAuSeuil1 / transitionWidth, 0.0, 1.0);
    float pourcentage2Alone = 1.0 - pourcentage1;

    return pourcentage2Alone;
}


void initTexelWithHauteur(vec2 uv){
   // float seuil_middle=alt_max*(ampl[1]-ampl[0])+ampl[0];
   // float min_hauteur=alt_min*(ampl[1]-ampl[0])+ampl[0];
   float distance_de_smooth=0.4*(alt_max-alt_min);
  if(height_<alt_min-0.4){
         heightsBiomes[1]= texture(desertB, uv);
      }else if(height_<alt_max-0.4 ){
         if(alt_max-alt_min<0.41){
            heightsBiomes[0]=mix(texture(desertB, uv), texture(desertT, uv), pourcentageProximite(height_,alt_min-0.4,alt_max-0.4));
         }else{
            heightsBiomes[0]= mix(texture(desertB, uv), texture(desertM, uv), pourcentageProximite(height_,alt_min-0.4,alt_min));
         }
      }else
       if(height_>alt_max-0.4){
          if(alt_max-alt_min<0.41){
            heightsBiomes[0]= texture(desertT, uv);
         }
         else{
             heightsBiomes[0]=mix(texture(desertM, uv), texture(desertT, uv), pourcentageProximite(height_,alt_max-0.4,alt_max));
         }
      }

   if(height_<alt_min-0.4){
         heightsBiomes[1]= texture(canyonB, uv);
      }else if(height_<alt_max-0.4 ){
         if(alt_max-alt_min<0.41){
            heightsBiomes[1]=mix(texture(canyonB, uv), texture(canyonT, uv), pourcentageProximite(height_,alt_min-0.4,alt_max-0.4));
         }else{
            heightsBiomes[1]= mix(texture(canyonB, uv), texture(canyonM, uv), pourcentageProximite(height_,alt_min-0.4,alt_min));
         }
      }else
       if(height_>alt_max-0.4){
          if(alt_max-alt_min<0.41){
            heightsBiomes[1]= texture(canyonT, uv);
         }
         else{
             heightsBiomes[1]=mix(texture(canyonM, uv), texture(canyonT, uv), pourcentageProximite(height_,alt_max-0.4,alt_max));
         }
      }

      if(height_<alt_min-0.4){
         heightsBiomes[2]= texture(montagneB, uv);
      }else if(height_<alt_max-0.4 ){
         if(alt_max-alt_min<0.41){
            heightsBiomes[2]=mix(texture(montagneB, uv), texture(montagneT, uv), pourcentageProximite(height_,alt_min-0.4,alt_max-0.4));
         }else{
            heightsBiomes[2]= mix(texture(montagneB, uv), texture(montagneM, uv), pourcentageProximite(height_,alt_min-0.4,alt_min));
         }
      }else
       if(height_>alt_max-0.4){
          if(alt_max-alt_min<0.41){
            heightsBiomes[2]= texture(montagneT, uv);
         }
         else{
             heightsBiomes[2]=mix(texture(montagneM, uv), texture(montagneT, uv), pourcentageProximite(height_,alt_max-0.4,alt_max));
         }
      }

      if(height_<alt_min-0.4){
         heightsBiomes[3]= texture(glacialB, uv);
      }else if(height_<alt_max-0.4 ){
         if(alt_max-alt_min<0.41){
            heightsBiomes[3]=mix(texture(glacialB, uv), texture(glacialT, uv), pourcentageProximite(height_,alt_min-0.4,alt_max-0.4));
         }else{
            heightsBiomes[3]= mix(texture(glacialB, uv), texture(glacialM, uv), pourcentageProximite(height_,alt_min-0.4,alt_min));
         }
      }else
       if(height_>alt_max-0.4){
          if(alt_max-alt_min<0.41){
            heightsBiomes[3]= texture(glacialT, uv);
         }
         else{
             heightsBiomes[3]=mix(texture(glacialM, uv), texture(glacialT, uv), pourcentageProximite(height_,alt_max-0.4,alt_max));
         }
      }
}

vec4 getFromBiome() {
      vec2 texelSize=vec2(1,1);
      if(textureSize!=vec2(0,0))
        texelSize=vec2(1.0/textureSize.x,1.0/textureSize.y);
      int nbPixelsPris=0;
      int nbTexelOfABiome[NB_BIOMES];
      for(int i=0;i<NB_BIOMES;i++){
         percentages[i]=0.0;;
      }

      for(int i=-20;i<=20;i++){
         for(int j=-20;j<=20;j++){
            vec2 offset = vec2(i, j) * texelSize;
            vec2 new_uvs=(uvs + offset);
            if(new_uvs.x>=0 && new_uvs.y>=0 && new_uvs.x<=1 && new_uvs.y<=1){
               vec3 texColor = texture(biome,new_uvs).xyz;
               float diffDesert = abs(texColor.x - desert_jaune.x);
               float diffCanyon = abs(texColor.x - canyon_rouge.x);
               float diffMontagne = abs(texColor.x - montagne_vert.x);
               float diffGlacial = abs(texColor.x - glacial_blanc.x);
               float mindiff=diffDesert;
               int id=0;
               
               if (diffCanyon<mindiff) {
                  id=1;
                  mindiff=diffCanyon;
               } 
               
               if (diffMontagne<mindiff) {
                  id=2;
                  mindiff=diffMontagne;
               }

               if (diffGlacial<mindiff) {
                  id=3;
                  mindiff=diffGlacial;
               }
               percentages[id]+=1;
               nbPixelsPris++;
            }  
         }  
      }
      for(int i=0;i<NB_BIOMES;i++){
         percentages[i]/=nbPixelsPris;
      }
      initTexelWithHauteur(uvs);
      return heightsBiomes[0]*percentages[0]+heightsBiomes[1]*percentages[1]+heightsBiomes[2]*percentages[2]+heightsBiomes[3]*percentages[3];
   
}

void main() {
   

   vec3 light_pos = light_position + vec3(0, ampl[1], 0);

   // ambient
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   // diffuse 
   vec3 norm = normalize(v_normal);
   vec3 lightDir = normalize(light_position - v_position);
   float diff = max(dot(norm, lightDir), 0.3);
   vec3 diffuse = diff * lightColor;

   //specular
   // float specularStrenght = 0.2;
   // vec3 viewDir = normalize(viewPos - v_position);
   // vec3 reflectDir = normalize(lightDir + viewDir);
   // float spec = pow(max(dot(norm, reflectDir), 0.0), 256.0);
   // vec3 specular = spec * lightColor;

   vec3 result;
   if(texture(water,uvs).x>0.1)
      result = (ambient + diffuse) * getFromBiome().xyz;
   else
      result=(ambient + diffuse) * texture(waterText,uvs).xyz;
   fragColor = vec4(result,1);

   // vec4 c = inverse(projection) * vec4(center, 1.0) * inverse(view);
   // c.w = 0;
   // normalize(c);
   // vec3 inters = vec3(0,0,0);
   // inters = cam_pos - (cam_pos.z / c.z) * c.xyz;

   vec3 v_pos = v_position.xyz;
   vec3 center_pos = (height_ != 0) ? vec3(center.x, height_, center.z) : vec3(center.x, 0, center.z);
   if(tool_active || height_tool || tree_active || biome_edit_active || water_active ||tree_active_delete)
   {
      float dist = distance(center_pos, v_pos);
      if(dist < radius)
      {
         fragColor = vec4((fragColor.xyz)/2, 0.1);
      }
      if(radius - 0.005 <= dist && dist <= radius)
      {
         fragColor = vec4(0, 0, 0, 1);
      }
   }
   
}
