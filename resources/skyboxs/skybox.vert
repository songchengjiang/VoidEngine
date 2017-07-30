attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord0;
uniform mat4 u_ModelViewProjectMat;
varying vec3 v_texcoord;
void main()
{
	v_texcoord =  vec3(a_position.x, a_position.yz); //see http://www.3dcpptutorials.sk/index.php?id=24
	gl_Position = u_ModelViewProjectMat * vec4(a_position, 1.0);
}
