layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord; 
uniform mat4 u_ModelMat;
out vec2 v_texcoord; 
void main()
{
	v_texcoord = texcoord;
	gl_Position = u_ModelMat * vec4(position, 1.0);
}