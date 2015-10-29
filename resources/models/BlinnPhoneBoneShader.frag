                   
uniform vec4 u_ambient;        
uniform vec4 u_diffuse;       
uniform float u_shininess;          
uniform sampler2D u_diffuseTex;     
in vec3 v_normal;                   
in vec2 v_texcoord;  
layout(location=0) out vec4 fragColor;
out vec4 color;                     
void main(){                        
	vec3 lDir = normalize(vec3(0.0, 0.0, 1.0));          
	vec3 normal = normalize(v_normal);                   
	//vec3 eye = normalize(-v_position);                   
	//vec3 H = normalize(eye + lDir);                      
	//float specfactor = pow(max(0.0, dot(normal, H)), u_shininess);      
	float diffactor = max(0.0, dot(normal, lDir));                      
	fragColor = diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord) + u_ambient; 
}