#include "Surface.h"
#include "Material.h"
#include "OverlayRenderer.h"
#include "BillboardRenderer.h"
#include "SceneManager.h"
#include "Constants.h"

const char* veSurface::SURFACE_BILLBOARD_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	uniform mat4 u_ModelViewProjectMat; \n \
	uniform mat4 u_ScaleMat; \n \
	out vec3 v_normal; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_normal = normal;  \n \
		v_texcoord = texcoord; \n \
		gl_Position = u_ModelViewProjectMat * u_ScaleMat * vec4(position, 1.0); \n \
	}";

const char* veSurface::OVERLAY_V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	uniform mat4 u_ModelMat; \n \
	uniform mat4 u_ScaleMat; \n \
	out vec3 v_normal; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_normal = normal;  \n \
		v_texcoord = texcoord; \n \
		gl_Position = u_ModelMat * u_ScaleMat * vec4(position, 1.0); \n \
	}";

const char* veSurface::COMMON_F_SHADER = " \
	uniform sampler2D u_texture; \n \
	uniform vec4 u_Color; \n \
	uniform float u_alphaThreshold; \n \
	in vec3 v_normal; \n \
	in vec2 v_texcoord; \n \
	layout(location=0) out vec4 fragColor;\n \
	void main() {  \n \
		vec4 texColor = texture(u_texture, v_texcoord); \n \
		if (texColor.a < u_alphaThreshold) \n \
			discard;                      \n \
		fragColor = u_Color * texColor; \n \
	}";

veSurface::veSurface(veSceneManager *sm)
	: veRenderableObject(sm)
	, _needRefreshMaterial(true)
	, _type(SURFACE)
{
	_alphaThreshold = new veUniform("u_alphaThreshold", 1.0f);
	_color = new veUniform("u_Color", veVec4::WHITE);
	_scaleMat = new veUniform("u_ScaleMat", veMat4::IDENTITY);
	setType(OVERLAY);
}

veSurface::~veSurface()
{

}

void veSurface::setAlphaThreshold(veReal threshold)
{
	_alphaThreshold->setValue(threshold);
}

veReal veSurface::getAlphaThreshold() const
{
	veReal val;
	_alphaThreshold->getValue(val);
	return val;
}

void veSurface::setColor(const veVec4 &color)
{
	_color->setValue(color);
}

veVec4 veSurface::getColor() const
{
	veVec4 val;
	_color->getValue(val);
	return val;
}

void veSurface::setType(Type type)
{
	if (type == _type) return;
	_type = type;
	if (_type == SURFACE) {
		_renderer = new veSurfaceRenderer;
	}
	else if (_type == OVERLAY) {
		_renderer = new veOverlayRenderer;
	}
	else if (_type == BILLBOARD) {
		_renderer = new veBillboardRenderer;
	}
	_boundingBox.dirty();
	_needRefreshMaterial = true;
}

void veSurface::initDefaultMaterial()
{
	_material = new veMaterial;
	auto tech = new veTechnique;
	auto pass = new vePass;
	_material->addTechnique(tech);
	tech->addPass(pass);

    pass->setRenderPass(vePass::FORWARD_PASS);
	if (_type == SURFACE) {
		pass->depthTest() = true;
		pass->depthWrite() = true;
		pass->cullFace() = false;
	}
	else if (_type == OVERLAY) {
		pass->depthTest() = false;
		pass->depthWrite() = false;
		pass->cullFace() = true;
	}
	else if (_type == BILLBOARD) {
		pass->depthTest() = true;
		pass->depthWrite() = true;
		pass->cullFace() = true;
	}

	pass->castShadow() = false;

	veShader *vShader = getVertexShader();
	veShader *fShader = getFragmentShader();
	pass->setShader(vShader);
	pass->setShader(fShader);

	pass->addUniform(new veUniform("u_texture", 0));
	pass->addUniform(_alphaThreshold.get());
	pass->addUniform(_color.get());
	pass->addUniform(_scaleMat.get());

	appendMaterial(_material.get());
}

void veSurface::appendMaterial(veMaterial *material)
{
	auto pass = material->getTechnique(0)->getPass(0);
	if (_texture.valid())
		pass->setTexture(vePass::DIFFUSE_TEXTURE, _texture.get());
	if (_type == SURFACE || _type == BILLBOARD) {
		pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	}
	else if (_type == OVERLAY) {
		pass->addUniform(new veUniform("u_ModelMat", M_MATRIX));
	}
}

veShader* veSurface::getVertexShader()
{
	if (_type == SURFACE || _type == BILLBOARD) {
		return new veShader(veShader::VERTEX_SHADER, SURFACE_BILLBOARD_V_SHADER);
	}
	else if (_type == OVERLAY) {
		return new veShader(veShader::VERTEX_SHADER, OVERLAY_V_SHADER);
	}

	return nullptr;
}

veShader* veSurface::getFragmentShader()
{
	return new veShader(veShader::FRAGMENT_SHADER, COMMON_F_SHADER);
}
