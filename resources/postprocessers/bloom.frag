uniform sampler2D u_extractlightTex;
uniform sampler2D u_colorTex;
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main()
{
	vec4 color = texture(u_extractlightTex, v_texcoord) + texture(u_colorTex, v_texcoord);
	fragColor = color;
}
