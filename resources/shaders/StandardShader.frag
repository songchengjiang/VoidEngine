uniform vec3 u_diffuse;
uniform float u_roughness;
uniform float u_metallic;
uniform float u_alpha;
uniform vec3 u_cameraWorldPos;


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
#ifdef VE_USE_ROUGHNESS_TEXTURE
uniform sampler2D u_roughnessTex;
#endif
#ifdef VE_USE_METALLIC_TEXTURE
uniform sampler2D u_metallicTex;
#endif
#ifdef VE_USE_LIGHTMAP_TEXTURE
uniform sampler2D u_lightMapTex;
#endif

varying vec3 v_worldPosition;
varying vec3 v_worldNormal;
#ifdef VE_USE_NORMAL_TEXTURE
varying vec3 v_worldTangent;
varying vec3 v_worldBinormal;
#endif

varying vec2 v_texcoord;
             
void main(){

#ifdef VE_USE_NORMAL_TEXTURE
    vec3 worldTangent  = normalize(v_worldTangent);
    vec3 worldBinormal = normalize(v_worldBinormal);
#endif
    vec3 worldNormal   = normalize(v_worldNormal);
    vec3 worldPosition = v_worldPosition;
    vec3 worldViewDir  = normalize(u_cameraWorldPos - worldPosition);
    
	vec2 texcoord;
#ifdef VE_USE_DISPLACEMENT_TEXTURE
    vec3 viewDir = worldViewDir;
#ifdef VE_USE_NORMAL_TEXTURE
    viewDir.x = dot(worldViewDir, worldTangent);
    viewDir.y = dot(worldViewDir, worldBinormal);
    viewDir.z = dot(worldViewDir, worldNormal);
#endif
	texcoord = parallaxMapping(u_displacementTex, v_texcoord, viewDir);
//	if(texcoord.x > 1.0 || texcoord.y > 1.0 || texcoord.x < 0.0 || texcoord.y < 0.0)
//    discard;
#else
	texcoord = v_texcoord;
#endif

#ifdef VE_USE_NORMAL_TEXTURE
	mat3 normCoords = mat3(worldTangent, worldBinormal, worldNormal);
	worldNormal = texture2D(u_normalTex, texcoord).rgb * 2.0 - 1.0;
	worldNormal = normalize(normCoords * worldNormal);
#endif

    vec3 albedo;
    float alpha;
#ifdef VE_USE_DIFFUSE_TEXTURE
    vec4 diff = texture2D(u_diffuseTex, texcoord);
	albedo = u_diffuse * nonLinearColor(diff.rgb);
    alpha = u_alpha * diff.a;
#else
	albedo = u_diffuse;
    alpha = u_alpha;
#endif
    
    float roughness;
#ifdef VE_USE_ROUGHNESS_TEXTURE
    roughness = u_roughness * texture2D(u_roughnessTex, texcoord).r;
#else
	roughness = u_roughness;
#endif
    
    float metallic;
#ifdef VE_USE_METALLIC_TEXTURE
    metallic = u_metallic * texture2D(u_metallicTex, texcoord).r;
#else
	metallic = u_metallic;
#endif
    
    vec3 occlusion;
#ifdef VE_USE_LIGHTMAP_TEXTURE
    occlusion = texture2D(u_lightMapTex, texcoord).rgb;
#else
    occlusion = vec3(1.0);
#endif

    vec3 finalColor = vec3(0.0);
    vec3 lightDirection;
    float intensity = 0.0;
    
#ifdef VE_USE_LIGHTING
    vec3 lightIntensity;
    vec3 brdf;
#ifdef VE_USE_DIR_LIGHT
    lightDirection = normalize(ve_u_dirLightDirection);
    lightIntensity = ve_u_dirLightColor * ve_u_dirLightIntensity;
    brdf = Lighting(worldNormal, worldViewDir, lightDirection, albedo, roughness, metallic);
    finalColor += brdf * lightIntensity;
#endif
#ifdef VE_USE_POINT_LIGHT
    float attenuation = GetAttenuation(worldNormal, worldPosition, ve_u_pointLightPosition, ve_u_pointLightARI, lightDirection);
    lightIntensity = ve_u_pointLightColor * ve_u_pointLightIntensity;
    brdf = Lighting(worldNormal, worldViewDir, lightDirection, albedo, roughness, metallic);
    finalColor += brdf * lightIntensity * attenuation;
#endif
    intensity = GetColorIntensity(finalColor);
#endif
    
#ifdef VE_USE_IBL
    finalColor += IBL(worldNormal, worldViewDir, albedo, roughness, metallic);
#else
    finalColor += albedo;
#endif
    
    gl_FragColor = vec4(clamp(linearColor(finalColor) * occlusion, 0.0, 1.0), mix(alpha, 1.0 - alpha, intensity));

}
