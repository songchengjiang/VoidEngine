#version 430                                                
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

vec4 updateBonePosition()
{
	vec4 weights = boneWeights;
	weights.w = 1.0 - dot(weights.xyz, vec3(1.0));
	vec4 localPos = vec4(position, 1.0);
	vec4 bonePos = u_BoneMates[int(boneIndices.x)] * localPos * weights.x;
	bonePos += u_BoneMates[int(boneIndices.y)] * localPos * weights.y;
	bonePos += u_BoneMates[int(boneIndices.z)] * localPos * weights.z;
	bonePos += u_BoneMates[int(boneIndices.w)] * localPos * weights.w;
	return bonePos;
}

void main()                                                 
{   
	vec4 finalPos = updateBonePosition();                     
	v_normal = u_NormalMat * normal;                          
	v_texcoord = texcoord; 
	gl_Position = u_ModelViewProjectMat * finalPos; 
}