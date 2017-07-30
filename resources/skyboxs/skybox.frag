varying vec3 v_texcoord;
uniform samplerCube u_cubeMap;
void main() {
	gl_FragColor = textureCube(u_cubeMap, v_texcoord);
}
