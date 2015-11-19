uniform samplerCube u_cubeMap;
in vec3 v_reflect; 
layout(location=0) out vec4 fragColor;                
void main(){
	fragColor = texture(u_cubeMap, vec3(v_reflect.x, -v_reflect.yz));
}