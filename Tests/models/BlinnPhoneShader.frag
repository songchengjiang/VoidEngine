uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess; 
uniform float u_opacity;         
uniform sampler2D u_diffuseTex;
in vec4 v_position;
in vec4 v_normalAndepth;                   
in vec2 v_texcoord;  

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
	if (0 < ve_DirectionalLightNumber){
		for (int i = 0; i < ve_DirectionalLightNumber; ++i){
			vec3 lDir = -ve_DirectionalLight[i].direction;                          
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color; 
			if (0.0 < u_shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), u_shininess) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color;      
		}		
	}

	if (0 < ve_PointLightNumber){
		for (int i = 0; i < ve_PointLightNumber; ++i){
			vec3 lDir = normalize(ve_PointLight[i].position - v_position.xyz);                          
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_PointLight[i].intensity * ve_PointLight[i].color; 
			if (0.0 < u_shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), u_shininess) * ve_PointLight[i].intensity * ve_PointLight[i].color;      
		}
	}

	if (0 < ve_SpotLightNumber){
		for (int i = 0; i < ve_SpotLightNumber; ++i){
			vec3 lDir = normalize(ve_SpotLight[i].position - v_position.xyz);
			float currentAngleCos = dot(lDir, -ve_SpotLight[i].direction);
			float attenuation = smoothstep(ve_SpotLight[i].outerAngleCos, ve_SpotLight[i].innerAngleCos, currentAngleCos);                        
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation; 
			if (0.0 < u_shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), u_shininess) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation;      
		}		
	}
}
#endif
             
void main(){      

#ifdef VE_USE_LIGHTS
#ifndef VE_USE_DEFERRED_PATH

#endif
#endif
    
#ifdef VE_USE_DEFERRED_PATH
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
    
#endif
}