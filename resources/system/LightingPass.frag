in vec3 v_normInView;

layout(location=0) out vec4 normal;

void main(){
	normal = vec4(v_normInView, 1.0);
}