layout (location = ATTR_POSITION) in vec3 position;
layout (location = ATTR_NORMAL) in vec3 normal;
#ifdef ATTR_BONE_INDICES
layout (location = ATTR_BONE_INDICES) in vec4 boneIndices;
#endif 
#ifdef ATTR_BONE_WEIGHTS
layout (location = ATTR_BONE_WEIGHTS) in vec4 boneWeights; 
#endif 
                 
uniform mat4 u_ModelViewProjectMat;
uniform mat3 u_NormalMat;            
#ifdef VE_USE_BONES
uniform mat4 u_BoneMates[60];
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
#endif

out vec3 v_normInView;

void main()                                                 
{   
	vec4 finalPos;
	vec3 finalNorm;
#ifdef VE_USE_BONES
	updateBonePositionAndNormal(finalPos, finalNorm);
#else	  
	finalPos = vec4(position, 1.0);
	finalNorm = normal;
#endif
	  
	gl_Position = u_ModelViewProjectMat * finalPos;
	v_normInView = u_NormalMat * finalNorm;  
}