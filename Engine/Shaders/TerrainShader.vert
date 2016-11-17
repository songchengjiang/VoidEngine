#define STRINGIFY(A)  #A

const char *TERRAIN_VERTEX_SHADER = STRINGIFY(
layout (location = ATTR_POSITION) in vec3 position;
layout (location = ATTR_NORMAL) in vec3 normal;
layout (location = ATTR_TANGENT) in vec3 tangent;
layout (location = ATTR_BITANGENT) in vec3 bitangent;
layout (location = ATTR_TEXTURE_COORD0) in vec2 texcoord0;
                 
uniform mat4 u_ModelViewProjectMat;
uniform mat4 u_ModelViewMat;
uniform mat3 u_NormalMat;
   
out vec3 v_viewNormal;
\n#ifdef VE_USE_NROMAL_MAPPING\n
out vec3 v_viewTangent;
out vec3 v_viewBitangent;
\n#endif\n

\n#ifdef VE_USE_PARALLAX_MAPPING\n
out vec3 v_viewDir;
\n#endif\n
            
out vec2 v_texcoord;

void main()
{
    vec4 finalPos = vec4(position, 1.0);
    vec3 finalNorm = normal;
\n#ifdef VE_USE_NROMAL_MAPPING\n
    vec3 finalTangent = tangent;
    vec3 finalBitangent = bitangent;
    v_viewTangent = normalize(u_NormalMat * finalTangent);
    v_viewBitangent = normalize(u_NormalMat * finalBitangent);
\n#endif\n
	v_viewNormal = normalize(u_NormalMat * finalNorm);
    
	v_texcoord = texcoord0;

\n#ifdef VE_USE_PARALLAX_MAPPING\n
	vec3 viewPosition = (u_ModelViewMat * finalPos).xyz;
	vec3 viewDir = normalize(-viewPosition);
	v_viewDir.x = dot(viewDir, v_viewTangent);
	v_viewDir.y = dot(viewDir, v_viewBitangent);
	v_viewDir.z = dot(viewDir, v_viewNormal);
\n#endif\n

	gl_Position = u_ModelViewProjectMat * finalPos; 
}
);