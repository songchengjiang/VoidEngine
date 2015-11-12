uniform sampler2D u_colorTex;
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {
	vec4 color = texture(u_colorTex, v_texcoord);
	float grey = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
	fragColor = vec4(grey, grey, grey, 1.0);
}