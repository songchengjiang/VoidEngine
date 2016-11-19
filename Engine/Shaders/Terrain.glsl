#include "Common.glsl"

const char *TERRAIN_VERTEX_SHADER = STRINGIFY(
layout (location = ATTR_POSITION) in vec3 position;
layout (location = ATTR_NORMAL) in vec3 normal;
layout (location = ATTR_TANGENT) in vec3 tangent;
layout (location = ATTR_BITANGENT) in vec3 bitangent;
layout (location = ATTR_TEXTURE_COORD0) in vec2 texcoord0;
                 
uniform mat4 u_ModelViewProjectMat;
uniform mat4 u_ModelViewMat;
uniform mat3 u_NormalMat;
   
out vec3 v_viewNormal;
\n#ifdef VE_USE_NROMAL_MAPPING\n
out vec3 v_viewTangent;
out vec3 v_viewBitangent;
\n#endif\n

\n#ifdef VE_USE_PARALLAX_MAPPING\n
out vec3 v_viewDir;
\n#endif\n
            
out vec2 v_texcoord;

void main()
{
    vec4 finalPos = vec4(position, 1.0);
    vec3 finalNorm = normal;
\n#ifdef VE_USE_NROMAL_MAPPING\n
    vec3 finalTangent = tangent;
    vec3 finalBitangent = bitangent;
    v_viewTangent = normalize(u_NormalMat * finalTangent);
    v_viewBitangent = normalize(u_NormalMat * finalBitangent);
\n#endif\n
	v_viewNormal = normalize(u_NormalMat * finalNorm);
    
	v_texcoord = texcoord0;

\n#ifdef VE_USE_PARALLAX_MAPPING\n
	vec3 viewPosition = (u_ModelViewMat * finalPos).xyz;
	vec3 viewDir = normalize(-viewPosition);
	v_viewDir.x = dot(viewDir, v_viewTangent);
	v_viewDir.y = dot(viewDir, v_viewBitangent);
	v_viewDir.z = dot(viewDir, v_viewNormal);
\n#endif\n

	gl_Position = u_ModelViewProjectMat * finalPos; 
}
);

const char *TERRAIN_FRAGMENT_SHADER = STRINGIFY(
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_lightMask;
uniform float u_roughness;
uniform float u_fresnel;
\n#ifdef VE_USE_DIFFUSE_TEXTURE\n
uniform sampler2D u_diffuseTex;
\n#endif\n
\n#ifdef VE_USE_SPECULAR_TEXTURE\n
uniform sampler2D u_specularTex;
\n#endif\n
\n#ifdef VE_USE_NORMAL_TEXTURE\n
uniform sampler2D u_normalTex;
\n#endif\n
\n#ifdef VE_USE_DISPLACEMENT_TEXTURE\n
uniform float u_displacement;
uniform sampler2D u_displacementTex;
\n#endif\n
\n#ifdef VE_USE_ROUGHNESS_TEXTURE\n
uniform sampler2D u_roughnessTex;
\n#endif\n
\n#ifdef VE_USE_REFLECTION_TEXTURE\n
uniform sampler2D u_reflectionTex;
\n#endif\n

in vec3 v_viewNormal;
\n#ifdef VE_USE_NROMAL_MAPPING\n
in vec3 v_viewTangent;
in vec3 v_viewBitangent;
\n#endif\n

\n#ifdef VE_USE_PARALLAX_MAPPING\n
in vec3 v_viewDir;
\n#endif\n

in vec2 v_texcoord;

layout(location=0) out vec4 RT0;
layout(location=1) out vec4 RT1;
layout(location=2) out vec4 RT2;

vec3 encode (vec3 normal)
{
    float p = sqrt(normal.z * 8.0 + 8.0);
    vec2 enc255 = vec2(normal.xy / p + 0.5) * 255.0;
    vec2 residual = floor(fract(enc255) * 16.0);
    return vec3(floor(enc255), residual.x * 16.0 + residual.y) / 255.0;
}

\n#ifdef VE_USE_PARALLAX_MAPPING\n
vec2 parallaxMapping(vec2 texcoord, vec3 viewDir){
    const float minLayers = 5.0;
    const float maxLayers = 10.0;
    float numLayers = mix(maxLayers, minLayers, clamp(abs(dot(vec3(0.0, 0.0, 1.0), viewDir)), 0.0, 1.0));
    
    float layerDepth = 1.0 / numLayers;
    vec2 p = viewDir.xy * u_displacement;
    vec2 deltaTexCoord = p / numLayers;
    
    float currentLayerDepth = 0.0;
    vec2 currentTexcoord = texcoord;
    float currentDepthMapValue = texture(u_displacementTex, currentTexcoord).r;
    while (currentLayerDepth < currentDepthMapValue){
        currentTexcoord -= deltaTexCoord;
        currentDepthMapValue = texture(u_displacementTex, currentTexcoord).r;
        currentLayerDepth += layerDepth;
    }	
    
    vec2 prevTexCoord = currentTexcoord + deltaTexCoord;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(u_displacementTex, prevTexCoord).r - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoord = mix(currentTexcoord, prevTexCoord, weight);
    
    return finalTexCoord;
}
\n#endif\n

void main(){
    
    vec2 texcoord;
\n#ifdef VE_USE_PARALLAX_MAPPING\n
    texcoord = parallaxMapping(v_texcoord, v_viewDir);
    //	if(texcoord.x > 1.0 || texcoord.y > 1.0 || texcoord.x < 0.0 || texcoord.y < 0.0)
    //    discard;
\n#else\n
        texcoord = v_texcoord;
\n#endif\n
    
\n#ifdef VE_USE_NROMAL_MAPPING\n
    mat3 normCoords = mat3(v_viewTangent, v_viewBitangent, v_viewNormal);
    vec3 norm = normalize(texture(u_normalTex, texcoord).rgb * 2.0 - 1.0);
    RT0.xyz = encode(normCoords * norm);
\n#else\n
        RT0.xyz = encode(v_viewNormal);
\n#endif\n
    RT0.w = u_lightMask;
    
\n#ifdef VE_USE_DIFFUSE_TEXTURE\n
    RT1.xyz = u_diffuse * texture(u_diffuseTex, texcoord).xyz;
\n#else\n
        RT1.xyz = u_diffuse;
\n#endif\n
\n#ifdef VE_USE_ROUGHNESS_TEXTURE\n
    RT1.w = u_roughness * texture(u_roughnessTex, texcoord).x;
\n#else\n
        RT1.w = u_roughness;
\n#endif\n
    
\n#ifdef VE_USE_SPECULAR_TEXTURE\n
    RT2.xyz = u_specular * texture(u_specularTex, texcoord).xyz;
\n#else\n
        RT2.xyz = u_specular;
\n#endif\n
\n#ifdef VE_USE_REFLECTION_TEXTURE\n
    RT2.w = u_fresnel * texture(u_reflectionTex, texcoord).x;
\n#else\n
        RT2.w = u_fresnel;
\n#endif\n
    
}
);