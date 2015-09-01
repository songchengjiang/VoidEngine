#version 430                                                
layout (location = 0) in vec3 position;                     
layout (location = 1) in vec3 normal;                                           
layout (location = 4) in vec2 texcoord;                     
uniform mat4 u_ModelViewProjectMat;   
uniform mat4 u_ModelViewMat;                        
uniform mat3 u_NormalMat;     
out vec3 v_position;                              
out vec3 v_normal;                   
out vec2 v_texcoord;  
out float v_linearDepth;  
void main()                                                 
{
	vec4 pos = vec4(position, 1.0);
    v_position = (u_ModelViewMat * pos).xyz;                                        
	v_normal = u_NormalMat * normal;                          
	v_texcoord = texcoord; 
	gl_Position = u_ModelViewProjectMat * pos; 
	v_linearDepth = gl_Position.w;
}