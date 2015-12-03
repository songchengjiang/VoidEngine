in vec3 v_posInLightView; 
void main(){
	gl_FragDepth = length(v_posInLightView);
}