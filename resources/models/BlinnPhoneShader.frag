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

#if VE_PLATFORM != VE_PLATFORM_ANDROID
#ifdef VE_POINT_LIGHT_MAX_NUM
in vec3 v_pointLightST[VE_POINT_LIGHT_MAX_NUM];
#endif
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

vec2 caculateLightFactor(vec3 normal, vec3 lightDir, vec3 eyeDir, float shininess){
	vec2 factor = vec2(0.0);
	factor.x = max(0.0, dot(normal, lightDir));
	if (0.0 < shininess){
		vec3 H = normalize(eyeDir + lightDir);
		factor.y = pow(max(0.0, dot(normal, H)), shininess);
	}
	return factor;
}

void Lighting(inout vec3 diffLightColor, inout vec3 specLightColor)
{
	vec3 normal = normalize(v_normalAndepth.xyz);                   
	vec3 eye = normalize(-v_position.xyz);
	vec3 lDir = vec3(0.0);
	float attenuation;

#ifdef VE_DIRECTIONAL_LIGHT_MAX_NUM
	for (int i = 0; i < VE_DIRECTIONAL_LIGHT_MAX_NUM; ++i){
		if (0 < ve_dirLightVisible[i]){
			lDir = -ve_dirLightDirection[i];
			attenuation = ve_dirLightIntensity[i];
			if (0 < ve_dirLightShadowEnabled[i]){
				vec4 shadowCoord = vec4(v_directionalLightST[i].xy / v_directionalLightST[i].w, float(i), (v_directionalLightST[i].z - ve_dirLightShadowBias[i]) / v_directionalLightST[i].w);
				float shadow = texture(ve_dirLightShadowMap, shadowCoord);
				if (shadow < 1.0)
					attenuation *=  mix(1.0, shadow, ve_dirLightShadowStrength[i]);
			}
			vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
			vec3 lightIntensity = ve_dirLightColor[i] * attenuation;
			diffLightColor += lightIntensity * factor.x;
			specLightColor += lightIntensity * factor.y;
		}
	}
#endif

#ifdef VE_POINT_LIGHT_MAX_NUM
	for (int i = 0; i < VE_POINT_LIGHT_MAX_NUM; ++i){
		if (0 < ve_pointLightVisible[i]){
			lDir = ve_pointLightPosition[i] - v_position.xyz;
			vec3 lDirAttenuation = lDir * ve_pointLightARI[i];
			attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_pointLightIntensity[i];
			lDir = normalize(lDir);

#if VE_PLATFORM != VE_PLATFORM_ANDROID
			if (0 < ve_pointLightShadowEnabled[i]){
				float pTolDis2 = dot(v_pointLightST[i], v_pointLightST[i]);
				pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);
				vec4 shadowCoord = vec4(v_pointLightST[i], float(i));
				float shadow = texture(ve_pointLightShadowMap, shadowCoord, pTolDis2 - ve_pointLightShadowBias[i]);
				if (shadow < 1.0)
					attenuation *=  mix(1.0, shadow, ve_pointLightShadowStrength[i]);
			}
#endif
			vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
			vec3 lightIntensity = ve_pointLightColor[i] * attenuation;
			diffLightColor += lightIntensity * factor.x;
			specLightColor += lightIntensity * factor.y;
		}
	}
#endif

#ifdef VE_SPOT_LIGHT_MAX_NUM
	for (int i = 0; i < VE_SPOT_LIGHT_MAX_NUM; ++i){
		if (0 < ve_spotLightVisible[i]){
			lDir = ve_spotLightPosition[i] - v_position.xyz;
			vec3 lDirAttenuation = lDir * ve_spotLightARI[i];
			lDir = normalize(lDir);
			float currentAngleCos = dot(lDir, -ve_spotLightDirection[i]);
			attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_spotLightIntensity[i];
			attenuation *= smoothstep(ve_spotLightOuterAngleCos[i], ve_spotLightInnerAngleCos[i], currentAngleCos);
			if (0 < ve_spotLightShadowEnabled[i]){
				vec4 shadowCoord = vec4(v_spotLightST[i].xy / v_spotLightST[i].w, float(i), (v_spotLightST[i].z - ve_spotLightShadowBias[i]) / v_spotLightST[i].w);
				float shadow = texture(ve_spotLightShadowMap, shadowCoord);
				if (shadow < 1.0)
					attenuation *=  mix(1.0, shadow, ve_spotLightShadowStrength[i]);
			}

			vec2 factor = caculateLightFactor(normal, lDir, eye, u_shininess);
			vec3 lightIntensity = ve_spotLightColor[i] * attenuation;
			diffLightColor += lightIntensity * factor.x;
			specLightColor += lightIntensity * factor.y;
		}
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
    vec3 diffactor = vec3(0.0);
    vec3 specfactor = vec3(0.0);
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