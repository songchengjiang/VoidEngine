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

vec2 caculateCoordsWithLatLong(vec3 D) {
    return vec2((atan(-D.z, D.x) / PI + 1.0) * 0.5,
                (asin(-D.y) / PI + 0.5));
}

void main(){
	vec3 normal = normalize(v_worldNormal);
	vec2 coords = caculateCoordsWithLatLong(normal);
	vec3 r = normalize(reflect(normalize(-v_viewDir), normal));
	vec2 specCoords = caculateCoordsWithLatLong(r);

	fragColor = texture(u_IBLDiffMap, coords) + textureLod(u_IBLSpecMap, specCoords, 0.0);
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0) * fragColor;
}