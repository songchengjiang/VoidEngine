uniform sampler2D u_radianceTexture;
varying vec3 v_viewDir;
varying vec3 v_worldNormal;

varying vec3 v_viewNormal;

void main(){
	vec3 normal = normalize(v_worldNormal);
	vec2 coords = caculateCoordsWithLatLong(normal);
	vec3 r = normalize(reflect(normalize(-v_viewDir), normal));
	vec2 specCoords = caculateCoordsWithLatLong(r);
    
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    //normal = normalize(v_viewNormal);
    vec3 viewDir = normalize(v_viewDir);
    float NdotL = max(0.0, dot(normal, lightDir));
    vec3 H = normalize(lightDir + viewDir);
    float NdotH = max(0.0, dot(normal, H));
    
    

	//gl_FragColor = texture2D(u_radianceTexture, coords);
    gl_FragColor = vec4(NdotL + pow(NdotH, 128.0));
}
