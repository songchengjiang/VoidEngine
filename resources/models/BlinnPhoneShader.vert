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
uniform mat4 u_ModelViewMat;                       
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

#ifdef VE_USE_TRANSFORMFEEDBACK
out vec3 tf_position;
out vec3 tf_normal;
#endif

#endif

out vec4 v_position;          
out vec4 v_normalAndepth;                   
out vec2 v_texcoord;

#ifdef VE_USE_LIGHTS

#ifdef VE_DIRECTIONAL_LIGHT_MAX_NUM
out vec4 v_directionalLightST[VE_DIRECTIONAL_LIGHT_MAX_NUM];
#endif

#ifdef VE_POINT_LIGHT_MAX_NUM
out vec3 v_pointLightST[VE_POINT_LIGHT_MAX_NUM];
#endif

#ifdef VE_SPOT_LIGHT_MAX_NUM
out vec4 v_spotLightST[VE_SPOT_LIGHT_MAX_NUM];
#endif

#endif

#ifdef VE_USE_LIGHTS
void caculateShadowTextureCoord(in vec4 position)
{
#ifdef VE_DIRECTIONAL_LIGHT_MAX_NUM
	for (int i = 0; i < ve_directionalLightNum; ++i){
		if (0.0 < ve_directionalLight[i].shadowEnabled){
			v_directionalLightST[i] = ve_directionalLight[i].lightMat * position;
		}
	}
#endif

#ifdef VE_POINT_LIGHT_MAX_NUM
	for (int i = 0; i < ve_pointLightNum; ++i){
		if (0.0 < ve_pointLight[i].shadowEnabled){
			v_pointLightST[i] = (ve_pointLight[i].lightMat * position).xyz;
		}
	}
#endif

#ifdef VE_SPOT_LIGHT_MAX_NUM
	for (int i = 0; i < ve_spotLightNum; ++i){
		if (0.0 < ve_spotLight[i].shadowEnabled){
			v_spotLightST[i] = ve_spotLight[i].lightMat * position;
		}
	}
#endif

}
#endif

void main()                                                 
{   
	vec4 finalPos;
	vec3 finalNorm;
#ifdef VE_USE_BONES
	updateBonePositionAndNormal(finalPos, finalNorm);
#ifdef VE_USE_TRANSFORMFEEDBACK
	tf_position = finalPos.xyz;
	tf_normal = finalNorm;
#endif
#else	  
	finalPos = vec4(position, 1.0);
	finalNorm = normal;
#endif
	
	v_texcoord = texcoord0;
	v_position = u_ModelViewMat * finalPos; 

#ifdef VE_USE_LIGHTS
	caculateShadowTextureCoord(finalPos);
#endif
	  
	gl_Position = u_ModelViewProjectMat * finalPos; 
	v_normalAndepth = vec4(u_NormalMat * finalNorm, gl_Position.w);  
}