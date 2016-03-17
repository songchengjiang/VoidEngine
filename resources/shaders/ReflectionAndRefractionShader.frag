uniform samplerCube u_cubeMap;
in vec3 v_reflect; 
layout(location=0) out vec4 RT0;
layout(location=1) out vec4 RT1;
layout(location=2) out vec4 RT2;
void main(){
	RT0 = vec4(0.0);
	RT1 = texture(u_cubeMap, vec3(v_reflect.x, -v_reflect.yz));
	RT2 = vec4(0.0);
	//fragColor = texture(u_cubeMap, vec3(v_reflect.x, -v_reflect.yz));
}