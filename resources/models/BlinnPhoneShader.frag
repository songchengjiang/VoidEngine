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
in vec4 v_shadowTexCoord[VE_LIGHT_MAX_NUM];
#endif

layout(location=0) out vec4 fragColor;
#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 position;
layout(location=2) out vec4 normAndepth;
#endif

#ifdef VE_USE_LIGHTS
void Lighting(out vec3 diffLightCol, out vec3 specLightColor)
{
	diffLightCol = specLightColor = vec3(0.0);
	vec3 normal = normalize(v_normalAndepth.xyz);                   
	vec3 eye = normalize(-v_position.xyz);

	vec3 lDir = vec3(0.0);
	float attenuation;
	for (int i = 0; i < ve_LightNum; ++i){
		if (ve_Light[i].type == VE_DIRECTIONAL_LIGHT){
			lDir = -ve_Light[i].direction;
			attenuation = ve_Light[i].intensity;
			if (0.0 < ve_Light[i].shadowEnabled){
				vec3 shadowCoord = vec3(v_shadowTexCoord[i].xy, v_shadowTexCoord[i].z - ve_Light[i].shadowBias) / v_shadowTexCoord[i].w;
				float shadow = texture(ve_lightShadowMap2D[i], shadowCoord);
				if (shadow < 1.0)
					attenuation *=  mix(1.0, shadow, ve_Light[i].shadowStrength);	
			}
		}else if (ve_Light[i].type == VE_POINT_LIGHT){
			lDir = ve_Light[i].position - v_position.xyz;
			vec3 lDirAttenuation = lDir * ve_Light[i].attenuationRangeInverse;
			attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_Light[i].intensity;
			lDir = normalize(lDir);
			if (0.0 < ve_Light[i].shadowEnabled){
				//vec4 shadowCoord = vec4(v_shadowTexCoord[i].xyz, length(v_shadowTexCoord[i].xyz) - ve_Light[i].shadowBias);
				//float shadow = texture(ve_lightShadowMapCube[i], shadowCoord);
				//if (shadow < 1.0)
					//attenuation *=  mix(1.0, shadow, ve_Light[i].shadowStrength);	
			}
		}else if (ve_Light[i].type == VE_SPOT_LIGHT){
			lDir = ve_Light[i].position - v_position.xyz;
			vec3 lDirAttenuation = lDir * ve_Light[i].attenuationRangeInverse;
			lDir = normalize(lDir);
			float currentAngleCos = dot(lDir, -ve_Light[i].direction);
			attenuation = clamp(1.0 - dot(lDirAttenuation, lDirAttenuation), 0.0, 1.0) * ve_Light[i].intensity;
			attenuation *= smoothstep(ve_Light[i].outerAngleCos, ve_Light[i].innerAngleCos, currentAngleCos);
			if (0.0 < ve_Light[i].shadowEnabled){
				vec3 shadowCoord = vec3(v_shadowTexCoord[i].xy, v_shadowTexCoord[i].z - ve_Light[i].shadowBias) / v_shadowTexCoord[i].w;
				float shadow = texture(ve_lightShadowMap2D[i], shadowCoord);
				if (shadow < 1.0)
					attenuation *=  mix(1.0, shadow, ve_Light[i].shadowStrength);	
			}
		}

		float NdotL = max(0.0, dot(normal, lDir));
		vec3 color = ve_Light[i].color * attenuation;
		diffLightCol += NdotL * color;
		if (0.0 < u_shininess){
			vec3 H = normalize(eye + lDir);
			specLightColor += pow(max(0.0, dot(normal, H)), u_shininess) * color;
		}
	}
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