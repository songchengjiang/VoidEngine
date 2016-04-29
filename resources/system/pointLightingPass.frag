uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;
uniform samplerCubeShadow u_shadowTex;
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform mat4 u_InvViewProjectMat;
uniform mat4 u_lightMatrix;
uniform vec3 u_cameraPosInWorld;

uniform vec3  u_lightPosition;
uniform vec3  u_lightColor;
uniform float u_lightIntensity;
uniform float u_lightARI;

uniform float u_lightShadowEnabled;
uniform float u_lightShadowBias;
uniform float u_lightShadowStrength;
uniform float u_lightShadowSoft;
uniform float u_lightShadowSoftness;

layout(location=0) out vec4 fragColor;

#define SAMPLE_SIZE 20
const vec3 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec3[]
(
   vec3( 1.0,  1.0,  1.0), vec3( 1.0, -1.0,  1.0), vec3(-1.0, -1.0,  1.0), vec3(-1.0,  1.0,  1.0), 
   vec3( 1.0,  1.0, -1.0), vec3( 1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0,  1.0, -1.0),
   vec3( 1.0,  1.0,  0.0), vec3( 1.0, -1.0,  0.0), vec3(-1.0, -1.0,  0.0), vec3(-1.0,  1.0,  0.0),
   vec3( 1.0,  0.0,  1.0), vec3(-1.0,  0.0,  1.0), vec3( 1.0,  0.0, -1.0), vec3(-1.0,  0.0, -1.0),
   vec3( 0.0,  1.0,  1.0), vec3( 0.0, -1.0,  1.0), vec3( 0.0, -1.0, -1.0), vec3( 0.0,  1.0, -1.0)
);

#if VE_PLATFORM == VE_PLATFORM_ANDROID
vec3 decode(vec2 encoded)
{
    vec2 fenc = encoded * 4.0 - 2.0;
    float f = dot(fenc,fenc);
    float g = sqrt(1.0 - f / 4.0);
    vec3 normal;
    normal.xy = fenc*g;
    normal.z = 1.0 - f / 2.0;
    return normal;
}
#endif

float shadowing(vec3 vertex){
	if (u_lightShadowEnabled < 1.0) return 1.0;
	vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);
	float pTolDis2 = dot(projectVertex.xyz, projectVertex.xyz);
	pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);
	vec4 shadowCoord = vec4(projectVertex.xyz, pTolDis2 - u_lightShadowBias);
	float factor = 0.0;
	if (0.0 < u_lightShadowSoft){
		float sum = 0.0;
		for (int i = 0; i < SAMPLE_SIZE; ++i){                                                                                                                                                                 
			sum += texture(u_shadowTex, vec4(shadowCoord.xyz + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.w));                                                                                                              
		}                                                                              
		factor = sum / float(SAMPLE_SIZE);
	}else{
		factor = texture(u_shadowTex, shadowCoord);
	}

	if (factor < 1.0)
		factor = mix(1.0, factor, u_lightShadowStrength);
	return factor;
}

void main(){
	vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
#if VE_PLATFORM == VE_PLATFORM_ANDROID
	vec3 normal = decode(texture(u_normalTex, texCoords).xy);
#else
	vec3 normal = texture(u_normalTex, texCoords).xyz;
#endif
	float depth = texture(u_depthTex, texCoords).r;
	vec4 posInWorld= u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	posInWorld.xyz /= posInWorld.w;
	vec3 eyeDir = normalize(u_cameraPosInWorld - posInWorld.xyz);
	vec3 lightDir = u_lightPosition - posInWorld.xyz;
	vec3 attDis = lightDir * u_lightARI;
	float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);
	lightDir = normalize(lightDir);

	float NdotL = max(0.0, dot(normal, lightDir));
	vec3 H = normalize(eyeDir + lightDir);
	float NdotH = max(0.0, dot(normal, H));

	fragColor = clamp(vec4(u_lightColor.r, u_lightColor.g, u_lightColor.b, NdotH) * NdotL * u_lightIntensity * attenuation * shadowing(posInWorld.xyz), 0.0, 1.0);
}