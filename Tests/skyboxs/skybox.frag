in vec3 v_texcoord;
layout(location = 0) out vec4 fragColor;
uniform samplerCube u_cubeMap;
void main() {
	fragColor = texture(u_cubeMap, v_texcoord);
}