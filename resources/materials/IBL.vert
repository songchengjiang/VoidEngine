layout (location = ATTR_POSITION) in vec3 position;                     
layout (location = ATTR_NORMAL) in vec3 normal;
#ifdef ATTR_TEXTURE_COORD0
layout (location = ATTR_TEXTURE_COORD0) in vec2 texcoord0; 
#endif 
#ifdef ATTR_TEXTURE_COORD1
layout (location = ATTR_TEXTURE_COORD1) in vec2 texcoord1; 
#endif 
#ifdef ATTR_COLOR0
layout (location = ATTR_COLOR0) in vec4 color0; 
#endif 
#ifdef ATTR_TANGENT
layout (location = ATTR_TANGENT) in vec3 tangent;
#endif          
#ifdef ATTR_BITANGENT     
layout (location = ATTR_BITANGENT) in vec3 bitangent;   
#endif
                  
uniform mat4 u_ModelViewProjectMat;   
uniform mat4 u_ModelMat;
uniform mat3 u_NormalWorldMat;
uniform vec3 u_cameraPos; 

out vec3 v_viewDir;
out vec3 v_worldNormal;
                    
void main()                                                 
{   
	vec4 worldVertex = u_ModelMat * vec4(position, 1.0);
	v_viewDir = u_cameraPos - worldVertex.xyz;
	v_worldNormal = u_NormalWorldMat * normal;
	gl_Position = u_ModelViewProjectMat * vec4(position, 1.0);
}