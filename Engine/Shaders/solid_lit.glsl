#vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int exp;
uniform float heightMultiplier;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} vs_out;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = aNormal;
    vs_out.UV = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos.x, pow(aPos.y, exp) * heightMultiplier, aPos.z, 1.0);
}


#fragment
#version 460 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} fs_in;

const float PI = 3.14159;

const int MAX_LIGHTS = 16;
const int MAX_BIOMES = 16;

struct PointLight{
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Exp;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Biome {
    float height;
    vec3 color;
};


out vec4 FragColor;

uniform vec3 viewPos;

uniform int numLights;
uniform PointLight pointLights[MAX_LIGHTS];
uniform DirLight dirLight;

uniform Biome biomes[MAX_BIOMES];
uniform int numBiomes;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor){
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    // combine results
    vec3 ambient  = light.ambient  * baseColor;
    vec3 diffuse  = light.diffuse  * diff;
    vec3 specular = light.specular * spec;
    return (ambient + diffuse + specular);

}
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor){
    vec3 lightDir = normalize(light.Position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);   //16 - shininess
    // attenuation
    float distance    = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance +
    light.Exp * (distance * distance));
    // combine results
    vec3 ambient  = light.Ambient * baseColor;
    vec3 diffuse  = light.Diffuse  * diff;
    vec3 specular = light.Specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calculateBiomeColor(float yPos){
    for(int i = 0; i < numBiomes; i++){
        if(yPos < biomes[i].height){
            return biomes[i].color;
        }
    }

    return biomes[numBiomes-1].color;
}

void main() {
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 baseColor = calculateBiomeColor(fs_in.FragPos.y);

    vec3 lighting = calculateDirLight(dirLight, norm, viewDir, baseColor);

    for (int i = 0; i < numLights; i++){
        lighting += calculatePointLight(pointLights[i], norm, fs_in.FragPos, viewDir, baseColor);
    }

    vec3 result = baseColor * lighting;

    FragColor = vec4(result, 1.0);
}
