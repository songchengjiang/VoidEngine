uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform mat4 u_InvProjectMat;

uniform vec3  u_lightPosition;
uniform vec3  u_lightColor;
uniform float u_lightIntensity;
uniform float u_lightARI;

layout(location=1) out vec4 fragColor;

void main(){
	vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
	vec3 normal = texture(u_normalTex, texCoords).xyz;
	float depth = texture(u_depthTex, texCoords).r;
	vec4 posInView = u_InvProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	vec3 eyeDir = normalize(-posInView.xyz);
	vec3 lightDir = u_lightPosition - posInView.xyz;
	vec3 attenuation = lightDir * u_lightARI;
	attenuation = clamp(1.0 - dot(attenuation, attenuation), 0.0, 1.0);
	lightDir = normalize(lightDir);

	float NdotL = max(0.0, dot(normal, lightDir));
	vec3 H = normalize(eyeDir + lightDir);
	float NdotH = max(0.0, dot(normal, H));

	fragColor = vec4(NdotL * u_lightColor.r, NdotL * u_lightColor.g, NdotL * u_lightColor.b, NdotL * NdotH) * attenuation;
}