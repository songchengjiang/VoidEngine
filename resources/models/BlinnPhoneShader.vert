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
#ifdef ATTR_BONE_INDICES
layout (location = ATTR_BONE_INDICES) in vec4 boneIndices;
#endif 
#ifdef ATTR_BONE_WEIGHTS
layout (location = ATTR_BONE_WEIGHTS) in vec4 boneWeights; 
#endif 
                 
uniform mat4 u_ModelViewProjectMat;
uniform mat3 u_NormalWorldMat;
#ifdef VE_USE_BONES
uniform mat4 u_BoneMates[60];
void updateBonePositionAndNormal(out vec4 pos, out vec3 norm, out vec3 tan, out vec3 bitan)
{
	vec4 weights = boneWeights;
	weights.w = 1.0 - dot(weights.xyz, vec3(1.0));
	mat4 boneMat = u_BoneMates[int(boneIndices.x)] * weights.x;
	boneMat += u_BoneMates[int(boneIndices.y)] * weights.y;
	boneMat += u_BoneMates[int(boneIndices.z)] * weights.z;
	boneMat += u_BoneMates[int(boneIndices.w)] * weights.w;
	pos = boneMat * vec4(position, 1.0);
	norm = (boneMat * vec4(normal, 0.0)).xyz;
#ifdef VE_USE_NROMAL_MAPPING
	tan = (boneMat * vec4(tangent, 0.0)).xyz;
	bitan = (boneMat * vec4(bitangent, 0.0)).xyz;
#endif
}

out vec3 tf_position;
out vec3 tf_normal;

#endif
   
out vec3 v_worldNormal;
#ifdef VE_USE_NROMAL_MAPPING
out vec3 v_worldTangent;
out vec3 v_worldBitangent;    
#endif
            
out vec2 v_texcoord;

void main()                                                 
{   
	vec4 finalPos;
	vec3 finalNorm;
	vec3 finalTangent;
	vec3 finalBitangent;
#ifdef VE_USE_BONES
	updateBonePositionAndNormal(finalPos, finalNorm, finalTangent, finalBitangent);
	tf_position = finalPos.xyz;
	tf_normal = finalNorm;
#else
	finalPos = vec4(position, 1.0);
	finalNorm = normal;
#endif
	
	v_texcoord = texcoord0;
	v_worldNormal = normalize(u_NormalWorldMat * finalNorm);  
#ifdef VE_USE_NROMAL_MAPPING
	v_worldTangent = normalize(u_NormalWorldMat * finalTangent);
	v_worldBitangent = normalize(u_NormalWorldMat * finalBitangent);  
#endif

	gl_Position = u_ModelViewProjectMat * finalPos; 
}