#version 430
uniform sampler2D u_position;
uniform sampler2D u_normalandepth;
uniform sampler2D u_color;
in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;
out vec4 color; 
void main() {
	fragColor = texture(u_color, v_texcoord);
}