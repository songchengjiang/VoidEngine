layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord; 
uniform mat4 u_ModelViewProjectMat;
out vec3 v_texcoord;
void main()
{
	v_texcoord =  vec3(position.x, -position.yz); //see http://www.3dcpptutorials.sk/index.php?id=24
	gl_Position = u_ModelViewProjectMat * vec4(position, 1.0);
}