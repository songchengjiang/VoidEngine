#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"

static const char* COMMON_FUNCTIONS = " \
	vec3 decode(vec2 encoded) {     \n \
		vec2 fenc = encoded * 4.0 - 2.0;    \n \
		float f = dot(fenc, fenc);    \n \
		float g = sqrt(1.0 - f / 4.0);    \n \
		vec3 normal;    \n \
		normal.xy = fenc * g;    \n \
		normal.z = 1.0 - f / 2.0;    \n \
		return normal;    \n \
	}    \n \
	";

static const char* AMBIENT_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_texcoord = texcoord; \n \
		gl_Position = vec4(position, 1.0); \n \
	}";

static const char* AMBIENT_F_SHADER = " \
	uniform vec3 u_ambient; \n \
	uniform sampler2D u_depthTex; \n \
	uniform sampler2D u_normalAndOpacityTex; \n \
	uniform sampler2D u_diffuseAndLightMaskTex; \n \
	uniform sampler2D u_specularAndRoughnessTex; \n \
	in vec2 v_texcoord; \n \
	layout(location = 0) out vec4 fragColor; \n \
	void main() {  \n \
		fragColor = vec4(texture(u_diffuseAndLightMaskTex, v_texcoord).xyz * u_ambient, 1.0); \n \
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
	uniform sampler2D u_depthTex; \n \
	uniform sampler2D u_normalAndOpacityTex; \n \
	uniform sampler2D u_diffuseAndLightMaskTex; \n \
	uniform sampler2D u_specularAndRoughnessTex; \n \
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
	void main() {  \n \
		vec4 normalAndOpacityTex = texture(u_normalAndOpacityTex, v_texcoord);    \n \
		vec4 diffuseAndLightMaskTex = texture(u_diffuseAndLightMaskTex, v_texcoord);    \n \
		vec4 specularAndRoughnessTex = texture(u_specularAndRoughnessTex, v_texcoord);     \n \
																							  \n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(normalAndOpacityTex.xy)), 0.0)).xyz;   \n \
		float depth = texture(u_depthTex, v_texcoord).r;    \n \
		vec4 worldPosition = u_InvViewProjectMat * vec4(v_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);    \n \
		worldPosition.xyz /= worldPosition.w;     \n \
		vec3 eyeDir = normalize(u_cameraPosInWorld - worldPosition.xyz);    \n \
		float NdotL = max(0.0, dot(worldNormal, -u_lightDirection));    \n \
		vec3 H = normalize(eyeDir - u_lightDirection);    \n \
		float NdotH = max(0.0, dot(worldNormal, H));    \n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity;    \n \
																							  \n \
		fragColor = vec4(clamp((diffuseAndLightMaskTex.xyz * NdotL + specularAndRoughnessTex.xyz * pow(NdotH, 16.0)) * lightIntensity, 0.0, 1.0), 1.0);     \n \
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
	uniform sampler2D u_depthTex; \n \
	uniform sampler2D u_normalAndOpacityTex; \n \
	uniform sampler2D u_diffuseAndLightMaskTex; \n \
	uniform sampler2D u_specularAndRoughnessTex; \n \
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
	void main() {  \n \
		vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);   \n \
		vec4 normalAndOpacityTex = texture(u_normalAndOpacityTex, texCoords);    \n \
		vec4 diffuseAndLightMaskTex = texture(u_diffuseAndLightMaskTex, texCoords);    \n \
		vec4 specularAndRoughnessTex = texture(u_specularAndRoughnessTex, texCoords);     \n \
																						\n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(normalAndOpacityTex.xy)), 0.0)).xyz;   \n \
		float depth = texture(u_depthTex, texCoords).r;    \n \
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
																						\n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity * attenuation;    \n \
		fragColor = vec4(clamp((diffuseAndLightMaskTex.xyz * NdotL + specularAndRoughnessTex.xyz * pow(NdotH, 16.0)) * lightIntensity, 0.0, 1.0), 1.0);     \n \
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
	uniform sampler2D u_depthTex; \n \
	uniform sampler2D u_normalAndOpacityTex; \n \
	uniform sampler2D u_diffuseAndLightMaskTex; \n \
	uniform sampler2D u_specularAndRoughnessTex; \n \
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
	void main() {  \n \
		vec2 texCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);   \n \
		vec4 normalAndOpacityTex = texture(u_normalAndOpacityTex, texCoords);    \n \
		vec4 diffuseAndLightMaskTex = texture(u_diffuseAndLightMaskTex, texCoords);    \n \
		vec4 specularAndRoughnessTex = texture(u_specularAndRoughnessTex, texCoords);     \n \
																						    \n \
		vec3 worldNormal = (u_InvViewMat * vec4(normalize(decode(normalAndOpacityTex.xy)), 0.0)).xyz;   \n \
		float depth = texture(u_depthTex, texCoords).r;    \n \
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
																						    \n \
		vec3 lightIntensity = vec3(u_lightColor.r, u_lightColor.g, u_lightColor.b) * u_lightIntensity * attenuation;    \n \
		fragColor = vec4(clamp((diffuseAndLightMaskTex.xyz * NdotL + specularAndRoughnessTex.xyz * pow(NdotH, 16.0)) * lightIntensity, 0.0, 1.0), 1.0);     \n \
	}";

veDeferredRenderPipeline::veDeferredRenderPipeline(veSceneManager *sm)
	: veRenderPipeline(sm)
{
	initDeferredParams();
	initLightingParams();
}

veDeferredRenderPipeline::~veDeferredRenderPipeline()
{

}

void veDeferredRenderPipeline::renderCamera(veCamera *camera, bool isMainCamera)
{
	unsigned int deferredClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	auto &vp = camera->getViewport();
	veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
	_DS->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
	_RT0->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	_RT1->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	_RT2->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	_FBO->setFrameBufferSize(size);
	_FBO->bind(deferredClearMask);
	draw(camera);
	_FBO->unBind();

	renderLights(camera);
	if (_sceneManager->getSkyBox())
		_sceneManager->getSkyBox()->render(camera);
	_fullScreenSurface->getTexture()->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	_fullScreenFBO->setFrameBufferSize(size);
	_fullScreenFBO->bind(deferredClearMask, GL_DRAW_FRAMEBUFFER);
	_FBO->bind(deferredClearMask, GL_READ_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, size.x(), size.y(),
		0, 0, size.x(), size.y(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	_fullScreenFBO->bind(deferredClearMask);
	unsigned int defaultClearMask = camera->getClearMask();
	camera->setClearMask(GL_COLOR_BUFFER_BIT);
	draw(camera);
	camera->setClearMask(defaultClearMask);
	_fullScreenFBO->unBind();

	_fullScreenSurface->update(_sceneManager->getRootNode(), _sceneManager);
	_fullScreenSurface->render(_sceneManager->getRootNode(), camera);
	if (camera->getFrameBufferObject())
		camera->getFrameBufferObject()->bind(camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
	if (isMainCamera && !_sceneManager->getPostProcesserList().empty()) {
		if (!_postProcesserFBO.valid())
			_postProcesserFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_POST_PROCESSER_FBO_");
		_postProcesserFBO->setFrameBufferSize(size);
		for (auto &iter : _sceneManager->getPostProcesserList()) {
			auto processer = iter.get();
			processer->process(this, _postProcesserFBO.get(), camera);
		}
	}
	draw(camera);
	if (camera->getFrameBufferObject())
		camera->getFrameBufferObject()->unBind();
}

void veDeferredRenderPipeline::initDeferredParams()
{
	_FBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_FBO_");
	_DS  = _sceneManager->createTexture("_VE_DEFERRED_RENDER_PIPELINE_DS_");
	_RT0 = _sceneManager->createTexture("_VE_DEFERRED_RENDER_PIPELINE_RT0_");
	_RT1 = _sceneManager->createTexture("_VE_DEFERRED_RENDER_PIPELINE_RT1_");
	_RT2 = _sceneManager->createTexture("_VE_DEFERRED_RENDER_PIPELINE_RT2_");

	_FBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _DS.get());
	_FBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _RT0.get());
	_FBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _RT1.get());
	_FBO->attach(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _RT2.get());

	_fullScreenFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_FBO_");
	auto fullScreenTex = _sceneManager->createTexture("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_TEXTURE_");
	_fullScreenSurface = _sceneManager->createImage("_VE_DEFERRED_RENDER_PIPELINE_FULL_SCREEN_SURFACE_", fullScreenTex);
	_fullScreenFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fullScreenTex);
}

void veDeferredRenderPipeline::initLightingParams()
{
	_ambientLightRenderPass = new vePass;
	_ambientLightRenderPass->depthTest() = false;
	_ambientLightRenderPass->depthWrite() = false;
	_ambientLightRenderPass->cullFace() = true;
	_ambientLightRenderPass->cullFaceMode() = GL_BACK;
	_ambientLightRenderPass->blendFunc().src = GL_ONE;
	_ambientLightRenderPass->blendFunc().dst = GL_ONE;
	_ambientLightRenderPass->blendEquation() = GL_FUNC_ADD;
	_ambientLightRenderPass->setShader(new veShader(veShader::VERTEX_SHADER, AMBIENT_V_SHADER));
	_ambientLightRenderPass->setShader(new veShader(veShader::FRAGMENT_SHADER, AMBIENT_F_SHADER));
	_ambientLightRenderPass->addUniform(new veUniform("u_ambient", veVec3::ZERO));
	_ambientLightRenderPass->addUniform(new veUniform("u_depthTex", 0));
	_ambientLightRenderPass->addUniform(new veUniform("u_normalAndOpacityTex", 1));
	_ambientLightRenderPass->addUniform(new veUniform("u_diffuseAndLightMaskTex", 2));
	_ambientLightRenderPass->addUniform(new veUniform("u_specularAndRoughnessTex", 3));
	_ambientLightRenderPass->addTexture(vePass::AMBIENT_TEXTURE, _DS.get());
	_ambientLightRenderPass->addTexture(vePass::DIFFUSE_TEXTURE, _RT0.get());
	_ambientLightRenderPass->addTexture(vePass::SPECULAR_TEXTURE, _RT1.get());
	_ambientLightRenderPass->addTexture(vePass::EMISSIVE_TEXTURE, _RT2.get());

	_ambientLightRender       = new veScreenLightRenderer;
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
	pass->addUniform(new veUniform("u_normalAndOpacityTex", 1));
	pass->addUniform(new veUniform("u_diffuseAndLightMaskTex", 2));
	pass->addUniform(new veUniform("u_specularAndRoughnessTex", 3));
	pass->addUniform(new veUniform("u_shadowTex", 4));
	pass->addTexture(vePass::AMBIENT_TEXTURE, _DS.get());
	pass->addTexture(vePass::DIFFUSE_TEXTURE, _RT0.get());
	pass->addTexture(vePass::SPECULAR_TEXTURE, _RT1.get());
	pass->addTexture(vePass::EMISSIVE_TEXTURE, _RT2.get());
	pass->addTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
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

	renderAmbientLight(camera);
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

void veDeferredRenderPipeline::renderAmbientLight(veCamera *camera)
{
	_ambientLightRenderPass->getUniform("u_ambient")->setValue(_sceneManager->getAmbientColor());
	_ambientLightRender->render(nullptr, _ambientLightRenderPass.get(), camera);
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
