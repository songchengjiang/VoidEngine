layout(location=0) out vec4 fragColor;
in float v_linearDepth;             
void main(){
	fragColor = vec4(v_linearDepth / (v_linearDepth + 1.0));
}