                                          
layout (location = 0) in vec3 position;                     
layout (location = 1) in vec3 normal;                       
layout (location = 2) in vec3 tangent;                       
layout (location = 3) in vec3 bitangent;                       
layout (location = 4) in vec2 texcoord;
layout (location = 5) in vec4 boneIndices;
layout (location = 6) in vec4 boneWeights;      
uniform mat4 u_ModelViewProjectMat;                         
uniform mat3 u_NormalMat;
uniform mat4 u_BoneMates[60];                        
out vec3 v_normal;                   
out vec2 v_texcoord;

void updateBonePositionAndNormal(out vec4 pos, out vec3 norm)
{
	vec4 weights = boneWeights;
	weights.w = 1.0 - dot(weights.xyz, vec3(1.0));
	mat4 boneMat = u_BoneMates[int(boneIndices.x)] * weights.x;
	boneMat += u_BoneMates[int(boneIndices.y)] * weights.y;
	boneMat += u_BoneMates[int(boneIndices.z)] * weights.z;
	boneMat += u_BoneMates[int(boneIndices.w)] * weights.w;
	pos = boneMat * vec4(position, 1.0);
	norm = (boneMat * vec4(normal, 0.0)).xyz;
}

void main()                                                 
{   
	vec4 finalPos;
	vec3 finalNorm;
	updateBonePositionAndNormal(finalPos, finalNorm);                     
	v_normal = u_NormalMat * finalNorm;                          
	v_texcoord = texcoord; 
	gl_Position = u_ModelViewProjectMat * finalPos; 
}