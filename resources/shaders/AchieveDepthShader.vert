layout (location = ATTR_POSITION) in vec3 position;
#ifdef ATTR_BONE_INDICES
layout (location = ATTR_BONE_INDICES) in vec4 boneIndices;
#endif 
#ifdef ATTR_BONE_WEIGHTS
layout (location = ATTR_BONE_WEIGHTS) in vec4 boneWeights; 
#endif 
                 
uniform mat4 u_ModelViewProjectMat;              
#ifdef VE_USE_BONES
uniform mat4 u_BoneMates[60];
void updateBonePosition(out vec4 pos)
{
	vec4 weights = boneWeights;
	weights.w = 1.0 - dot(weights.xyz, vec3(1.0));
	mat4 boneMat = u_BoneMates[int(boneIndices.x)] * weights.x;
	boneMat += u_BoneMates[int(boneIndices.y)] * weights.y;
	boneMat += u_BoneMates[int(boneIndices.z)] * weights.z;
	boneMat += u_BoneMates[int(boneIndices.w)] * weights.w;
	pos = boneMat * vec4(position, 1.0);
}
#endif

void main()                                                 
{   
	vec4 finalPos;
#ifdef VE_USE_BONES
	updateBonePosition(finalPos);
#else	  
	finalPos = vec4(position, 1.0);
#endif
	  
	gl_Position = u_ModelViewProjectMat * finalPos;
}