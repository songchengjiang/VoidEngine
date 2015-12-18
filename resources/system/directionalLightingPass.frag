uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform mat4 u_InvProjectMat;

uniform vec3  u_lightDirection;
uniform vec3  u_lightColor;
uniform float u_lightIntensity;

layout(location=0) out vec4 fragColor;

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

void main(){
	vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
#if VE_PLATFORM == VE_PLATFORM_ANDROID
	vec3 normal = decode(texture(u_normalTex, texCoords).xy);
#else
	vec3 normal = texture(u_normalTex, texCoords).xyz;
#endif
	float depth = texture(u_depthTex, texCoords).r;
	vec4 posInView = u_InvProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	posInView.xyz /= posInView.w;
	vec3 eyeDir = normalize(-posInView.xyz);
	float NdotL = max(0.0, dot(normal, -u_lightDirection));
	vec3 H = normalize(eyeDir - u_lightDirection);
	float NdotH = max(0.0, dot(normal, H));
	fragColor = clamp(vec4(NdotL * u_lightColor.r, NdotL * u_lightColor.g, NdotL * u_lightColor.b, NdotL * NdotH) * u_lightIntensity, 0.0, 1.0);
}