uniform sampler2D u_Texture;
uniform float u_Threshold;
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

float grey(vec3 val){
	return 0.2126 * val.r + 0.7152 * val.g + 0.0722 * val.b; 
}

void main()
{
	vec4 color = texture(u_Texture, v_texcoord);
	if (u_Threshold < grey(color.rgb))
		fragColor = color;
	//fragColor = color * clamp(grey(color.rgb) - u_Threshold, 0.0, 1.0);
}
