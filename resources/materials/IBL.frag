uniform sampler2D u_IBLEnvMap;
uniform sampler2D u_IBLDiffMap;
uniform sampler2D u_IBLSpecMap;
in vec3 v_viewDir;
in vec3 v_worldNormal;
layout(location=0) out vec4 fragColor;    

const float PI = 3.1415926535;

vec2 caculateIBLUV(vec3 D){ //see http://www.pauldebevec.com/Probes/
	float r = (1.0 / PI) * acos(D.z) / sqrt(D.x * D.x + D.y * D.y);
	return vec2(D.x * r, D.y * r) * 0.5 + 0.5;
}         
       
void main(){
	vec3 normal = normalize(v_worldNormal);
	vec2 coords = caculateIBLUV(normal);
	vec3 r = normalize(reflect(normalize(-v_viewDir), normal));
	vec2 specCoords = caculateIBLUV(r);

	fragColor = texture(u_IBLEnvMap, coords) * 0.4 + texture(u_IBLDiffMap, coords) * 0.6 + texture(u_IBLSpecMap, specCoords) * 0.4;
}