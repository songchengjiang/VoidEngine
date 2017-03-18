#include "Common.glsl.h"

static const char* SCREEN_BASED_LIGHT_VERTEX_SHADER = STRINGIFY(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
out vec2 v_texcoord;
void main()
{
    v_texcoord = texcoord;
    gl_Position = vec4(position, 1.0);
});

static const char* WORLD_BASED_LIGHT_VERTEX_SHADER = STRINGIFY(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
uniform mat4 u_ModelViewProjectMat; 
void main()
{                  
    gl_Position = u_ModelViewProjectMat * vec4(position, 1.0);
});

static const char* AMBIENT_LIGHT_FRAGMENT_SHADER = STRINGIFY(
uniform sampler2D u_RT1;
uniform vec3 u_ambient;  
in highp vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;
void main() { 
    vec4 RT1 = texture(u_RT1, v_texcoord);
    fragColor = vec4(clamp(RT1.xyz * u_ambient, 0.0, 1.0), 1.0);
});

static const char* DIRECTIONAL_LIGHT_FRAGMENT_SHADER =
SHADER_COMMON_FUNCTION
STRINGIFY(
uniform highp sampler2D u_depthTex;
uniform highp sampler2D u_RT0;
uniform sampler2D u_RT1;
uniform sampler2D u_RT2;
uniform sampler2DArrayShadow u_shadowTex;
uniform sampler2D u_irradiance;
uniform sampler2D u_radiance;
uniform float u_radianceMipMapCount;

uniform mat4 u_InvViewMat; 
uniform mat4 u_InvViewProjectMat; 
uniform mat4 u_lightMatrixs[4];   
uniform float u_shadowCascadedLevels[4];   
uniform vec3 u_cameraPosInWorld;   

uniform vec3  u_lightDirection;  
uniform vec3  u_lightColor;   
uniform float u_lightIntensity;  

uniform float u_lightShadowEnabled;  
uniform float u_lightShadowBias;  
uniform float u_lightShadowStrength;  
uniform float u_lightShadowSoft;     
uniform float u_lightShadowSoftness;    

in highp vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

\n#define SAMPLE_SIZE 9\n
const vec2 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec2[]   
(                                                      
    vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
    vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 1.0),
    vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
);                                                     

float shadowing(vec3 vertex, float depthInEye) {         
    if (u_lightShadowEnabled < 1.0) return 1.0;
    int layer = 3;
    if (depthInEye < u_shadowCascadedLevels[0])
        layer = 0;
    else if (depthInEye < u_shadowCascadedLevels[1])
        layer = 1;
    else if (depthInEye < u_shadowCascadedLevels[2])
        layer = 2;
    vec4 projectVertex = u_lightMatrixs[layer] * vec4(vertex, 1.0);
    vec3 shadowCoord = projectVertex.xyz / projectVertex.w;
    float factor = 0.0;
    if (0.0 < u_lightShadowSoft) {
        float sum = 0.0;
        for (int i = 0; i < SAMPLE_SIZE; ++i) {
            sum += texture(u_shadowTex, vec4(shadowCoord.xy + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, layer, shadowCoord.z - u_lightShadowBias));
        }
        factor = sum / float(SAMPLE_SIZE);
    }
    else {
        factor = texture(u_shadowTex, vec4(shadowCoord.xy, layer, shadowCoord.z - u_lightShadowBias));
    }

    if (factor < 1.0)
        factor = mix(1.0, factor, u_lightShadowStrength);
    return factor;
}

void main() { 
    vec4 RT0 = texture(u_RT0, v_texcoord);
    vec4 RT1 = texture(u_RT1, v_texcoord);
    vec4 RT2 = texture(u_RT2, v_texcoord);
    if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }
    
    vec3 albedo = RT1.xyz;
    vec3 specular = RT2.xyz;
    float roughness = RT1.w;
    float metallic = RT2.w;

    vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;
    highp float depth = texture(u_depthTex, v_texcoord).r;
    vec4 worldPosition = u_InvViewProjectMat * vec4(v_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    worldPosition.xyz /= worldPosition.w;
    vec3 eyePosition = u_cameraPosInWorld - worldPosition.xyz;
    vec3 eyeDir = normalize(eyePosition);
    float NdotL = max(0.0, dot(worldNormal, -u_lightDirection));
    vec3 H = normalize(eyeDir - u_lightDirection);
    float NdotH = max(0.0, dot(worldNormal, H));
    float NdotV = max(0.0, dot(worldNormal, eyeDir));
    float HdotV = max(0.0, dot(H, eyeDir));
    float diffFactor = OrenNayar(worldNormal, -u_lightDirection, eyeDir, NdotL, NdotV, roughness);
    //float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, roughness, metallic);
    
    // cook-torrance brdf
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F    = FresnelSchlick(HdotV, F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * NdotV * NdotL + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 Lo = (kD * albedo * diffFactor + brdf * NdotL);

    //IBL
    vec2 irradianceCoords = caculateCoordsWithLatLong(worldNormal);
    vec3 r = normalize(reflect(-eyeDir, worldNormal));
    vec2 radianceCoords = caculateCoordsWithLatLong(r);
    vec3 irradiance = texture(u_irradiance, irradianceCoords).rgb;
    vec3 radiance = textureLod(u_radiance, radianceCoords, roughness * u_radianceMipMapCount).rgb;
    vec3 ambient = kD * irradiance * albedo + kS * radiance;
    
    float shadow = shadowing(worldPosition.xyz, length(eyePosition));
    vec3 lightIntensity = u_lightColor * u_lightIntensity;
    fragColor = vec4(clamp(linearColor((ambient + Lo * shadow) * lightIntensity), 0.0, 1.0), 1.0);
});


static const char* POINT_LIGHT_FRAGMENT_SHADER =
SHADER_COMMON_FUNCTION
STRINGIFY(
uniform highp sampler2D u_depthTex;
uniform highp sampler2D u_RT0;
uniform sampler2D u_RT1;
uniform sampler2D u_RT2;
uniform samplerCubeShadow u_shadowTex;
uniform sampler2D u_irradiance;
uniform sampler2D u_radiance;
uniform float u_radianceMipMapCount;

uniform highp float u_screenWidth;               
uniform highp float u_screenHeight;               
uniform mat4 u_InvViewMat; 
uniform mat4 u_InvViewProjectMat;               
uniform mat4 u_lightMatrix;               
uniform vec3 u_cameraPosInWorld;               

uniform vec3  u_lightPosition;               
uniform vec3  u_lightColor;               
uniform float u_lightIntensity;               
uniform float u_lightARI;               

uniform float u_lightShadowEnabled;     
uniform float u_lightShadowBias;     
uniform float u_lightShadowStrength;     
uniform float u_lightShadowSoft;     
uniform float u_lightShadowSoftness;     

layout(location = 0) out vec4 fragColor;

\n#define SAMPLE_SIZE 20\n
const vec3 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec3[]     
(                                                   
    vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, 1.0, 1.0),
    vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 1.0, -1.0),
    vec3(1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0), vec3(-1.0, 0.0, 1.0), vec3(1.0, 0.0, -1.0), vec3(-1.0, 0.0, -1.0),
    vec3(0.0, 1.0, 1.0), vec3(0.0, -1.0, 1.0), vec3(0.0, -1.0, -1.0), vec3(0.0, 1.0, -1.0)
);     

float shadowing(highp vec3 vertex) {     
    if (u_lightShadowEnabled < 1.0) return 1.0;
    highp vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);
    highp float pTolDis2 = dot(projectVertex.xyz, projectVertex.xyz);
    pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);
    pTolDis2 = pTolDis2 - u_lightShadowBias;
    highp vec4 shadowCoord = vec4(projectVertex.xyz, pTolDis2);
    float factor = 0.0;
    if (0.0 < u_lightShadowSoft) {
        float sum = 0.0;
        for (int i = 0; i < SAMPLE_SIZE; ++i) {
            sum += texture(u_shadowTex, vec4(shadowCoord.xyz + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.w));
        }
        factor = sum / float(SAMPLE_SIZE);
    }
    else {
        factor = texture(u_shadowTex, shadowCoord);
    }

    if (factor < 1.0)
        factor = mix(1.0, factor, u_lightShadowStrength);
    return factor;
}     

void main() { 
    highp vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
    vec4 RT0 = texture(u_RT0, texCoords);
    vec4 RT1 = texture(u_RT1, texCoords);
    vec4 RT2 = texture(u_RT2, texCoords);
    if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }

    vec3 albedo = RT1.xyz;
    vec3 specular = RT2.xyz;
    float roughness = RT1.w;
    float metallic = RT2.w;
    
    vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;
    highp float depth = texture(u_depthTex, texCoords).r;
    highp vec4 worldPosition = u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    worldPosition.xyz /= worldPosition.w;

    vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);
    vec3 lightDir = u_lightPosition - worldPosition.xyz;
    vec3 attDis = lightDir * u_lightARI;
    float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);
    lightDir = normalize(lightDir);

    float NdotL = max(0.0, dot(worldNormal, lightDir));
    vec3 H = normalize(eyeDir + lightDir);
    float NdotH = max(0.0, dot(worldNormal, H));
    float NdotV = max(0.0, dot(worldNormal, eyeDir));
    float HdotV = max(0.0, dot(H, eyeDir));
    float diffFactor = OrenNayar(worldNormal, lightDir, eyeDir, NdotL, NdotV, roughness);
    //float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, RT1.w, RT2.w);
    
    // cook-torrance brdf
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F    = FresnelSchlick(HdotV, F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * NdotV * NdotL + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 Lo = kD * albedo * diffFactor + brdf * NdotL;
    
    //IBL
    vec2 irradianceCoords = caculateCoordsWithLatLong(worldNormal);
    vec3 r = normalize(reflect(-eyeDir, worldNormal));
    vec2 radianceCoords = caculateCoordsWithLatLong(r);
    vec3 irradiance = texture(u_irradiance, irradianceCoords).rgb;
    vec3 radiance = textureLod(u_radiance, radianceCoords, roughness * u_radianceMipMapCount).rgb;
    vec3 ambient = kD * irradiance * albedo + kS * radiance;
    
    float shadow = shadowing(worldPosition.xyz);
    vec3 lightIntensity = u_lightColor * u_lightIntensity * attenuation;
    fragColor = vec4(clamp(linearColor(ambient + Lo * shadow) * lightIntensity, 0.0, 1.0), 1.0);
    
    //fragColor = vec4(clamp((linearColor(RT1.xyz) * diffFactor + linearColor(RT2.xyz) * specFactor) * lightIntensity, 0.0, 1.0), 1.0);
});

static const char* SPOT_LIGHT_FRAGMENT_SHADER =
SHADER_COMMON_FUNCTION
STRINGIFY(
uniform highp sampler2D u_depthTex;
uniform highp sampler2D u_RT0;
uniform sampler2D u_RT1;
uniform sampler2D u_RT2;
uniform sampler2DShadow u_shadowTex;
uniform sampler2D u_irradiance;
uniform sampler2D u_radiance;
uniform float u_radianceMipMapCount;

uniform float u_screenWidth; 
uniform float u_screenHeight; 
uniform mat4 u_InvViewMat; 
uniform mat4 u_InvViewProjectMat; 
uniform mat4 u_lightMatrix; 
uniform vec3 u_cameraPosInWorld; 

uniform vec3  u_lightPosition; 
uniform vec3  u_lightDirection; 
uniform vec3  u_lightColor; 
uniform float u_lightIntensity; 
uniform float u_lightARI; 
uniform float u_lightInnerAngleCos; 
uniform float u_lightOuterAngleCos; 

uniform float u_lightShadowEnabled; 
uniform float u_lightShadowBias; 
uniform float u_lightShadowStrength; 
uniform float u_lightShadowSoft; 
uniform float u_lightShadowSoftness; 

layout(location = 0) out vec4 fragColor;

\n#define SAMPLE_SIZE 9\n
const vec2 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec2[]   
(                                                      
    vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
    vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 1.0),
    vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
);                                                     

float shadowing(vec3 vertex) {         
    if (u_lightShadowEnabled < 1.0) return 1.0;
    vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);
    vec3 shadowCoord = projectVertex.xyz / projectVertex.w;
    float factor = 0.0;
    if (0.0 < u_lightShadowSoft) {
        float sum = 0.0;
        for (int i = 0; i < SAMPLE_SIZE; ++i) {
            sum += texture(u_shadowTex, vec3(shadowCoord.xy + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.z - u_lightShadowBias));
        }
        factor = sum / float(SAMPLE_SIZE);
    }
    else {
        factor = texture(u_shadowTex, vec3(shadowCoord.xy, shadowCoord.z - u_lightShadowBias));
    }

    if (factor < 1.0)
        factor = mix(1.0, factor, u_lightShadowStrength);
    return factor;
}         

void main() { 
    highp vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
    vec4 RT0 = texture(u_RT0, texCoords);
    vec4 RT1 = texture(u_RT1, texCoords);
    vec4 RT2 = texture(u_RT2, texCoords);
    if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }
    
    vec3 albedo = RT1.xyz;
    vec3 specular = RT2.xyz;
    float roughness = RT1.w;
    float metallic = RT2.w;

    vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;
    highp float depth = texture(u_depthTex, texCoords).r;
    vec4 worldPosition = u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    worldPosition.xyz /= worldPosition.w;

    vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);
    vec3 lightDir = u_lightPosition - worldPosition.xyz;
    vec3 attDis = lightDir * u_lightARI;
    float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);
    lightDir = normalize(lightDir);
    float currentAngleCos = dot(lightDir, -u_lightDirection);
    attenuation *= smoothstep(u_lightOuterAngleCos, u_lightInnerAngleCos, currentAngleCos);

    float NdotL = max(0.0, dot(worldNormal, lightDir));
    vec3 H = normalize(eyeDir + lightDir);
    float NdotH = max(0.0, dot(worldNormal, H));
    float NdotV = max(0.0, dot(worldNormal, eyeDir));
    float HdotV = max(0.0, dot(H, eyeDir));
    float diffFactor = OrenNayar(worldNormal, lightDir, eyeDir, NdotL, NdotV, roughness);
    //float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, RT1.w, RT2.w);

    // cook-torrance brdf
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F    = FresnelSchlick(HdotV, F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * NdotV * NdotL + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 Lo = kD * albedo * diffFactor + brdf * NdotL;
    
    //IBL
    vec2 irradianceCoords = caculateCoordsWithLatLong(worldNormal);
    vec3 r = normalize(reflect(-eyeDir, worldNormal));
    vec2 radianceCoords = caculateCoordsWithLatLong(r);
    vec3 irradiance = texture(u_irradiance, irradianceCoords).rgb;
    vec3 radiance = textureLod(u_radiance, radianceCoords, roughness * u_radianceMipMapCount).rgb;
    vec3 ambient = kD * irradiance * albedo + kS * radiance;
    
    float shadow = shadowing(worldPosition.xyz);
    vec3 lightIntensity = u_lightColor * u_lightIntensity * attenuation;
    fragColor = vec4(clamp(linearColor(ambient + Lo * shadow) * lightIntensity, 0.0, 1.0), 1.0);
});
