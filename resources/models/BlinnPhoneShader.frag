uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess; 
uniform float u_opacity;         
uniform sampler2D u_diffuseTex;
in vec4 v_position;
in vec4 v_normalAndepth;                   
in vec2 v_texcoord;

#ifdef VE_USE_LIGHTS

#ifdef VE_DIRECTIONAL_LIGHT_MAX_NUM
in vec4 v_directionalLightST[VE_DIRECTIONAL_LIGHT_MAX_NUM];
#endif

#ifdef VE_POINT_LIGHT_MAX_NUM
in vec3 v_pointLightST[VE_POINT_LIGHT_MAX_NUM];
#endif

#ifdef VE_SPOT_LIGHT_MAX_NUM
in vec4 v_spotLightST[VE_SPOT_LIGHT_MAX_NUM];
#endif

#endif

layout(location=0) out vec4 fragColor;
#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 position;
layout(location=2) out vec4 normAndepth;
#endif

#ifdef VE_USE_LIGHTS

vec2 caculateLightFactor(in vec3 normal, in vec3 lightDir, in vec3 eyeDir, in float shininess){
	vec2 factor = vec2(0.0);
	factor.x = max(0.0, dot(normal, lightDir));
	if (0.0 < shininess){
		vec3 H = normalize(eyeDir + lightDir);
		factor.y = pow(max(0.0, dot(normal, H)), shininess);
	}
	return factor;
}

void Lighting(out vec3 diffLightColor, out vec3 specLightColor)
{
	diffLightColor = specLightColor = vec3(0.0);
	vec3 normal = normalize(v_normalAndepth.xyz);                   
	vec3 eye = normalize(-v_position.xyz);
	vec3 lDir = vec3(0.0);
	float attenuation;

#ifdef VE_DIRECTIONAL_LIGHT_MAX_NUM
	for (int i = 0; i < ve_directionalLightNum; ++i){
		lDir = -ve_directionalLight[i].direction;
		attenuation = ve_directionalLight[i].intensity;
		if (0.0 < ve_directionalLight[i].shadowEnabled){
			vec3 shadowCoord = vec3(v_directionalLightST[i].xy, v_directionalLightST[i].z - ve_directionalLight[i].shadowBias) / v_directionalLightST[i].w;
			float shadow = texture(ve_directionalLight[i].shadowMap2D, shadowCoord);
			if (shadow < 1.0)
				attenuation *=  mix(1.0, shadow, ve_directionalLight[i].shadowStrength);	
		}

		vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
		vec3 lightIntensity = ve_directionalLight[i].color * attenuation;
		diffLightColor += lightIntensity * factor.x;
		specLightColor += lightIntensity * factor.y;
	}
#endif

#ifdef VE_POINT_LIGHT_MAX_NUM
	for (int i = 0; i < ve_pointLightNum; ++i){
		lDir = ve_pointLight[i].position - v_position.xyz;
		vec3 lDirAttenuation = lDir * ve_pointLight[i].attenuationRangeInverse;
		attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_pointLight[i].intensity;
		lDir = normalize(lDir);
		if (0.0 < ve_pointLight[i].shadowEnabled){
			float pTolDis2 = dot(v_pointLightST[i], v_pointLightST[i]);
			pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);
			vec4 shadowCoord = vec4(v_pointLightST[i], pTolDis2 - ve_pointLight[i].shadowBias);
			float shadow = texture(ve_pointLight[i].shadowMapCube, shadowCoord);
			if (shadow < 1.0)
				attenuation *=  mix(1.0, shadow, ve_pointLight[i].shadowStrength);	
		}

		vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
		vec3 lightIntensity = ve_pointLight[i].color * attenuation;
		diffLightColor += lightIntensity * factor.x;
		specLightColor += lightIntensity * factor.y;
	}
#endif

#ifdef VE_SPOT_LIGHT_MAX_NUM
	for (int i = 0; i < ve_spotLightNum; ++i){
		lDir = ve_spotLight[i].position - v_position.xyz;
		vec3 lDirAttenuation = lDir * ve_spotLight[i].attenuationRangeInverse;
		lDir = normalize(lDir);
		float currentAngleCos = dot(lDir, -ve_spotLight[i].direction);
		attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_spotLight[i].intensity;
		attenuation *= smoothstep(ve_spotLight[i].outerAngleCos, ve_spotLight[i].innerAngleCos, currentAngleCos);
		if (0.0 < ve_spotLight[i].shadowEnabled){
			vec3 shadowCoord = vec3(v_spotLightST[i].xy, v_spotLightST[i].z - ve_spotLight[i].shadowBias) / v_spotLightST[i].w;
			float shadow = texture(ve_spotLight[i].shadowMap2D, shadowCoord);
			if (shadow < 1.0)
				attenuation *=  mix(1.0, shadow, ve_spotLight[i].shadowStrength);	
		}

		vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
		vec3 lightIntensity = ve_spotLight[i].color * attenuation;
		diffLightColor += lightIntensity * factor.x;
		specLightColor += lightIntensity * factor.y;
	}
#endif
}
#endif
             
void main(){
    
#ifdef VE_USE_DEFERRED_PATH
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif
    position = vec4(v_position.xyz, u_shininess);
    normAndepth = v_normalAndepth;
#else //NOT VE_USE_DEFERRED_PATH
#ifdef VE_USE_LIGHTS
    vec3 diffactor;
    vec3 specfactor;
    Lighting(diffactor, specfactor);
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord).xyz + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(diffactor * u_diffuse + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_TEXTURES
#else //NOT VE_USE_LIGHTS
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_TEXTURES
#endif //END VE_USE_LIGHTS
#endif //END VE_USE_DEFERRED_PATH
}