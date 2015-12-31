uniform sampler2D u_IBLMap;
in vec3 v_worldNormal;
layout(location=0) out vec4 fragColor;    

const float PI = 3.1415926535;
vec2 caculateIBLUV(vec3 D){ //see http://www.pauldebevec.com/Probes/
	float r = (1.0 / PI) * acos(D.z) / sqrt(D.x * D.x + D.y * D.y);
	return vec2(D.x * r, D.y * r) * 0.5 + 0.5;
}         
       
void main(){
	vec2 coords = caculateIBLUV(normalize(v_worldNormal));
	fragColor = texture(u_IBLMap, coords);
}