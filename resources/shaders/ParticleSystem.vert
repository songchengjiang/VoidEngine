layout (location = ATTR_POSITION) in vec3 position;
layout (location = ATTR_TEXTURE_COORD0) in vec2 texcoord0;
layout (location = ATTR_COLOR0) in vec4 color;
layout (location = ATTR_MVP_MAT) in mat4 modelViewProjectMat;

out vec4 v_color;
out vec2 v_texcoord;

void main()                                                 
{
    v_color = color;
	v_texcoord = texcoord0;
	gl_Position = modelViewProjectMat * vec4(position, 1.0);
}