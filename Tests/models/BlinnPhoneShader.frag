uniform vec4 u_ambient;        
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess; 
uniform float u_opacity;         
uniform sampler2D u_diffuseTex;
in vec3 v_position;
in vec3 v_normal;                   
in vec2 v_texcoord;  
layout(location=0) out vec4 fragColor;
out vec4 color;                     
void main(){      

#if VE_USE_LIGHTS 
	vec3 lDir = normalize(vec3(0.0, 0.0, 1.0));          
	vec3 normal = normalize(v_normal);                   
	vec3 eye = normalize(-v_position);                   
	vec3 H = normalize(eye + lDir);                 
	float specfactor = pow(max(0.0, dot(normal, H)), u_shininess);      
	float diffactor = max(0.0, dot(normal, lDir));   
#endif
	            
#if VE_USE_TEXTURES && VE_USE_LIGHTS 
	fragColor = vec4((diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord) + specfactor * u_specular + u_ambient).xyz, u_opacity); 
#elif VE_USE_TEXTURES
	fragColor = vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity); 
#elif VE_USE_LIGHTS
	fragColor = vec4((diffactor * u_diffuse + specfactor * u_specular + u_ambient).xyz, u_opacity); 
#else
	fragColor = vec4((u_diffuse + u_specular + u_ambient).xyz, u_opacity); 
#endif
}