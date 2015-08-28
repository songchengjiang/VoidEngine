#version 430                                                
layout (location = 0) in vec3 position;                     
layout (location = 1) in vec3 normal;                       
layout (location = 2) in vec3 tangent;                       
layout (location = 3) in vec3 bitangent;                       
layout (location = 4) in vec2 texcoord;                     
uniform mat4 u_ModelViewProjectMat;                         
uniform mat3 u_NormalMat;                                   
out vec3 v_normal;                   
out vec2 v_texcoord;  
void main()                                                 
{                                                           
	v_normal = u_NormalMat * normal;                          
	v_texcoord = texcoord; 
	gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); 
}