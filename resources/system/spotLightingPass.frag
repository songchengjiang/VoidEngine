uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform mat4 u_InvProjectMat;

uniform vec3  u_lightPosition;
uniform vec3  u_lightDirection;
uniform vec3  u_lightColor;
uniform float u_lightIntensity;
uniform float u_lightARI;
uniform float u_lightInnerAngleCos;
uniform float u_lightOuterAngleCos;

layout(location=0) out vec4 fragColor;

void main(){
	vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
	vec4 normAndshininess = texture(u_normalTex, texCoords);
	vec3 normal = normAndshininess.xyz;
	float shininess = normAndshininess.w;
	float depth = texture(u_depthTex, texCoords).r;
	vec4 posInView = u_InvProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	posInView.xyz /= posInView.w;
	vec3 eyeDir = normalize(-posInView.xyz);
	vec3 lightDir = u_lightPosition - posInView.xyz;
	vec3 attDis = lightDir * u_lightARI;
	float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);
	lightDir = normalize(lightDir);
	float currentAngleCos = dot(lightDir, -u_lightDirection);
	attenuation *= smoothstep(u_lightOuterAngleCos, u_lightInnerAngleCos, currentAngleCos);

	float NdotL = max(0.0, dot(normal, lightDir));
	vec3 H = normalize(eyeDir + lightDir);
	float NdotH = max(0.0, dot(normal, H));

	fragColor = vec4(NdotL * u_lightColor.r, NdotL * u_lightColor.g, NdotL * u_lightColor.b, NdotL * NdotH) * u_lightIntensity * attenuation;
}