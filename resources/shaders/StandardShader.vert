attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec3 a_binormal;
attribute vec2 a_texCoord0;
                 
uniform mat4 u_ModelViewProjectMat;
uniform mat4 u_ModelViewMat;
uniform mat4 u_ModelMat;
uniform mat4 u_InvViewMat;
uniform mat3 u_NormalMat;
uniform mat3 u_NormalWorldMat;
uniform vec3 u_cameraWorldPos;

varying vec3 v_worldPosition;
varying vec3 v_worldNormal;
#ifdef VE_USE_NORMAL_TEXTURE
varying vec3 v_worldTangent;
varying vec3 v_worldBinormal;
#endif

varying vec2 v_texcoord;

void main()                                                 
{   
	vec4 finalPosition = vec4(a_position, 1.0);
	vec3 finalNormal   = a_normal;
	vec3 finalTangent  = a_tangent;
	vec3 finalBinormal = a_binormal;
#ifdef VE_USE_BONES
    simulationBoneTransform(finalPosition, finalNormal, finalTangent, finalBinormal);
#endif

#ifdef VE_USE_NORMAL_TEXTURE
	v_worldTangent = u_NormalWorldMat * finalTangent;
	v_worldBinormal = u_NormalWorldMat * finalBinormal;
#endif
	v_worldNormal = u_NormalWorldMat * finalNormal;
    
    v_worldPosition = (u_ModelMat * finalPosition).xyz;

    v_texcoord = a_texCoord0;
    
	gl_Position = u_ModelViewProjectMat * finalPosition;
}
