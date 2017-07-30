#define STRINGIFY(A)  #A
#define SHADER_COMMON_FUNCTION STRINGIFY(                                                                           \
vec3 decode(vec3 encoded) {                                                                                       \n\
    float nz = floor(encoded.z * 255.0) / 16.0;                                                                   \n\
    vec2 dec = encoded.xy + vec2(floor(nz) / 16.0, fract(nz)) / 255.0;                                            \n\
    vec2 fenc = dec * 4.0 - 2.0;                                                                                  \n\
    float f = dot(fenc, fenc);                                                                                    \n\
    float g = sqrt(1.0 - f / 4.0);                                                                                \n\
    return vec3(fenc * g, 1.0 - f / 2.0);                                                                         \n\
}                                                                                                                 \n\
                                                                                                                  \n\
vec3 linearColor(vec3 col) {                                                                                      \n\
    return pow(col, vec3(0.45454545));                                                                            \n\
}                                                                                                                 \n\
vec3 nonLinearColor(vec3 col) {                                                                                   \n\
    return pow(col, vec3(2.2));                                                                                   \n\
}                                                                                                                 \n\
                                                                                                                  \n\
float GetColorIntensity(vec3 col) {                                                                               \n\
    return col.r * 0.299 + col.g * 0.587 + col.b * 0.114;                                                         \n\
}                                                                                                                 \n\
                                                                                                                  \n\
const float PI = 3.1415926535;                                                                                    \n\
float OrenNayar(vec3 norm, vec3 ldir, vec3 vdir, float NdotL, float NdotV, float roughness) {                     \n\
                                                                                                                  \n\
    float roughness2 = roughness * roughness;                                                                     \n\
    float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));                                                     \n\
    float B = 0.45 * (roughness2 / (roughness2 + 0.09));                                                          \n\
    float C = max(0.0, dot(normalize(vdir - norm * NdotV), normalize(ldir - norm * NdotL)));                      \n\
    float NdotLAgl = acos(NdotL);                                                                                 \n\
    float NdotVAgl = acos(NdotV);                                                                                 \n\
    float alpha = max(NdotLAgl, NdotVAgl);                                                                        \n\
    float beta = min(NdotLAgl, NdotVAgl);                                                                         \n\
                                                                                                                  \n\
    return NdotL * max(0.0, (A + B * sin(alpha) * tan(beta) * C));                                                \n\
}                                                                                                                 \n\
                                                                                                                  \n\
                                                                       \n\
vec3 FresnelSchlick(float cosTheta, vec3 F0)                          \n\
{                                                                       \n\
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);                                                                       \n\
}                                                                       \n\
                                                                       \n\
float DistributionGGX(float NdotH, float roughness)                                                                       \n\
{                                                                       \n\
    float a      = roughness*roughness;                                                                       \n\
    float a2     = a*a;                                                                       \n\
    float NdotH2 = NdotH*NdotH;                                                                       \n\
                                                                           \n\
    float nom   = a2;                                                                       \n\
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);                                                                       \n\
    denom = PI * denom * denom;                                                                       \n\
                                                                           \n\
    return nom / denom;                                                                       \n\
}                                                                       \n\
                                                                       \n\
float GeometrySchlickGGX(float NdotV, float roughness)                                                                       \n\
{                                                                       \n\
    float r = (roughness + 1.0);                                                                       \n\
    float k = (r*r) / 8.0;                                                                       \n\
                                                                         \n\
    float denom = NdotV * (1.0 - k) + k;                                                                       \n\
                                                                           \n\
    return NdotV / denom;                                                                       \n\
}                                                                       \n\
float GeometrySmith(float NdotV, float NdotL, float roughness)                                                                       \n\
{                                                                       \n\
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);                                                                       \n\
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);                                                                       \n\
                                                                           \n\
    return ggx1 * ggx2;                                                                       \n\
}                                                                       \n\
                                                                       \n\
                                                                       \n\
vec2 caculateCoordsWithLatLong(vec3 D) {                                                                       \n\
    return vec2((atan(D.x, D.z) / PI + 1.0) * 0.5, (acos(D.y) / PI));                                                                       \n\
}                                                                       \n\
                                                                       \n\
float CookTorrance(float NdotL, float NdotV, float HdotV, float NdotH, float roughness, float fresnel) {         \n\
    float alpha = pow(1.0 - (1.0 - roughness) * 0.7, 6.0);                                                        \n\
    float alpha2 = alpha * alpha;                                                                                 \n\
                                                                                                                  \n\
    float k = (0.8 + 0.5 * alpha) * (0.8 + 0.5 * alpha) / 2.0;                                                    \n\
    float Gl = NdotL / (NdotL * (1.0 - k) + k);                                                                   \n\
    float Gv = NdotV / (NdotV * (1.0 - k) + k);                                                                   \n\
    float G = Gl * Gv;                                                                                            \n\
                                                                                                                  \n\
    float F = fresnel + (1.0 - fresnel) * pow(1.0 - HdotV, 5.0);                                                  \n\
                                                                                                                  \n\
    float Db = PI * pow(NdotH * NdotH * (alpha2 - 1.0) + 1.0, 2.0);                                               \n\
    float D = alpha2 / Db; 	                                                                                      \n\
    return (F * D * G) / (4.0 * NdotL * NdotV + 0.001);                                                          \n\
}                                                                                                                 \n\
                                                                                                             \n\
vec3 Irradiance(vec3 worldNormal) {                                                             \n\
    vec2 irradianceCoords = caculateCoordsWithLatLong(worldNormal);                             \n\
    return texture2D(ve_u_irradiance, irradianceCoords).rgb;                                       \n\
}                                                                                               \n\
vec3 Radiance(vec3 worldReflect, float roughness) {                                          \n\
    vec2 radianceCoords = caculateCoordsWithLatLong(worldReflect);                                          \n\
    return texture2D(ve_u_radiance, radianceCoords).rgb;                                          \n\
}                                          \n\
vec2 Lut(float NdotV, float roughness) {                                          \n\
    return texture2D(ve_u_lut, vec2(NdotV, roughness)).rg;                       \n\
}                                          \n\
                       \n\
vec3 Lighting(vec3 normal, vec3 viewDir, vec3 lightDirection, vec3 albedo, float roughness, float metallic) {                       \n\
                           \n\
    float NdotL = max(0.0, dot(normal, lightDirection));                       \n\
    vec3 H = normalize(viewDir + lightDirection);                       \n\
    float NdotH = max(0.0, dot(normal, H));                       \n\
    float NdotV = max(0.0, dot(normal, viewDir));                       \n\
    float HdotV = max(0.0, dot(H, viewDir));                       \n\
    float diffFactor = OrenNayar(normal, lightDirection, viewDir, NdotL, NdotV, roughness);                       \n\
                           \n\
    vec3 F0 = vec3(0.04);                       \n\
    F0 = clamp(mix(F0, albedo, metallic), vec3(0.0), vec3(1.0));     \n\
    float NDF = DistributionGGX(NdotH, roughness);                       \n\
    float G   = GeometrySmith(NdotV, NdotL, roughness);                       \n\
    vec3 F    = FresnelSchlick(HdotV, F0);                       \n\
    vec3 kS = F;                       \n\
    vec3 kD = vec3(1.0) - kS;                       \n\
    kD *= 1.0 - metallic;                       \n\
    vec3 nominator    = NDF * G * F;                       \n\
    float denominator = 4.0 * NdotV * NdotL + 0.001;                       \n\
    vec3 brdf = nominator / denominator;                       \n\
    vec3 Lo = (kD * diffFactor * albedo + brdf * NdotL);                       \n\
    return Lo;                       \n\
}                       \n\
vec3 IBL(vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic) {                  \n\
    float NdotV = max(0.0, dot(normal, viewDir));                       \n\
    vec3 F0 = vec3(0.04);                       \n\
    vec3 F    = FresnelSchlick(NdotV, F0);                       \n\
    vec3 kS = F;                       \n\
    vec3 kD = vec3(1.0) - kS;                       \n\
    kD *= 1.0 - metallic;                       \n\
                                            \n\
    vec3 irradiance = Irradiance(normal);                       \n\
    vec3 r = normalize(reflect(-viewDir, normal));                       \n\
    vec3 radiance = Radiance(r, roughness);                       \n\
    vec2 envBRDF = Lut(NdotV, roughness);                       \n\
    vec3 specular = radiance * (kS * envBRDF.x + envBRDF.y);                       \n\
    vec3 ambient = (kD * irradiance + specular) * albedo;                       \n\
    return ambient;                       \n\
}                                           \n\
                                           \n\
float GetAttenuation(vec3 normal, vec3 worldPos, vec3 lightPosition, float lightARI, out vec3 lightDir)                                           \n\
{                                           \n\
    vec3 lightVec = (lightPosition - worldPos) * lightARI;                                           \n\
    float lightDist = length(lightVec);                                           \n\
    float falloff = pow(clamp(1.0 - pow(lightDist / 1.0, 4.0), 0.0, 1.0), 2.0) * PI / (4.0 * PI)*(pow(lightDist, 2.0) + 1.0);                                           \n\
    lightDir = lightVec / lightDist;                                           \n\
    return clamp(dot(normal, lightDir), 0.0, 1.0) * falloff;                                           \n\
                                               \n\
}                                           \n\
#ifdef VE_USE_DISPLACEMENT_TEXTURE                                          \n\
vec2 parallaxMapping(sampler2D displacement, vec2 texcoord, vec3 viewDir){                                          \n\
    const float minLayers = 5.0;                                          \n\
    const float maxLayers = 10.0;                                          \n\
    float numLayers = mix(maxLayers, minLayers, clamp(abs(dot(vec3(0.0, 0.0, 1.0), viewDir)), 0.0, 1.0));                                          \n\
                                              \n\
    float layerDepth = 1.0 / numLayers;                                          \n\
    vec2 p = viewDir.xy * u_displacement;                                          \n\
    vec2 deltaTexCoord = p / numLayers;                                          \n\
                                              \n\
    float currentLayerDepth = 0.0;                                          \n\
    vec2 currentTexcoord = texcoord;                                          \n\
    float currentDepthMapValue = texture2D(displacement, currentTexcoord).r;                                          \n\
    while (currentLayerDepth < currentDepthMapValue){                                          \n\
        currentTexcoord -= deltaTexCoord;                                          \n\
        currentDepthMapValue = texture2D(displacement, currentTexcoord).r;                                          \n\
        currentLayerDepth += layerDepth;                                          \n\
    }                                          \n\
                                              \n\
    vec2 prevTexCoord = currentTexcoord + deltaTexCoord;                                          \n\
    float afterDepth  = currentDepthMapValue - currentLayerDepth;                                          \n\
    float beforeDepth = texture2D(displacement, prevTexCoord).r - currentLayerDepth + layerDepth;                                          \n\
    float weight = afterDepth / (afterDepth - beforeDepth);                                          \n\
    vec2 finalTexCoord = mix(currentTexcoord, prevTexCoord, weight);                                          \n\
                                              \n\
    return finalTexCoord;                                          \n\
}                                          \n\
#endif                                          \n\
                                          \n\
#ifdef VE_USE_BONES                                          \n\
uniform mat4 u_BoneMates[48];                                          \n\
attribute vec4 a_blendWeights;                                          \n\
attribute vec4 a_blendIndices;                                          \n\
void simulationBoneTransform(inout vec4 position, inout vec3 normal, inout vec3 tangent, inout vec3 binormal)                                          \n\
{                                          \n\
    vec4 weights = a_blendWeights;                                          \n\
    weights.w = 1.0 - dot(weights.xyz, vec3(1.0));                                          \n\
    mat4 boneMat = u_BoneMates[int(a_blendIndices.x)] * weights.x;                                          \n\
    boneMat += u_BoneMates[int(a_blendIndices.y)] * weights.y;                                          \n\
    boneMat += u_BoneMates[int(a_blendIndices.z)] * weights.z;                                          \n\
    boneMat += u_BoneMates[int(a_blendIndices.w)] * weights.w;                                          \n\
    position = boneMat * position;                                          \n\
    normal = (boneMat * vec4(normal, 0.0)).xyz;                                          \n\
#ifdef VE_USE_NORMAL_TEXTURE                                          \n\
    tangent = (boneMat * vec4(tangent, 0.0)).xyz;                                          \n\
    binormal = (boneMat * vec4(binormal, 0.0)).xyz;;                                          \n\
#endif                                          \n\
}                                          \n\
#endif                                          \n\
)
