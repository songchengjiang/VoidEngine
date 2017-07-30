attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord0;
attribute vec3 a_tangent;
attribute vec3 a_binormal;

uniform mat4 u_ModelViewProjectMat;   
uniform mat4 u_ModelMat;
uniform mat4 u_ModelViewMat;
uniform mat3 u_NormalWorldMat;
uniform mat3 u_NormalMat;
uniform vec3 u_cameraPos;

varying vec3 v_viewDir;
varying vec3 v_worldNormal;

varying vec3 v_viewNormal;
                    
void main()                                                 
{   
	vec4 worldVertex = u_ModelMat * vec4(a_position, 1.0);
	v_viewDir = u_cameraPos - worldVertex.xyz;
	v_worldNormal = u_NormalWorldMat * a_normal;
    
    v_viewNormal = u_NormalMat * a_normal;
    //v_viewDir = -(u_ModelViewMat * vec4(a_position, 1.0)).xyz;
    
    
	gl_Position = u_ModelViewProjectMat * vec4(a_position, 1.0);
}
