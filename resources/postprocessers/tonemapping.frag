uniform sampler2D u_colorTexure;
uniform float u_middleGrey;
uniform float u_key;
uniform float u_white;

in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {

	vec4 color = texture(u_colorTexure, v_texcoord);
	vec4 L = (u_key / u_middleGrey) * color;
	vec4 ld = (L * (1.0 + L / u_white)) / (1.0 + L);

	fragColor = vec4(clamp(ld.xyz, 0.0, 1.0), 1.0);
}