#include "Common.glsl"

const char *DEPTH_WRITE_VERTEX_SHADER = STRINGIFY(
layout (location = ATTR_POSITION) in vec3 position;
\n#ifdef ATTR_BONE_INDICES\n
layout (location = ATTR_BONE_INDICES) in vec4 boneIndices;
\n#endif\n
\n#ifdef ATTR_BONE_WEIGHTS\n
layout (location = ATTR_BONE_WEIGHTS) in vec4 boneWeights; 
\n#endif\n
                 
uniform mat4 u_ModelViewProjectMat;              
\n#ifdef VE_USE_BONES\n
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
\n#endif\n

//out float v_depthInView;

void main()                                                 
{   
	vec4 finalPos;
\n#ifdef VE_USE_BONES\n
	updateBonePosition(finalPos);
\n#else\n
	finalPos = vec4(position, 1.0);
\n#endif\n
	  
	gl_Position = u_ModelViewProjectMat * finalPos;
	//v_depthInView = gl_Position.w;
}
);

const char *DEPTH_WRITE_FRAGMENT_SHADER = STRINGIFY(
//in float v_depthInView;
void main(){
    //gl_FragDepth = v_depthInView / (v_depthInView + 1.0);
}
);