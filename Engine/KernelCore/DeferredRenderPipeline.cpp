#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"
#include "Configuration.h"

static const char* COMMON_FUNCTIONS = " \
	precision highp float;           \n \
	vec3 decode(vec3 encoded) {     \n \
        float nz = floor(encoded.z * 255.0) / 16.0;     \n \
        vec2 dec = encoded.xy + vec2(floor(nz) / 16.0, fract(nz)) / 255.0;     \n \
		vec2 fenc = dec * 4.0 - 2.0;    \n \
		float f = dot(fenc, fenc);    \n \
		float g = sqrt(1.0 - f / 4.0);    \n \
		return vec3(fenc * g, 1.0 - f / 2.0);    \n \
	}    \n \
                                                                         \n\
    vec3 linearColor(vec3 col) {    \n \
        return pow(col, vec3(0.45454545));    \n \
    }       \n \
                                                                                                                      \n \
	const float PI = 3.1415926535;                                                                                    \n\
	float OrenNayar(vec3 norm, vec3 ldir, vec3 vdir, float NdotL, float NdotV, float roughness) {                     \n\
			                                                                                                          \n\
		float roughness2 = roughness * roughness;                                                                     \n\
		float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));                                                     \n\
		float B = 0.45 * (roughness2 / (roughness2 + 0.09));                                                           \n\
		float C = max(0.0, dot(normalize(vdir - norm * NdotV), normalize(ldir - norm * NdotL)));                      \n\
		float NdotLAgl = acos(NdotL);                                                                                 \n\
		float NdotVAgl = acos(NdotV);                                                                                 \n\
		float alpha = max(NdotLAgl, NdotVAgl);                                                                        \n\
		float beta = min(NdotLAgl, NdotVAgl);                                                                         \n\
			                                                                                                          \n\
		return NdotL * max(0.0, (A + B * sin(alpha) * tan(beta) * C));                                                \n\
	}                                                                                                                 \n\
			                                                                                                          \n\
	float CookTorrance(float NdotL, float NdotV, float HdotV, float NdotH, float roughness, float fresnel) {        \n\
			float alpha = pow(1.0 - (1.0 - roughness) * 0.7, 6.0);                                                        \n\
			float alpha2 = alpha * alpha;                                                                                 \n\
			                                                                                                              \n\
			float k = (0.8 + 0.5 * alpha) * (0.8 + 0.5 * alpha) / 2.0;                                                    \n\
			float Gl = NdotL / (NdotL * (1.0 - k) + k);                                                                   \n\
			float Gv = NdotV / (NdotV * (1.0 - k) + k);                                                                   \n\
			float G = Gl * Gv;                                                                                            \n\
			                                                                                                              \n\
			float F = fresnel + (1.0 - fresnel) * pow(1.0 - HdotV, 5.0);                                                  \n\
			                                                                                                              \n\
			float Db = PI * pow(NdotH * NdotH * (alpha2 - 1.0) + 1.0, 2.0);                                               \n\
			float D = alpha2 / Db; 	                                                                                      \n\
			                                                                                                              \n\
			return max(0.0, (F * D * G) / (4.0f * NdotL * NdotV));                                                        \n\
	}                                                                                                                     \n\
	";


static const char* SCREEN_BASED_LIGHT_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_texcoord = texcoord; \n \
		gl_Position = vec4(position, 1.0); \n \
	}";

static const char* AMBIENT_LIGHT_F_SHADER = " \
    uniform sampler2D u_RT1; \n \
    uniform vec3 u_ambient;   \n \
    in highp vec2 v_texcoord; \n \
    layout(location = 0) out vec4 fragColor; \n \
    void main() {  \n \
        vec4 RT1 = texture(u_RT1, v_texcoord);    \n \
        fragColor = vec4(clamp(RT1.xyz * u_ambient, 0.0, 1.0), 1.0);     \n \
    }";

static const char* DIRECTIONAL_LIGHT_F_SHADER = " \
	uniform highp sampler2D u_depthTex; \n \
	uniform highp sampler2D u_RT0; \n \
	uniform sampler2D u_RT1; \n \
	uniform sampler2D u_RT2; \n \
	uniform sampler2DShadow u_shadowTex; \n \
												  \n \
	uniform mat4 u_InvViewMat;  \n \
	uniform mat4 u_InvViewProjectMat;  \n \
	uniform mat4 u_lightMatrix;    \n \
	uniform vec3 u_cameraPosInWorld;    \n \
												  \n \
	uniform vec3  u_lightDirection;   \n \
	uniform vec3  u_lightColor;    \n \
	uniform float u_lightIntensity;   \n \
												  \n \
	uniform float u_lightShadowEnabled;   \n \
	uniform float u_lightShadowBias;   \n \
	uniform float u_lightShadowStrength;   \n \
	uniform float u_lightShadowSoft;      \n \
	uniform float u_lightShadowSoftness;     \n \
												  \n \
	in highp vec2 v_texcoord; \n \
	layout(location = 0) out vec4 fragColor; \n \
												  \n \
	#define SAMPLE_SIZE 9  \n \
	const vec2 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec2[]    \n \
	(                                                       \n \
		vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),    \n \
		vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 1.0),    \n \
		vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)   \n \
	);                                                      \n \
												  \n \
	float shadowing(vec3 vertex) {          \n \
		if (u_lightShadowEnabled < 1.0) return 1.0;          \n \
		vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);          \n \
		vec3 shadowCoord = vec3(projectVertex.xy, projectVertex.z - u_lightShadowBias) / projectVertex.w;          \n \
		float factor = 0.0;          \n \
		if (0.0 < u_lightShadowSoft) {          \n \
			float sum = 0.0;          \n \
			for (int i = 0; i < SAMPLE_SIZE; ++i) {          \n \
				sum += texture(u_shadowTex, vec3(shadowCoord.xy + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.z));          \n \
			}          \n \
			factor = sum / float(SAMPLE_SIZE);          \n \
		}          \n \
		else {          \n \
			factor = texture(u_shadowTex, shadowCoord);          \n \
		}          \n \
												  \n \
		if (factor < 1.0)          \n \
			factor = mix(1.0, factor, u_lightShadowStrength);          \n \
		return factor;          \n \
	}          \n \
												  \n \
	void main() {  \n \
		vec4 RT0 = texture(u_RT0, v_texcoord);    \n \
		vec4 RT1 = texture(u_RT1, v_texcoord);    \n \
		vec4 RT2 = texture(u_RT2, v_texcoord);     \n \
		if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }             \n \
																							  \n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;   \n \
		highp float depth = texture(u_depthTex, v_texcoord).r;    \n \
		vec4 worldPosition = u_InvViewProjectMat * vec4(v_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
		worldPosition.xyz /= worldPosition.w;     \n \
		vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);    \n \
		float NdotL = max(0.0, dot(worldNormal, -u_lightDirection));    \n \
		vec3 H = normalize(eyeDir - u_lightDirection);    \n \
		float NdotH = max(0.0, dot(worldNormal, H));    \n \
		float NdotV = max(0.0, dot(worldNormal, eyeDir));    \n \
		float HdotV = max(0.0, dot(H, eyeDir));    \n \
		float diffFactor = OrenNayar(worldNormal, -u_lightDirection, eyeDir, NdotL, NdotV, RT1.w);     \n \
		float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, RT1.w, RT2.w);     \n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity * shadowing(worldPosition.xyz);    \n \
																							               \n \
		fragColor = vec4(clamp((linearColor(RT1.xyz) * diffFactor + linearColor(RT2.xyz) * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
	}";

static const char* IB_LIGHT_F_SHADER = " \
    uniform highp sampler2D u_depthTex; \n \
    uniform highp sampler2D u_RT0; \n \
    uniform sampler2D u_RT1; \n \
    uniform sampler2D u_RT2; \n \
    uniform sampler2DShadow u_shadowTex; \n \
    uniform sampler2D u_diffuseLighting; \n \
    uniform sampler2D u_specularLighting; \n \
                                        \n \
    uniform mat4 u_InvViewMat;  \n \
    uniform mat4 u_InvViewProjectMat;  \n \
    uniform mat4 u_lightMatrix;    \n \
    uniform vec3 u_cameraPosInWorld;    \n \
                                        \n \
    uniform vec3  u_lightDirection;   \n \
    uniform vec3  u_lightColor;    \n \
    uniform float u_lightIntensity;   \n \
    uniform float u_specularMipMapCount; \n \
                                        \n \
    uniform float u_lightShadowEnabled;   \n \
    uniform float u_lightShadowBias;   \n \
    uniform float u_lightShadowStrength;   \n \
    uniform float u_lightShadowSoft;      \n \
    uniform float u_lightShadowSoftness;     \n \
                                                \n \
    in highp vec2 v_texcoord; \n \
    layout(location = 0) out vec4 fragColor; \n \
                                                \n \
    vec2 caculateCoordsWithLatLong(vec3 D) {    \n \
        return vec2((atan(D.x, D.z) / PI + 1.0) * 0.5,    \n \
                (acos(D.y) / PI));    \n \
    }    \n \
                                                \n \
    float luminance(vec3 col) {          \n \
        return 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;          \n \
    }         \n \
                                                        \n \
    void main() {  \n \
        vec4 RT0 = texture(u_RT0, v_texcoord);    \n \
        vec4 RT1 = texture(u_RT1, v_texcoord);    \n \
        vec4 RT2 = texture(u_RT2, v_texcoord);     \n \
        if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }             \n \
                                                                                \n \
        vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;   \n \
        highp float depth = texture(u_depthTex, v_texcoord).r;    \n \
        vec4 worldPosition = u_InvViewProjectMat * vec4(v_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
        worldPosition.xyz /= worldPosition.w;     \n \
        vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);    \n \
        float NdotV = max(0.0, dot(worldNormal, eyeDir));                  \n \
        float F = RT2.w + (1.0 - RT2.w) * pow(1.0 - NdotV, 5.0);     \n \
                                                                                \n \
        vec2 diffCoords = caculateCoordsWithLatLong(worldNormal);          \n \
        vec3 r = normalize(reflect(-eyeDir, worldNormal));          \n \
        vec2 specCoords = caculateCoordsWithLatLong(r);          \n \
        vec3 diffLighing = texture(u_diffuseLighting, diffCoords).rgb;          \n \
        vec3 diffLightIntensity = diffLighing * RT1.w;                           \n \
        vec3 specLightIntensity = textureLod(u_specularLighting, specCoords, (1.0 - F) * u_specularMipMapCount).rgb;   \n \
        float lum = luminance(specLightIntensity);            \n \
        lum = lum / (lum + 1.0);                              \n \
        fragColor = vec4(clamp((linearColor(RT1.xyz) * diffLightIntensity + linearColor(RT1.xyz * RT2.xyz) * pow(lum, RT1.w) * specLightIntensity) * u_lightIntensity, 0.0, 1.0), 1.0);     \n \
    }";

static const char* POINT_LIGHT_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	uniform mat4 u_ModelViewProjectMat;  \n \
	void main() \n \
	{                   \n \
		gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); \n \
	}";

static const char* POINT_LIGHT_F_SHADER = " \
	uniform highp sampler2D u_depthTex; \n \
	uniform highp sampler2D u_RT0; \n \
	uniform sampler2D u_RT1; \n \
	uniform sampler2D u_RT2; \n \
	uniform samplerCubeShadow u_shadowTex;  \n \
                                                 \n \
	uniform highp float u_screenWidth;                \n \
	uniform highp float u_screenHeight;                \n \
	uniform mat4 u_InvViewMat;  \n \
	uniform mat4 u_InvViewProjectMat;                \n \
	uniform mat4 u_lightMatrix;                \n \
	uniform vec3 u_cameraPosInWorld;                \n \
												   \n \
	uniform vec3  u_lightPosition;                \n \
	uniform vec3  u_lightColor;                \n \
	uniform float u_lightIntensity;                \n \
	uniform float u_lightARI;                \n \
										    \n \
	uniform float u_lightShadowEnabled;      \n \
	uniform float u_lightShadowBias;      \n \
	uniform float u_lightShadowStrength;      \n \
	uniform float u_lightShadowSoft;      \n \
	uniform float u_lightShadowSoftness;      \n \
										    \n \
	layout(location = 0) out vec4 fragColor; \n \
										    \n \
	#define SAMPLE_SIZE 20      \n \
	const vec3 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec3[]      \n \
	(                                                    \n \
		vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, 1.0, 1.0),      \n \
		vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 1.0, -1.0),      \n \
		vec3(1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),      \n \
		vec3(1.0, 0.0, 1.0), vec3(-1.0, 0.0, 1.0), vec3(1.0, 0.0, -1.0), vec3(-1.0, 0.0, -1.0),      \n \
		vec3(0.0, 1.0, 1.0), vec3(0.0, -1.0, 1.0), vec3(0.0, -1.0, -1.0), vec3(0.0, 1.0, -1.0)      \n \
	);      \n \
										    \n \
	float shadowing(highp vec3 vertex) {      \n \
		if (u_lightShadowEnabled < 1.0) return 1.0;      \n \
		highp vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);      \n \
		highp float pTolDis2 = dot(projectVertex.xyz, projectVertex.xyz);      \n \
        pTolDis2 = pTolDis2 - u_lightShadowBias * pTolDis2;         \n \
		pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);      \n \
		highp vec4 shadowCoord = vec4(projectVertex.xyz, pTolDis2);      \n \
		float factor = 0.0;      \n \
		if (0.0 < u_lightShadowSoft) {      \n \
			float sum = 0.0;      \n \
			for (int i = 0; i < SAMPLE_SIZE; ++i) {      \n \
				sum += texture(u_shadowTex, vec4(shadowCoord.xyz + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.w));      \n \
			}      \n \
			factor = sum / float(SAMPLE_SIZE);      \n \
		}      \n \
		else {      \n \
			factor = texture(u_shadowTex, shadowCoord);      \n \
		}      \n \
										    \n \
		if (factor < 1.0)      \n \
			factor = mix(1.0, factor, u_lightShadowStrength);      \n \
		return factor;      \n \
	}      \n \
										    \n \
	void main() {  \n \
		highp vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);   \n \
		vec4 RT0 = texture(u_RT0, texCoords);    \n \
		vec4 RT1 = texture(u_RT1, texCoords);    \n \
		vec4 RT2 = texture(u_RT2, texCoords);     \n \
		if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }             \n \
																						\n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;   \n \
		highp float depth = texture(u_depthTex, texCoords).r;    \n \
		highp vec4 worldPosition = u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
		worldPosition.xyz /= worldPosition.w;     \n \
																						\n \
		vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);     \n \
		vec3 lightDir = u_lightPosition - worldPosition.xyz;     \n \
		float attDis = 1.0 + length(lightDir) * u_lightARI;     \n \
        attDis = 1.0 / (attDis * attDis);\n \
		float attenuation = clamp(attDis, 0.0, 1.0);     \n \
		lightDir = normalize(lightDir);     \n \
																						\n \
		float NdotL = max(0.0, dot(worldNormal, lightDir));     \n \
		vec3 H = normalize(eyeDir + lightDir);     \n \
		float NdotH = max(0.0, dot(worldNormal, H));     \n \
		float NdotV = max(0.0, dot(worldNormal, eyeDir));    \n \
		float HdotV = max(0.0, dot(H, eyeDir));    \n \
		float diffFactor = OrenNayar(worldNormal, lightDir, eyeDir, NdotL, NdotV, RT1.w);     \n \
		float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, RT1.w, RT2.w);     \n \
																						\n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity * attenuation * shadowing(worldPosition.xyz);    \n \
		fragColor = vec4(clamp((linearColor(RT1.xyz) * diffFactor + linearColor(RT2.xyz) * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
	}";

static const char* SPOT_LIGHT_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	uniform mat4 u_ModelViewProjectMat;  \n \
	void main() \n \
	{                   \n \
		gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); \n \
	}";

static const char* SPOT_LIGHT_F_SHADER = " \
	uniform highp sampler2D u_depthTex; \n \
	uniform highp sampler2D u_RT0; \n \
	uniform sampler2D u_RT1; \n \
	uniform sampler2D u_RT2; \n \
	uniform sampler2DShadow u_shadowTex; \n \
										    \n \
	uniform float u_screenWidth;  \n \
	uniform float u_screenHeight;  \n \
	uniform mat4 u_InvViewMat;  \n \
	uniform mat4 u_InvViewProjectMat;  \n \
	uniform mat4 u_lightMatrix;  \n \
	uniform vec3 u_cameraPosInWorld;  \n \
										    \n \
	uniform vec3  u_lightPosition;  \n \
	uniform vec3  u_lightDirection;  \n \
	uniform vec3  u_lightColor;  \n \
	uniform float u_lightIntensity;  \n \
	uniform float u_lightARI;  \n \
	uniform float u_lightInnerAngleCos;  \n \
	uniform float u_lightOuterAngleCos;  \n \
										    \n \
	uniform float u_lightShadowEnabled;  \n \
	uniform float u_lightShadowBias;  \n \
	uniform float u_lightShadowStrength;  \n \
	uniform float u_lightShadowSoft;  \n \
	uniform float u_lightShadowSoftness;  \n \
										    \n \
	layout(location = 0) out vec4 fragColor; \n \
												  \n \
	#define SAMPLE_SIZE 9  \n \
	const vec2 SAMPLE_OFFSETS[SAMPLE_SIZE] = vec2[]    \n \
	(                                                       \n \
		vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),    \n \
		vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 1.0),    \n \
		vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)   \n \
	);                                                      \n \
												  \n \
	float shadowing(vec3 vertex) {          \n \
		if (u_lightShadowEnabled < 1.0) return 1.0;          \n \
		vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);          \n \
		vec3 shadowCoord = vec3(projectVertex.xy, projectVertex.z - u_lightShadowBias) / projectVertex.w;          \n \
		float factor = 0.0;          \n \
		if (0.0 < u_lightShadowSoft) {          \n \
			float sum = 0.0;          \n \
			for (int i = 0; i < SAMPLE_SIZE; ++i) {          \n \
				sum += texture(u_shadowTex, vec3(shadowCoord.xy + SAMPLE_OFFSETS[i] * u_lightShadowSoftness, shadowCoord.z));          \n \
			}          \n \
			factor = sum / float(SAMPLE_SIZE);          \n \
		}          \n \
		else {          \n \
			factor = texture(u_shadowTex, shadowCoord);          \n \
		}          \n \
												  \n \
		if (factor < 1.0)          \n \
			factor = mix(1.0, factor, u_lightShadowStrength);          \n \
		return factor;          \n \
	}          \n \
												  \n \
	void main() {  \n \
		highp vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);   \n \
		vec4 RT0 = texture(u_RT0, texCoords);    \n \
		vec4 RT1 = texture(u_RT1, texCoords);    \n \
		vec4 RT2 = texture(u_RT2, texCoords);     \n \
		if (RT0.w <= 0.0){ fragColor = vec4(0.0); return; }             \n \
																						    \n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(RT0.xyz)), 0.0)).xyz;   \n \
		highp float depth = texture(u_depthTex, texCoords).r;    \n \
		vec4 worldPosition = u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
		worldPosition.xyz /= worldPosition.w;     \n \
																						    \n \
		vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);    \n \
		vec3 lightDir = u_lightPosition - worldPosition.xyz;   \n \
		vec3 attDis = lightDir * u_lightARI;   \n \
		float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);   \n \
		lightDir = normalize(lightDir);   \n \
		float currentAngleCos = dot(lightDir, -u_lightDirection);   \n \
		attenuation *= smoothstep(u_lightOuterAngleCos, u_lightInnerAngleCos, currentAngleCos);   \n \
																						    \n \
		float NdotL = max(0.0, dot(worldNormal, lightDir));   \n \
		vec3 H = normalize(eyeDir + lightDir);   \n \
		float NdotH = max(0.0, dot(worldNormal, H));   \n \
		float NdotV = max(0.0, dot(worldNormal, eyeDir));    \n \
		float HdotV = max(0.0, dot(H, eyeDir));    \n \
		float diffFactor = OrenNayar(worldNormal, lightDir, eyeDir, NdotL, NdotV, RT1.w);     \n \
		float specFactor = CookTorrance(NdotL, NdotV, HdotV, NdotH, RT1.w, RT2.w);          \n \
																						    \n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity * attenuation * shadowing(worldPosition.xyz);    \n \
		fragColor = vec4(clamp((linearColor(RT1.xyz) * diffFactor + linearColor(RT2.xyz) * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
	}";

veDeferredRenderPipeline::veDeferredRenderPipeline(veSceneManager *sm)
	: veRenderPipeline(sm)
{
	initLightingParams();
}

veDeferredRenderPipeline::~veDeferredRenderPipeline()
{

}

void veDeferredRenderPipeline::renderScene(veCamera *camera, unsigned int contextID)
{
    unsigned int defaultClearMask = camera->getClearMask();
    unsigned int deferredClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    camera->setClearMask(deferredClearMask);
    if (_sceneManager->getSkyBox())
        _sceneManager->getSkyBox()->render(camera, contextID);
    
    auto &vp = camera->getViewport();
	auto &params = getCameraParams(camera);
	veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
	params.DS->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
	params.RT0->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.RT1->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.RT2->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.FBO->setFrameBufferSize(size);
	params.FBO->bind(contextID, camera->getClearMask());
    prepareForDraws(camera);
	draw(camera, camera->getRenderQueue()->deferredRenderGroup);
	params.FBO->unBind();

	renderLights(camera, contextID);
    bool needPostPorcess = false;
	if (!camera->getPostProcesserList().empty()) {
        for (auto &iter : camera->getPostProcesserList()) {
            auto processer = iter.get();
            if (processer->isEnabled()) {
                needPostPorcess = true;
                break;
            }
        }
        
        if (needPostPorcess) {
            initPostProcesserParams(camera);
            params.sceneColorTexture->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
            params.sceneDepthTexture->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
            params.postProcesserFBO->setFrameBufferSize(size);
            params.postProcesserFBO->bind(contextID, deferredClearMask, GL_DRAW_FRAMEBUFFER);
            params.FBO->blitFramebuffer(GL_DEPTH_BUFFER_BIT, GL_NEAREST, contextID);
            camera->setClearMask(GL_COLOR_BUFFER_BIT);
            prepareForDraws(camera);
            draw(camera, camera->getRenderQueue()->forwardRenderGroup);
            params.postProcesserFBO->unBind();
            
            for (auto &iter : camera->getPostProcesserList()) {
                auto processer = iter.get();
                if (processer->isEnabled()) {
                    processer->process(this, camera, params.sceneColorTexture.get(), params.sceneDepthTexture.get(), contextID);
                }
            }
            
            for (auto &iter : camera->getPostProcesserList()) {
                auto processer = iter.get();
                if (processer->isEnabled()) {
                    processer->getProcesserSurface()->update(_sceneManager->getRootNode(), _sceneManager);
                    processer->getProcesserSurface()->render(_sceneManager->getRootNode(), camera, contextID);
                }
            }
        
        }
	}
    
    if (camera->getFrameBufferObject())
        camera->getFrameBufferObject()->bind(contextID, camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
    
    for (auto &comp : _sceneManager->getComponentList()) {
        (comp.get())->beforeDraw(_sceneManager, camera);
    }
    
    if (needPostPorcess) {
        params.postProcesserFBO->blitFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST, contextID);
        camera->setClearMask(0);
    }else{
        params.FBO->blitFramebuffer(GL_DEPTH_BUFFER_BIT, GL_NEAREST, contextID);
        camera->setClearMask(GL_COLOR_BUFFER_BIT);
    }
	prepareForDraws(camera);
	draw(camera, camera->getRenderQueue()->forwardRenderGroup);
	for (auto &comp : _sceneManager->getComponentList()) {
		(comp.get())->afterDraw(_sceneManager, camera);
	}
	if (camera->getFrameBufferObject())
		camera->getFrameBufferObject()->unBind();
    
    camera->setClearMask(defaultClearMask);
}

void veDeferredRenderPipeline::renderToPostProcesser(vePostProcesser *processer, veCamera *camera, unsigned int contextID)
{
    prepareForDraws(camera);
    draw(camera, camera->getRenderQueue()->forwardRenderGroup);
}

veDeferredRenderPipeline::CameraRenderParams& veDeferredRenderPipeline::getCameraParams(veCamera *camera)
{
	auto &params = _cameraRenderParamsList[camera];
	if (!params.FBO.valid()) {
		params.FBO = _sceneManager->createFrameBufferObject(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FBO_"));
		params.DS  = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_DS_"));
		params.RT0 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT0_"));
		params.RT1 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT1_"));
		params.RT2 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT2_"));

		params.DS->setFilterMode(veTexture::LINEAR);
		params.RT0->setFilterMode(veTexture::NEAREST);
		params.RT1->setFilterMode(veTexture::NEAREST);
		params.RT2->setFilterMode(veTexture::NEAREST);


		params.FBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, params.DS.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.RT0.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, params.RT1.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, params.RT2.get());
        
        
        params.FBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, params.DS.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.RT0.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, params.RT1.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, params.RT2.get());
	}
	return params;
}

void veDeferredRenderPipeline::initPostProcesserParams(veCamera *camera)
{
    auto &params = getCameraParams(camera);
    if (!params.postProcesserFBO.valid()) {
        params.postProcesserFBO = _sceneManager->createFrameBufferObject(camera->getName() + std::string("_VE_PRE_POST_PROCESSER_FBO_"));
        
        params.sceneColorTexture = _sceneManager->createTexture(camera->getName() + std::string("_VE_SCENE_COLOR_TEXTURE_"));
        params.postProcesserFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.sceneColorTexture.get());
        params.sceneColorTexture->setWrapMode(veTexture::CLAMP);
        params.sceneColorTexture->setFilterMode(veTexture::NEAREST);
        
        params.sceneDepthTexture = _sceneManager->createTexture(camera->getName() + std::string("_VE_SCENE_DEPTH_TEXTURE_"));
        params.postProcesserFBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, params.sceneDepthTexture.get());
        params.sceneDepthTexture->setWrapMode(veTexture::CLAMP);
        params.sceneDepthTexture->setFilterMode(veTexture::LINEAR);
    }
}

void veDeferredRenderPipeline::initLightingParams()
{
    _ambientLightRenderer     = new veScreenLightRenderer;
	_directionalLightRenderer = new veScreenLightRenderer;
	_pointLightRenderer       = new veSphereLightRenderer;
	_spotLightRenderer        = new veConeLightRenderer;
    _iBLightRenderer          = new veScreenLightRenderer;
    
    _ambientLightMaterial = createAmbientLightMaterial();
}

veMaterial* veDeferredRenderPipeline::createAmbientLightMaterial()
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass);
    
    pass->setRenderPass(vePass::FORWARD_PASS);
    pass->depthTest() = false;
    pass->depthWrite() = false;
    pass->stencilTest() = false;
    pass->cullFace() = true;
    pass->cullFaceMode() = GL_BACK;
    pass->blendFunc().src = GL_ONE;
    pass->blendFunc().dst = GL_ONE;
    pass->blendEquation() = GL_FUNC_ADD;
    pass->setShader(new veShader(veShader::VERTEX_SHADER, SCREEN_BASED_LIGHT_V_SHADER));
    pass->setShader(new veShader(veShader::FRAGMENT_SHADER, std::string(AMBIENT_LIGHT_F_SHADER).c_str()));
    pass->addUniform(new veUniform("u_ambient", veVec3::ZERO));
    pass->addUniform(new veUniform("u_RT1", 0));
    
    return material;
}

veMaterial* veDeferredRenderPipeline::createDirectionalLightMaterial(veLight *light)
{
	auto material = new veMaterial;
	auto technique = new veTechnique;
	auto pass = new vePass;
	material->addTechnique(technique);
	technique->addPass(pass);

	initLightCommomParams(light, pass);
    pass->setRenderPass(vePass::FORWARD_PASS);
	pass->depthTest() = false;
	pass->depthWrite() = false;
	pass->stencilTest() = false;
	pass->cullFace() = true;
	pass->cullFaceMode() = GL_BACK;
	pass->blendFunc().src = GL_ONE;
	pass->blendFunc().dst = GL_ONE;
	pass->blendEquation() = GL_FUNC_ADD;
	pass->setShader(new veShader(veShader::VERTEX_SHADER, SCREEN_BASED_LIGHT_V_SHADER));
	pass->setShader(new veShader(veShader::FRAGMENT_SHADER, (std::string(COMMON_FUNCTIONS) + std::string(DIRECTIONAL_LIGHT_F_SHADER)).c_str()));
	pass->addUniform(new veUniform("u_lightDirection", veVec3::ZERO));
    pass->addUniform(new veUniform("u_shadowTex", 4));

	return material;
}

veMaterial* veDeferredRenderPipeline::createIBLightMaterial(veLight *light)
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass);
    
    initLightCommomParams(light, pass);
    pass->setRenderPass(vePass::FORWARD_PASS);
    pass->depthTest() = false;
    pass->depthWrite() = false;
    pass->stencilTest() = false;
    pass->cullFace() = true;
    pass->cullFaceMode() = GL_BACK;
    pass->blendFunc().src = GL_ONE;
    pass->blendFunc().dst = GL_ONE;
    pass->blendEquation() = GL_FUNC_ADD;
    pass->setShader(new veShader(veShader::VERTEX_SHADER, SCREEN_BASED_LIGHT_V_SHADER));
    pass->setShader(new veShader(veShader::FRAGMENT_SHADER, (std::string(COMMON_FUNCTIONS) + std::string(IB_LIGHT_F_SHADER)).c_str()));
    pass->addUniform(new veUniform("u_lightDirection", veVec3::ZERO));
    pass->addUniform(new veUniform("u_specularMipMapCount", 1.0f));
    pass->addUniform(new veUniform("u_diffuseLighting", 4));
    pass->addUniform(new veUniform("u_specularLighting", 5));
    
    
    return material;
}

veMaterial* veDeferredRenderPipeline::createPointLightMaterial(veLight *light)
{
	auto material = new veMaterial;
	auto technique = new veTechnique;
	auto pass0 = new vePass;
	auto pass1 = new vePass;
	material->addTechnique(technique);
	technique->addPass(pass0);
	technique->addPass(pass1);

    pass0->setRenderPass(vePass::FORWARD_PASS);
	pass0->depthTest() = true;
	pass0->depthWrite() = false;
	pass0->stencilTest() = true;
	pass0->cullFace() = false;
	pass0->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
	pass0->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
	pass0->blendFunc() = veBlendFunc::DISABLE;
	pass0->setShader(new veShader(veShader::VERTEX_SHADER, POINT_LIGHT_V_SHADER));
	pass0->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
	pass0->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass0->setApplyFunctionCallback([]() {
		glClear(GL_STENCIL_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	});

	initLightCommomParams(light, pass1);
    pass1->setRenderPass(vePass::FORWARD_PASS);
	pass1->depthTest() = false;
	pass1->depthWrite() = false;
	pass1->stencilTest() = true;
	pass1->cullFace() = true;
	pass1->cullFaceMode() = GL_FRONT;
	pass1->blendFunc().src = GL_ONE;
	pass1->blendFunc().dst = GL_ONE;
	pass1->blendEquation() = GL_FUNC_ADD;
	pass1->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
	pass1->setShader(new veShader(veShader::VERTEX_SHADER, POINT_LIGHT_V_SHADER));
	pass1->setShader(new veShader(veShader::FRAGMENT_SHADER, (std::string(COMMON_FUNCTIONS) + std::string(POINT_LIGHT_F_SHADER)).c_str()));
	pass1->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass1->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
	pass1->addUniform(new veUniform("u_lightARI", 0.0f));
    pass1->addUniform(new veUniform("u_shadowTex", 4));
	pass1->setApplyFunctionCallback([]() {
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	});

	return material;
}

veMaterial* veDeferredRenderPipeline::createSpotLightMaterial(veLight *light)
{
	auto material = new veMaterial;
	auto technique = new veTechnique;
	auto pass0 = new vePass;
	auto pass1 = new vePass;
	material->addTechnique(technique);
	technique->addPass(pass0);
	technique->addPass(pass1);

    pass0->setRenderPass(vePass::FORWARD_PASS);
	pass0->depthTest() = true;
	pass0->depthWrite() = false;
	pass0->stencilTest() = true;
	pass0->cullFace() = false;
	pass0->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
	pass0->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
	pass0->blendFunc() = veBlendFunc::DISABLE;
	pass0->setShader(new veShader(veShader::VERTEX_SHADER, SPOT_LIGHT_V_SHADER));
	pass0->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
	pass0->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass0->setApplyFunctionCallback([]() {
		glClear(GL_STENCIL_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	});

	initLightCommomParams(light, pass1);
    pass1->setRenderPass(vePass::FORWARD_PASS);
	pass1->depthTest() = false;
	pass1->depthWrite() = false;
	pass1->stencilTest() = true;
	pass1->cullFace() = true;
	pass1->cullFaceMode() = GL_FRONT;
	pass1->blendFunc().src = GL_ONE;
	pass1->blendFunc().dst = GL_ONE;
	pass1->blendEquation() = GL_FUNC_ADD;
	pass1->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
	pass1->setShader(new veShader(veShader::VERTEX_SHADER, SPOT_LIGHT_V_SHADER));
	pass1->setShader(new veShader(veShader::FRAGMENT_SHADER, (std::string(COMMON_FUNCTIONS) + std::string(SPOT_LIGHT_F_SHADER)).c_str()));
	pass1->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass1->addUniform(new veUniform("u_lightDirection", veVec3(0.0f)));
	pass1->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
	pass1->addUniform(new veUniform("u_lightARI", 0.0f));
	pass1->addUniform(new veUniform("u_lightInnerAngleCos", 0.0f));
	pass1->addUniform(new veUniform("u_lightOuterAngleCos", 0.0f));
    pass1->addUniform(new veUniform("u_shadowTex", 4));
	pass1->setApplyFunctionCallback([]() {
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	});

	return material;
}

void veDeferredRenderPipeline::initLightCommomParams(veLight *light, vePass *pass)
{
	pass->addUniform(new veUniform("u_InvViewMat", INV_V_MATRIX));
	pass->addUniform(new veUniform("u_InvViewProjectMat", INV_VP_MATRIX));
	pass->addUniform(new veUniform("u_screenWidth", SCREEN_WIDTH));
	pass->addUniform(new veUniform("u_screenHeight", SCREEN_HEIGHT));
	pass->addUniform(new veUniform("u_cameraPosInWorld", veVec3::ZERO));
	pass->addUniform(new veUniform("u_lightColor", veVec3::ZERO));
	pass->addUniform(new veUniform("u_lightIntensity", 0.0f));
	pass->addUniform(new veUniform("u_lightMatrix", veMat4::IDENTITY));
	pass->addUniform(new veUniform("u_lightShadowEnabled", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowBias", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowStrength", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowSoft", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowSoftness", 0.0f));

	pass->addUniform(new veUniform("u_depthTex", 0));
	pass->addUniform(new veUniform("u_RT0", 1));
	pass->addUniform(new veUniform("u_RT1", 2));
	pass->addUniform(new veUniform("u_RT2", 3));
}

void veDeferredRenderPipeline::updateLightCommomParams(veLight *light, vePass *pass, veCamera *camera)
{
	veMat4 cameraWorldMat = camera->getNodeToWorldMatrix();
	pass->getUniform("u_cameraPosInWorld")->setValue(veVec3(cameraWorldMat[0][3], cameraWorldMat[1][3], cameraWorldMat[2][3]));
	pass->getUniform("u_lightColor")->setValue(light->getColor());
	pass->getUniform("u_lightIntensity")->setValue(light->getIntensity());
	pass->getUniform("u_lightMatrix")->setValue(light->getLightMatrix());
	pass->getUniform("u_lightShadowEnabled")->setValue(light->isShadowEnabled() ? 1.0f : 0.0f);
	pass->getUniform("u_lightShadowBias")->setValue(light->getShadowBias());
	pass->getUniform("u_lightShadowStrength")->setValue(light->getShadowStrength());
	pass->getUniform("u_lightShadowSoft")->setValue(light->isUseSoftShadow() ? 1.0f : 0.0f);
	pass->getUniform("u_lightShadowSoftness")->setValue(light->getShadowSoftness());

	auto &params = _cameraRenderParamsList[camera];
	pass->setTexture(vePass::DEPTH_STENCIL_BUFFER_TEXTURE, params.DS.get());
	pass->setTexture(vePass::DIFFUSE_TEXTURE, params.RT0.get());
	pass->setTexture(vePass::SPECULAR_TEXTURE, params.RT1.get());
	pass->setTexture(vePass::EMISSIVE_TEXTURE, params.RT2.get());
}

void veDeferredRenderPipeline::renderLights(veCamera *camera, unsigned int contextID)
{
    
    renderAmbientLight(_sceneManager->getAmbientColor(), camera, contextID);
    
	auto &lightListMap = _sceneManager->getLightListMap();
	if (!lightListMap.empty()) {
		{
			auto iter = lightListMap.find(veLight::POINT);
			if (iter != lightListMap.end()) {
				auto &pointLightList = iter->second;
				for (auto &light : pointLightList) {
                    if (light->isEnabled()) {
                        cullPointLight(light.get(), camera, contextID);
                        renderPointLight(light.get(), camera, contextID);
                    }
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::SPOT);
			if (iter != lightListMap.end()) {
				auto &spotLightList = iter->second;
				for (auto &light : spotLightList) {
                    if (light->isEnabled()) {
                        cullSpotLight(light.get(), camera, contextID);
                        renderSpotLight(light.get(), camera, contextID);
                    }
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::DIRECTIONAL);
			if (iter != lightListMap.end()) {
				auto &directionalLightList = iter->second;
				for (auto &light : directionalLightList) {
                    if (light->isEnabled()) {
                        renderDirectionalLight(light.get(), camera, contextID);
                    }
				}
			}
		}
        
        {
            auto iter = lightListMap.find(veLight::IB);
            if (iter != lightListMap.end()) {
                auto &iBLightList = iter->second;
                for (auto &light : iBLightList) {
                    if (light->isEnabled()) {
                        renderIBLight(light.get(), camera, contextID);
                    }
                }
            }
        }
	}
}

void veDeferredRenderPipeline::cullPointLight(veLight *light, veCamera *camera, unsigned int contextID)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createPointLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
    veMat4 lightInWorld = light->getAttachedNodeList()[0]->getNodeToWorldMatrix();
	_pointLightRenderer->render(lightInWorld * veMat4::scale(veVec3(light->getAttenuationRange())), pass, camera, contextID);
}

void veDeferredRenderPipeline::cullSpotLight(veLight *light, veCamera *camera, unsigned int contextID)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createSpotLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
    veMat4 lightInWorld = spotLight->getAttachedNodeList()[0]->getNodeToWorldMatrix();
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->render(lightInWorld * veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())), pass, camera, contextID);
}

void veDeferredRenderPipeline::renderAmbientLight(const veVec3 &ambient, veCamera *camera, unsigned int contextID)
{
    vePass *pass = _ambientLightMaterial->getTechnique(0)->getPass(0);
    pass->getUniform("u_ambient")->setValue(ambient);
    auto &params = _cameraRenderParamsList[camera];
    pass->setTexture(vePass::COLOR_BUFFER_TEXTURE, params.RT1.get());
    _ambientLightRenderer->render(veMat4::IDENTITY, pass, camera, contextID);
}

void veDeferredRenderPipeline::renderDirectionalLight(veLight *light, veCamera *camera, unsigned int contextID)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createDirectionalLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
	updateLightCommomParams(light, pass, camera);
	veMat4 lightInWorld = light->getAttachedNodeList()[0]->getNodeToWorldMatrix();
	lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
	veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);
    pass->setTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
	_directionalLightRenderer->render(lightInWorld, pass, camera, contextID);
}

void veDeferredRenderPipeline::renderIBLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    veIBLight *iBLight = static_cast<veIBLight *>(light);
    if (iBLight->getDiffuseLightingTexture() && iBLight->getSpecularLightingTexture()) {
        auto &material = _lightRenderParamsList[light];
        if (!material.valid()) {
            _lightRenderParamsList[light] = createIBLightMaterial(light);
        }

        vePass *pass = material->getTechnique(0)->getPass(0);
        updateLightCommomParams(light, pass, camera);
        
        veMat4 lightInWorld = iBLight->getAttachedNodeList()[0]->getNodeToWorldMatrix();
        lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
        veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
        direction.normalize();
        pass->getUniform("u_lightDirection")->setValue(direction);
        pass->getUniform("u_specularMipMapCount")->setValue((veReal)(iBLight->getSpecularLightingTexture()->getMipMapLevelCount() - 1));
        pass->setTexture(vePass::LIGHTMAP_TEXTURE, iBLight->getDiffuseLightingTexture());
        pass->setTexture(vePass::REFLECTION_TEXTURE, iBLight->getSpecularLightingTexture());
        _iBLightRenderer->render(lightInWorld, pass, camera, contextID);
    }
}

void veDeferredRenderPipeline::renderPointLight(veLight *light, veCamera *camera, unsigned int contextID)
{
	auto &material = _lightRenderParamsList[light];

	vePass *pass = material->getTechnique(0)->getPass(1);
	updateLightCommomParams(light, pass, camera);

	vePointLight *pointLight = static_cast<vePointLight *>(light);
	veMat4 lightInWorld = pointLight->getAttachedNodeList()[0]->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
	pass->getUniform("u_lightARI")->setValue(pointLight->getAttenuationRangeInverse());
    pass->setTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
	_pointLightRenderer->render(lightInWorld * veMat4::scale(veVec3(pointLight->getAttenuationRange())), pass, camera, contextID);
}

void veDeferredRenderPipeline::renderSpotLight(veLight *light, veCamera *camera, unsigned int contextID)
{
	auto &material = _lightRenderParamsList[light];

	vePass *pass = material->getTechnique(0)->getPass(1);
	updateLightCommomParams(light, pass, camera);

	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
	veMat4 lightInWorld = spotLight->getAttachedNodeList()[0]->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
    veMat4 lightInWorldNoTrans = lightInWorld;
	lightInWorldNoTrans[0][3] = lightInWorldNoTrans[1][3] = lightInWorldNoTrans[2][3] = 0.0f;
	veVec3 direction = lightInWorldNoTrans * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);

	pass->getUniform("u_lightARI")->setValue(spotLight->getAttenuationRangeInverse());
	pass->getUniform("u_lightInnerAngleCos")->setValue(spotLight->getInnerAngleCos());
	pass->getUniform("u_lightOuterAngleCos")->setValue(spotLight->getOuterAngleCos());
    pass->setTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->render(lightInWorld * veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())), pass, camera, contextID);
}