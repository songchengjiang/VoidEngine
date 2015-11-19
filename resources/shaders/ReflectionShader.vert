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
uniform mat4 u_ModelViewMat;   
uniform mat4 u_InvViewMat;
uniform mat3 u_NormalMat;     
out vec3 v_reflect;                              
void main()                                                 
{
	vec4 pos = vec4(position, 1.0);
    vec3 ePosition = (u_ModelViewMat * pos).xyz;                                        
	vec3 eNormal = u_NormalMat * normal;
	v_reflect = reflect(normalize(ePosition), normalize(eNormal));
	v_reflect = (u_InvViewMat * vec4(v_reflect, 0.0)).xyz;
	gl_Position = u_ModelViewProjectMat * pos;
}