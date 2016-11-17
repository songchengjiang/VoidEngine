#define STRINGIFY(A)  #A

const char *SQUARE_DEPTH_WRITE_FRAGMENT_SHADER = STRINGIFY(
in highp vec3 v_posInLightView;
void main(){
	highp float pTolDis2 = dot(v_posInLightView, v_posInLightView);
	gl_FragDepth = pTolDis2 / (pTolDis2 + 1.0);
}
);