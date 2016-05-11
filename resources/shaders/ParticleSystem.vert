layout (location = ATTR_POSITION) in vec3 position;
layout (location = ATTR_TEXTURE_COORD0) in vec2 texcoord0;
                 
uniform mat4 ve_offsetMats[100];
uniform vec4 ve_colors[100];
uniform mat4 u_ModelViewProjectMat;

out vec4 v_color;
out vec2 v_texcoord;

void main()                                                 
{
    v_color = ve_colors[gl_InstanceID];
	v_texcoord = texcoord0;
	gl_Position = u_ModelViewProjectMat * ve_offsetMats[gl_InstanceID] * vec4(position, 1.0);
}