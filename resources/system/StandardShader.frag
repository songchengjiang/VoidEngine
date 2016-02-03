uniform float u_screenWidth;
uniform float u_screenHeight;
uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess; 
uniform float u_opacity;
uniform float u_lightMask;
uniform float u_roughness;
uniform float u_fresnel; 
#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif
#ifdef VE_USE_SPECULAR_TEXTURE
uniform sampler2D u_specularTex;
#endif
#ifdef VE_USE_NORMAL_TEXTURE
uniform sampler2D u_normalTex;
#endif
#ifdef VE_USE_DISPLACEMENT_TEXTURE
uniform float u_displacement;
uniform sampler2D u_displacementTex;
#endif

in vec3 v_viewNormal;
#ifdef VE_USE_NROMAL_MAPPING
in vec3 v_viewTangent;
in vec3 v_viewBitangent; 
#endif

#ifdef VE_USE_PARALLAX_MAPPING
in vec3 v_viewDir;
#endif
            
in vec2 v_texcoord;

layout(location=0) out vec4 RT0;
//#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 RT1;
layout(location=2) out vec4 RT2;
//#endif

vec2 encode (vec3 normal)
{
    float p = sqrt(normal.z * 8.0 + 8.0);
    return vec2(normal.xy / p + 0.5);
}

#ifdef VE_USE_PARALLAX_MAPPING
vec2 parallaxMapping(vec2 texcoord, vec3 viewDir){
	const float minLayers = 5;
    const float maxLayers = 10;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir))); 

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
#endif
             
void main(){

	vec2 texcoord;
#ifdef VE_USE_PARALLAX_MAPPING
	texcoord = parallaxMapping(v_texcoord, v_viewDir);
	if(texcoord.x > 1.0 || texcoord.y > 1.0 || texcoord.x < 0.0 || texcoord.y < 0.0)
    discard;
#else
	texcoord = v_texcoord;
#endif

#ifdef VE_USE_NROMAL_MAPPING
	mat3 normCoords = mat3(v_viewTangent, v_viewBitangent, v_viewNormal);
	vec3 norm = normalize(texture(u_normalTex, texcoord).rgb * 2.0 - 1.0);
	RT0.xy = encode(normCoords * norm);
#else
	RT0.xy = encode(v_viewNormal);
#endif
	RT0.z = u_lightMask;

#ifdef VE_USE_DIFFUSE_TEXTURE
	RT1.xyz = u_diffuse * texture(u_diffuseTex, texcoord).xyz;
#else
	RT1.xyz = u_diffuse;
#endif
	RT1.w = u_roughness;

#ifdef VE_USE_SPECULAR_TEXTURE
	RT2.xyz = u_specular * texture(u_specularTex, texcoord).xyz;
#else
	RT2.xyz = u_specular;
#endif
	RT2.w = u_fresnel;
}