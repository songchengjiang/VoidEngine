#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"

static const char* COMMON_FUNCTIONS = " \
	vec3 decode(vec3 encoded) {     \n \
        float nz = floor(encoded.z * 255.0) / 16.0;     \n \
        vec2 dec = encoded.xy + vec2(floor(nz) / 16.0, fract(nz)) / 255.0;     \n \
		vec2 fenc = dec * 4.0 - 2.0;    \n \
		float f = dot(fenc, fenc);    \n \
		float g = sqrt(1.0 - f / 4.0);    \n \
		return vec3(fenc * g, 1.0 - f / 2.0);    \n \
	}    \n \
                                                                                                                      \n \
	const float PI = 3.1415926535;                                                                                    \n\
	float OrenNayar(vec3 norm, vec3 ldir, vec3 vdir, float NdotL, float NdotV, float roughness) {                     \n\
			                                                                                                          \n\
		float roughness2 = roughness * roughness;                                                                     \n\
		float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));                                                     \n\
		float B = 0.45 * (roughness2 / (roughness2 + 0.09));                                                          \n\
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

static const char* FULL_SCREEN_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_texcoord = texcoord; \n \
		gl_Position = vec4(position, 1.0); \n \
	}";

static const char* FULL_SCREEN_F_SHADER = " \
	uniform vec3 u_ambient; \n \
	uniform sampler2D u_lightTex; \n \
	uniform sampler2D u_RT1; \n \
	in vec2 v_texcoord; \n \
	layout(location = 0) out vec4 fragColor; \n \
	void main() {  \n \
		fragColor = vec4(texture(u_lightTex, v_texcoord).xyz + texture(u_RT1, v_texcoord).xyz * u_ambient, 1.0); \
        \n \
	}";


static const char* DIRECTIONAL_LIGHT_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_texcoord = texcoord; \n \
		gl_Position = vec4(position, 1.0); \n \
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
	in vec2 v_texcoord; \n \
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
		float depth = texture(u_depthTex, v_texcoord).r;    \n \
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
		fragColor = vec4(clamp((RT1.xyz * diffFactor + RT2.xyz * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
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
	uniform float u_screenWidth;                \n \
	uniform float u_screenHeight;                \n \
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
	float shadowing(vec3 vertex) {      \n \
		if (u_lightShadowEnabled < 1.0) return 1.0;      \n \
		vec4 projectVertex = u_lightMatrix * vec4(vertex, 1.0);      \n \
		float pTolDis2 = dot(projectVertex.xyz, projectVertex.xyz);      \n \
		pTolDis2 = pTolDis2 / (pTolDis2 + 1.0);      \n \
		vec4 shadowCoord = vec4(projectVertex.xyz, pTolDis2 - u_lightShadowBias);      \n \
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
		vec4 worldPosition = u_InvViewProjectMat * vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
		worldPosition.xyz /= worldPosition.w;     \n \
																						\n \
		vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);     \n \
		vec3 lightDir = u_lightPosition - worldPosition.xyz;     \n \
		vec3 attDis = lightDir * u_lightARI;     \n \
		float attenuation = clamp(1.0 - dot(attDis, attDis), 0.0, 1.0);     \n \
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
		fragColor = vec4(clamp((RT1.xyz * diffFactor + RT2.xyz * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
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
		fragColor = vec4(clamp((RT1.xyz * diffFactor + RT2.xyz * specFactor) * lightIntensity, 0.0, 1.0), 1.0);     \n \
	}";

veDeferredRenderPipeline::veDeferredRenderPipeline(veSceneManager *sm)
	: veRenderPipeline(sm)
{
	initLightingParams();
	_ambientColor = new veUniform("u_ambient", veVec3::ZERO);
}

veDeferredRenderPipeline::~veDeferredRenderPipeline()
{

}

void veDeferredRenderPipeline::renderScene(veCamera *camera, bool isMainCamera)
{
	auto &params = getCameraParams(camera);
	unsigned int deferredClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	auto &vp = camera->getViewport();
	veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
	params.DS->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
	params.RT0->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.RT1->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.RT2->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.FBO->setFrameBufferSize(size * VE_DEVICE_PIXEL_RATIO);
	params.FBO->bind(deferredClearMask);
    prepareForDraws(camera);
	draw(camera, camera->getRenderQueue()->deferredRenderGroup);
	params.FBO->unBind();

	renderLights(camera);
	if (_sceneManager->getSkyBox())
		_sceneManager->getSkyBox()->render(camera);
	_ambientColor->setValue(_sceneManager->getAmbientColor());
	params.fullScreenTexture->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	params.fullScreenFBO->setFrameBufferSize(size * VE_DEVICE_PIXEL_RATIO);
	params.fullScreenFBO->bind(deferredClearMask, GL_DRAW_FRAMEBUFFER);
	params.FBO->bind(deferredClearMask, GL_READ_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO,
		0, 0, size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	params.fullScreenFBO->bind(deferredClearMask);
	unsigned int defaultClearMask = camera->getClearMask();
	camera->setClearMask(GL_COLOR_BUFFER_BIT);
    prepareForDraws(camera);
	draw(camera, camera->getRenderQueue()->forwardRenderGroup);
	camera->setClearMask(defaultClearMask);
	params.fullScreenFBO->unBind();


	params.fullScreenSurface->update(_sceneManager->getRootNode(), _sceneManager);
	params.fullScreenSurface->render(_sceneManager->getRootNode(), camera);
	if (camera->getFrameBufferObject())
		camera->getFrameBufferObject()->bind(camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
	if (isMainCamera && !_sceneManager->getPostProcesserList().empty()) {
		if (!_postProcesserFBO.valid())
			_postProcesserFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_POST_PROCESSER_FBO_");
		_postProcesserFBO->setFrameBufferSize(size * VE_DEVICE_PIXEL_RATIO);
		for (auto &iter : _sceneManager->getPostProcesserList()) {
			auto processer = iter.get();
			processer->process(this, _postProcesserFBO.get(), camera);
		}
	}
	draw(camera, camera->getRenderQueue()->forwardRenderGroup);
	if (camera->getFrameBufferObject())
		camera->getFrameBufferObject()->unBind();
}

veDeferredRenderPipeline::CameraRenderParams& veDeferredRenderPipeline::getCameraParams(veCamera *camera)
{
	auto &params = _cameraRenderParamsList[camera];
	if (!params.FBO.valid()) {
		params.FBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FBO_"));
		params.DS = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_DS_"));
		params.RT0 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT0_"));
		params.RT1 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT1_"));
		params.RT2 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT2_"));

		params.DS->setFilterMode(veTexture::NEAREST);
		params.RT0->setFilterMode(veTexture::NEAREST);
		params.RT1->setFilterMode(veTexture::NEAREST);
		params.RT2->setFilterMode(veTexture::NEAREST);


		params.FBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, params.DS.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.RT0.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, params.RT1.get());
		params.FBO->attach(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, params.RT2.get());

		params.fullScreenFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_FBO_"));
		params.fullScreenTexture = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_TEXTURE_"));
		params.fullScreenSurface = _sceneManager->createSurface(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_SURFACE_"));
		params.fullScreenFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.fullScreenTexture.get());

		params.fullScreenTexture->setFilterMode(veTexture::NEAREST);

		auto fullScreenMats = _sceneManager->createMaterialArray(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_MATERIAL_ARRAY_"));
		auto fullScreenMat = new veMaterial;
		auto fullScreenTech = new veTechnique;
		auto pass = new vePass;
		fullScreenMats->addMaterial(fullScreenMat);
		fullScreenMat->addTechnique(fullScreenTech);
		fullScreenTech->addPass(pass);
		params.fullScreenSurface->setMaterialArray(fullScreenMats);

        pass->setRenderPass(vePass::FORWARD_PASS);
		pass->depthTest() = false;
		pass->depthWrite() = false;
		pass->cullFace() = true;
		pass->cullFaceMode() = GL_BACK;
		pass->blendFunc() = veBlendFunc::DISABLE;
		pass->setShader(new veShader(veShader::VERTEX_SHADER, FULL_SCREEN_V_SHADER));
		pass->setShader(new veShader(veShader::FRAGMENT_SHADER, FULL_SCREEN_F_SHADER));
		pass->addUniform(new veUniform("u_lightTex", 0));
		pass->addUniform(new veUniform("u_RT1", 1));
		pass->setTexture(vePass::AMBIENT_TEXTURE, params.fullScreenTexture.get());
		pass->setTexture(vePass::SPECULAR_TEXTURE, params.RT1.get());
		pass->addUniform(_ambientColor.get());
	}
	return params;
}

void veDeferredRenderPipeline::initLightingParams()
{
	_directionalLightRenderer = new veScreenLightRenderer;
	_pointLightRenderer       = new veSphereLightRenderer;
	_spotLightRenderer        = new veConeLightRenderer;
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
	pass->setShader(new veShader(veShader::VERTEX_SHADER, DIRECTIONAL_LIGHT_V_SHADER));
	pass->setShader(new veShader(veShader::FRAGMENT_SHADER, (std::string(COMMON_FUNCTIONS) + std::string(DIRECTIONAL_LIGHT_F_SHADER)).c_str()));
	pass->addUniform(new veUniform("u_lightDirection", veVec3::ZERO));

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
	pass->addUniform(new veUniform("u_shadowTex", 4));
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
	pass->setTexture(vePass::AMBIENT_TEXTURE, params.DS.get());
	pass->setTexture(vePass::DIFFUSE_TEXTURE, params.RT0.get());
	pass->setTexture(vePass::SPECULAR_TEXTURE, params.RT1.get());
	pass->setTexture(vePass::EMISSIVE_TEXTURE, params.RT2.get());
	pass->setTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
}

void veDeferredRenderPipeline::renderLights(veCamera *camera)
{
	auto &lightListMap = _sceneManager->getLightListMap();
	if (!lightListMap.empty()) {
		{
			auto iter = lightListMap.find(veLight::POINT);
			if (iter != lightListMap.end()) {
				auto &pointLightList = iter->second;
				for (auto &light : pointLightList) {
					cullPointLight(light.get(), camera);
					renderPointLight(light.get(), camera);
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::SPOT);
			if (iter != lightListMap.end()) {
				auto &spotLightList = iter->second;
				for (auto &light : spotLightList) {
					cullSpotLight(light.get(), camera);
					renderSpotLight(light.get(), camera);
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::DIRECTIONAL);
			if (iter != lightListMap.end()) {
				auto &directionalLightList = iter->second;
				for (auto &light : directionalLightList) {
					renderDirectionalLight(light.get(), camera);
				}
			}
		}
	}
}

void veDeferredRenderPipeline::cullPointLight(veLight *light, veCamera *camera)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createPointLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
	_pointLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(light->getAttenuationRange())));
	_pointLightRenderer->render(light, pass, camera);
}

void veDeferredRenderPipeline::cullSpotLight(veLight *light, veCamera *camera)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createSpotLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())));
	_spotLightRenderer->render(spotLight, pass, camera);
}

void veDeferredRenderPipeline::renderDirectionalLight(veLight *light, veCamera *camera)
{
	auto &material = _lightRenderParamsList[light];
	if (!material.valid()) {
		_lightRenderParamsList[light] = createDirectionalLightMaterial(light);
	}
	vePass *pass = material->getTechnique(0)->getPass(0);
	updateLightCommomParams(light, pass, camera);
	veMat4 lightInWorld = light->getNodeToWorldMatrix();
	lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
	veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);
	_directionalLightRenderer->render(light, pass, camera);
}

void veDeferredRenderPipeline::renderPointLight(veLight *light, veCamera *camera)
{
	auto &material = _lightRenderParamsList[light];

	vePass *pass = material->getTechnique(0)->getPass(1);
	updateLightCommomParams(light, pass, camera);

	vePointLight *pointLight = static_cast<vePointLight *>(light);
	veMat4 lightInWorld = pointLight->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
	pass->getUniform("u_lightARI")->setValue(pointLight->getAttenuationRangeInverse());
	_pointLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(pointLight->getAttenuationRange())));
	_pointLightRenderer->render(pointLight, pass, camera);
}

void veDeferredRenderPipeline::renderSpotLight(veLight *light, veCamera *camera)
{
	auto &material = _lightRenderParamsList[light];

	vePass *pass = material->getTechnique(0)->getPass(1);
	updateLightCommomParams(light, pass, camera);

	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
	veMat4 lightInWorld = spotLight->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
	lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
	veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);

	pass->getUniform("u_lightARI")->setValue(spotLight->getAttenuationRangeInverse());
	pass->getUniform("u_lightInnerAngleCos")->setValue(spotLight->getInnerAngleCos());
	pass->getUniform("u_lightOuterAngleCos")->setValue(spotLight->getOuterAngleCos());
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())));
	_spotLightRenderer->render(spotLight, pass, camera);
}
