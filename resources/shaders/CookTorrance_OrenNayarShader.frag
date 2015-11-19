uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_smoothness; 
uniform float u_opacity;    
uniform float u_fresnelFactor; 
uniform float u_refractivity; 
uniform float u_reflectivity;
#ifdef VE_USE_TEXTURES   
uniform sampler2D u_diffuseTex;
#endif
#ifdef VE_USE_SKYBOX
uniform samplerCube u_cubeMap;
#endif
in vec4 v_position;
in vec4 v_normalAndepth;                   
in vec2 v_texcoord;  

#ifdef VE_USE_SKYBOX
in vec3 v_refract; 
in vec3 v_reflect;
#endif

layout(location=0) out vec4 fragColor;
#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 position;
layout(location=2) out vec4 normAndepth;
#endif

#ifdef VE_USE_LIGHTS
const float PI = 3.1415926535;
float OrenNayar(vec3 norm, vec3 ldir, vec3 vdir, float NdotL, float NdotV){

	float roughness2 = (1.0 - u_smoothness) * (1.0 - u_smoothness);
	float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));
	float B = 0.45 * (roughness2 / (roughness2 + 0.09));
	float C = max(0.0, dot( normalize(vdir - norm * NdotV) , normalize(ldir - norm * NdotL) ));
	float NdotLAgl = acos(NdotL);
	float NdotVAgl = acos(NdotV);
	float alpha = max(NdotLAgl, NdotVAgl);
	float beta = min(NdotLAgl, NdotVAgl);

	return NdotL * max(0.0, (A + B * sin(alpha) * tan(beta) * C));
}

float CookTorrance(float NdotL, float NdotV, float HdotV, float NdotH){
	float alpha = pow(1.0 - u_smoothness * 0.7, 6.0);
	float alpha2 = alpha * alpha;

	float k = (0.8 + 0.5 * alpha) * (0.8 + 0.5 * alpha) / 2.0;
	float Gl = NdotL / (NdotL * (1.0 - k) + k);
	float Gv = NdotV / (NdotV * (1.0 - k) + k);
	float G = Gl * Gv;

	float F = u_fresnelFactor + (1.0 - u_fresnelFactor) * pow(1.0 - HdotV, 5.0);
	
	float Db = PI * pow(NdotH * NdotH * (alpha2 - 1) + 1, 2.0);
	float D = alpha2 / Db; 	

	return max(0.0, (F * D * G) / (4.0f * NdotL * NdotV));
}

void Lighting(out vec3 diffLightCol, out vec3 specLightColor)
{
	diffLightCol = specLightColor = vec3(0.0);
	vec3 normal = normalize(v_normalAndepth.xyz);                   
	vec3 vDir = normalize(-v_position.xyz);
	float NdotV = dot(normal, vDir);
	if (0 < ve_DirectionalLightNumber){
		for (int i = 0; i < ve_DirectionalLightNumber; ++i){
			vec3 lDir = -ve_DirectionalLight[i].direction;                          
			vec3 H = normalize(vDir + lDir);
			float NdotL = max(0, dot(normal, lDir));
			float HdotV = dot(H, vDir);
			float NdotH = dot(normal, H);
			if (0.0 < NdotL)
				diffLightCol += OrenNayar(normal, lDir, vDir, NdotL, NdotV) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color; 
			if (0.0 < u_smoothness)              
				specLightColor += CookTorrance(NdotL, NdotV, HdotV, NdotH) * ve_DirectionalLight[i].intensity * ve_DirectionalLight[i].color;      
		}		
	}

	if (0 < ve_PointLightNumber){
		for (int i = 0; i < ve_PointLightNumber; ++i){
			vec3 lDir = normalize(ve_PointLight[i].position - v_position.xyz);                          
			vec3 H = normalize(vDir + lDir);   
			float NdotL = max(0, dot(normal, lDir));
			float HdotV = dot(H, vDir);
			float NdotH = dot(normal, H);
			if (0.0 < NdotL)
				diffLightCol += OrenNayar(normal, lDir, vDir, NdotL, NdotV) * ve_PointLight[i].intensity * ve_PointLight[i].color; 
			if (0.0 < u_smoothness)              
				specLightColor += CookTorrance(NdotL, NdotV, HdotV, NdotH) * ve_PointLight[i].intensity * ve_PointLight[i].color;      
		}
	}

	if (0 < ve_SpotLightNumber){
		for (int i = 0; i < ve_SpotLightNumber; ++i){
			vec3 lDir = normalize(ve_SpotLight[i].position - v_position.xyz);
			float currentAngleCos = dot(lDir, -ve_SpotLight[i].direction);
			float attenuation = smoothstep(ve_SpotLight[i].outerAngleCos, ve_SpotLight[i].innerAngleCos, currentAngleCos);                        
			vec3 H = normalize(vDir + lDir);
			float NdotL = max(0, dot(normal, lDir));
			float HdotV = dot(H, vDir);
			float NdotH = dot(normal, H); 
			if (0.0 < NdotL)
				diffLightCol += OrenNayar(normal, lDir, vDir, NdotL, NdotV) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation; 
			if (0.0 < u_smoothness)              
				specLightColor += CookTorrance(NdotL, NdotV, HdotV, NdotH) * ve_SpotLight[i].intensity * ve_SpotLight[i].color * attenuation;      
		}		
	}
}
#endif
             
void main(){
    
	vec3 ambient = u_ambient;
#ifdef VE_USE_SKYBOX
	if (0.0 < u_refractivity)
		ambient += texture(u_cubeMap, vec3(v_refract.x, -v_refract.yz)).xyz * u_refractivity;
	if (0.0 < u_reflectivity)
		ambient += texture(u_cubeMap, vec3(v_reflect.x, -v_reflect.yz)).xyz * u_reflectivity;
	ambient = clamp(ambient, 0.0, 1.0);
#endif

#ifdef VE_USE_DEFERRED_PATH
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(u_diffuse + u_specular + ambient, u_opacity), 0.0, 1.0);
#endif
    position = vec4(v_position.xyz, u_shininess);
    normAndepth = v_normalAndepth;
#else //NOT VE_USE_DEFERRED_PATH
#ifdef VE_USE_LIGHTS
    vec3 diffactor;
    vec3 specfactor;
    Lighting(diffactor, specfactor);    
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord).xyz + specfactor * u_specular + ambient, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(diffactor * u_diffuse + specfactor * u_specular + ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_TEXTURES
#else //NOT VE_USE_LIGHTS
#ifdef VE_USE_TEXTURES
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_TEXTURES
    fragColor = clamp(vec4(u_diffuse + u_specular + ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_TEXTURES
#endif //END VE_USE_LIGHTS
#endif //END VE_USE_DEFERRED_PATH
}