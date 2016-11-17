#define STRINGIFY(A)  #A

const char *DEPTH_WRITE_FRAGMENT_SHADER = STRINGIFY(
//in float v_depthInView;
void main(){
	//gl_FragDepth = v_depthInView / (v_depthInView + 1.0);
}
);