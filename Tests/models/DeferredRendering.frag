#version 430
uniform sampler2D u_position;
uniform sampler2D u_normalandepth;
uniform sampler2D u_color;
uniform vec3 u_lightPos;
in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

float PointLight(in vec3 lp, in vec3 pos, in vec3 norm){
	vec3 dir = normalize(lp - pos);
	return max(0.0, dot(dir, norm));
}

void main() {
	vec3 position = texture(u_position, v_texcoord).xyz;
	vec4 normdepth = texture(u_normalandepth, v_texcoord);
	vec3 norm = normalize(normdepth.xyz);

	fragColor = texture(u_color, v_texcoord) * PointLight(u_lightPos, position, norm);
}