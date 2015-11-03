uniform sampler2D u_color;
uniform sampler2D u_position;
uniform sampler2D u_normAndepth;
in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

#ifdef VE_USE_LIGHTS
void Lighting(in vec4 position, in vec3 normal, out vec3 diffLightCol, out vec3 specLightColor)
{
	diffLightCol = specLightColor = vec3(0.0);               
	vec3 eye = normalize(-position.xyz);  
	float  shininess = position.w;
	if (0 < ve_DirectionalLightNumber){
		for (int i = 0; i < ve_DirectionalLightNumber; ++i){
			vec3 lDir = -ve_DirectionalLight[i].direction;                          
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color; 
			if (0.0 < shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), shininess) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color;      
		}		
	}

	if (0 < ve_PointLightNumber){
		for (int i = 0; i < ve_PointLightNumber; ++i){
			vec3 lDir = normalize(ve_PointLight[i].position - position.xyz);                          
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_PointLight[i].intensity * ve_PointLight[i].color; 
			if (0.0 < shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), shininess) * ve_PointLight[i].intensity * ve_PointLight[i].color;      
		}
	}

	if (0 < ve_SpotLightNumber){
		for (int i = 0; i < ve_SpotLightNumber; ++i){
			vec3 lDir = normalize(ve_SpotLight[i].position - position.xyz);
			float currentAngleCos = dot(lDir, -ve_SpotLight[i].direction);
			float attenuation = smoothstep(ve_SpotLight[i].outerAngleCos, ve_SpotLight[i].innerAngleCos, currentAngleCos);                        
			vec3 H = normalize(eye + lDir);   
			diffLightCol += max(0.0, dot(normal, lDir)) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation; 
			if (0.0 < shininess)              
				specLightColor += pow(max(0.0, dot(normal, H)), shininess) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation;      
		}		
	}
}
#endif

void main() {
	vec4 color = texture(u_color, v_texcoord);
	vec4 position = texture(u_position, v_texcoord);
	vec4 normAndepth = texture(u_normAndepth, v_texcoord);
	vec3 norm = normalize(normAndepth.xyz);
	float depth = normAndepth.w / (normAndepth.w + 1.0);

#ifdef VE_USE_LIGHTS
#ifndef VE_USE_DEFERRED_PATH
  vec3 diffactor = vec3(0.5);
  vec3 specfactor = vec3(0.5);
  Lighting(position, norm, diffactor, specfactor);
#endif
#endif

#ifdef VE_USE_LIGHTS
	//if (all(lessThanEqual(diffactor, vec3(0.0))))
	//	fragColor = color;
	//else
		fragColor = vec4(color.xyz * (diffactor + specfactor), 1.0);
#else
	fragColor = color;
#endif

	//fragColor = vec4(depth);
}