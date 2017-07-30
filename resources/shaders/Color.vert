attribute vec3 a_position;
attribute vec2 a_texCoord0;

uniform mat4 u_ModelViewProjectMat;

varying vec2 v_texcoord;

void main()                                                 
{
	v_texcoord = a_texCoord0;
	gl_Position = u_ModelViewProjectMat * vec4(a_position, 1.0);
}
