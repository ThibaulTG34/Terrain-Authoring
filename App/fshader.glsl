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

float color_factor;
uniform sampler2D biome;

in float ampl;

uniform sampler2D desertB;
uniform sampler2D desertM;
uniform sampler2D desertT;

uniform sampler2D canyonB;
uniform sampler2D canyonM;
uniform sampler2D canyonT;

uniform sampler2D montagneB;
uniform sampler2D montagneM;
uniform sampler2D montagneT;

uniform vec2 textureSize;

uniform bool tool_active;


vec4 canyon_rouge =vec4(0.341, 0.341, 0.341, 1.0);
vec4 desert_jaune =vec4(0.843, 0.843, 0.843, 1.0);
vec4 montagne_vert =vec4(0.5, 0.5, 0.5, 1.0);
#define NB_BIOMES 3
vec3 Biomes[NB_BIOMES];
vec4 heightsBiomes[NB_BIOMES];

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

    float transitionWidth = seuil2 - seuil1;
    
    // Pourcentage de proximité par rapport au seuil1
    float pourcentage1 = 1.0 - clamp(distanceAuSeuil1 / transitionWidth, 0.0, 1.0);
    // Inversion du pourcentage par rapport à seuil2
    float pourcentage2Alone = 1.0 - pourcentage1;

    return pourcentage2Alone;
}

float percentages[NB_BIOMES];

void makePercentagesOfBiomesWithValue(vec3 value){
   float totalDistance = 0.0;
   float distances[NB_BIOMES]; 

   for (int i = 0; i < NB_BIOMES; i++) {
       distances[i] = distance(value, Biomes[i]);
       totalDistance += distances[i];
   }
   for (int i = 0; i < NB_BIOMES; i++) {
       percentages[i] = distances[i] / totalDistance;
   }
}

void initTexelWithHauteur(vec2 uv){

   if(height_<0.6){
         heightsBiomes[0]= texture(desertB, uv);
      }else if(height_<1.5){
         heightsBiomes[0]= mix(texture(desertB, uv), texture(desertM, uv), pourcentageProximite(height_,0.6,1));
      }else{
          heightsBiomes[0]= texture(desertT, uv);
      }

      if(height_<0.6){
         heightsBiomes[1]= texture(canyonB, uv);
      }else if(height_<1.5){
         heightsBiomes[1]= mix(texture(canyonB, uv), texture(canyonM, uv), pourcentageProximite(height_,0.6,1.5));
      }else{
          heightsBiomes[1]=mix(texture(canyonM, uv), texture(canyonT, uv), pourcentageProximite(height_,1.5,2));
      }

      if(height_<0.6){
         heightsBiomes[2]= texture(montagneB, uv);
      }else if(height_<1.5){
         heightsBiomes[2]= mix(texture(montagneB, uv), texture(montagneM, uv), pourcentageProximite(height_,0.6,1.5));
      }else{
          heightsBiomes[2]=mix(texture(montagneM, uv), texture(montagneT, uv), pourcentageProximite(height_,1.5,2));
      }
}

vec4 getFromBiome() {
      

      makePercentagesOfBiomesWithValue(texture(biome,uvs).xyz);
      vec4 finalColor;
      vec2 texelSize=1.0/textureSize;
      int nbPixelsPris=0;
      int nbTexelOfABiome[NB_BIOMES];
      for(int i=0;i<NB_BIOMES;i++){
         percentages[i]=0.0;;
      }

      for(int i=-1;i<=1;i++){
         for(int j=-1;j<=1;j++){
            vec2 offset = vec2(i, j) * texelSize;
            vec2 new_uvs=(uvs + offset);
            if(new_uvs.x>=0 && new_uvs.y>=0 && new_uvs.x<=1 && new_uvs.y<=1){
               vec3 texColor = texture(biome,new_uvs).xyz;
               // finalColor+=vec4(texColor,1.0);
               float diffDesert = abs(texColor.x - desert_jaune.x);
               float diffCanyon = abs(texColor.x - canyon_rouge.x);
               float diffMontagne = abs(texColor.x - montagne_vert.x);
               float mindiff=min(diffCanyon,min(diffDesert,diffMontagne));
               float threshold = 0.01; 
               initTexelWithHauteur(new_uvs);
               if (mindiff==diffDesert) {
                  // finalColor += heightsBiomes[0];
                  percentages[0]+=1;
                  nbPixelsPris++;

               } else if (mindiff==diffCanyon) {
                  // finalColor += heightsBiomes[1];
                  percentages[1]+=1;;
                  nbPixelsPris++;
               }else if (mindiff==diffMontagne) {
                  return vec4(1,0,1,1);
               }
               //  else if (all(lessThan(diffMontagne, vec3(threshold)))) {
               //    // finalColor += heightsBiomes[2];
               //    nbTexelOfABiome[2]++;
               // }
            }  
         }  
      }
      for(int i=0;i<NB_BIOMES;i++){
         percentages[i]/=nbPixelsPris;
      }
         // finalColor=vec4(finalColor.xyz/nbPixelsPris,1.0);
      // }
      initTexelWithHauteur(uvs);
      // finalColor=mix(heightsBiomes[0], mix(heightsBiomes[1], heightsBiomes[2], (nbTexelOfABiome[1]/nbPixelsPris)),  (nbTexelOfABiome[0]/nbPixelsPris));
      finalColor=mix(heightsBiomes[0], mix(heightsBiomes[1], heightsBiomes[2], percentages[1]), percentages[0]);
      // finalColor=mix(heightsBiomes[1],finalColor,percentages[1]);
      // vec4 finalColor = (heightsBiomes[0] * percentages[0]) + (heightsBiomes[1] * percentages[1]) + (heightsBiomes[2] * percentages[2]);
      if(finalColor==vec4(0,0,0,1)){
         return vec4(1,1,1,1);
      }else{
         return finalColor;
      }
}

void main() {
   
   Biomes[0] = desert_jaune.xyz; 
   Biomes[1] = canyon_rouge.xyz;
   Biomes[2] = montagne_vert.xyz;


   vec3 light_pos = light_position + vec3(0, ampl, 0);

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
   vec3 reflectDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(norm, reflectDir), 0.0), 32.0);
   vec3 specular = spec * lightColor;

   vec3 result = (ambient + diffuse + specular) * getFromBiome().xyz;
   fragColor = vec4(result, 1.0);

   vec2 v_pos = v_position.xz;
   vec2 center_pos = center.xz;
   if(tool_active)
      {
         if(distance(center_pos, v_pos) < radius)
         {
            fragColor = vec4((fragColor.xyz + vec3(0, 0, 0))/2, 0.5);
         }
         if(radius - 0.005 <= distance(center_pos, v_pos) && distance(center_pos, v_pos) <= radius)
         {
            fragColor = vec4(0, 0, 0, 1);
         }
      }
   
}
