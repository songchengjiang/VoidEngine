#include "Overlay.h"
#include "OverlayRenderer.h"
#include "Material.h"
#include "Constants.h"
#include "TextureManager.h"

veOverlay::veOverlay()
{
	_renderer = new veOverlayRenderer;
}

veOverlay::veOverlay(veTexture *texture)
{
	_renderer = new veOverlayRenderer;
	_materials = new veMaterialArray;
	initMaterial(texture);
}

veOverlay::~veOverlay()
{

}

void veOverlay::setRenderOrder(int order)
{
	static_cast<veOverlayRenderer *>(_renderer.get())->setRenderOrder(order);
}

int veOverlay::getRenderOrder() const
{
	return static_cast<veOverlayRenderer *>(_renderer.get())->getRenderOrder();
}

void veOverlay::setAlphaThreshold(veReal threshold)
{
	_alphaThreshold->setValue(threshold);
}

veReal veOverlay::getAlphaThreshold() const
{
	veReal val;
	_alphaThreshold->getValue(val);
	return val;
}

void veOverlay::initMaterial(veTexture *texture)
{
	static const char *V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec3 normal; \n \
	layout(location = 2) in vec2 texcoord; \n \
	uniform mat4 u_ModelMat; \n \
	out vec3 v_normal; \n \
	out vec2 v_texcoord; \n \
	void main() \n \
	{                   \n \
		v_normal = normal;  \n \
		v_texcoord = texcoord; \n \
		gl_Position = u_ModelMat * vec4(position, 1.0); \n \
	}";

	static const char *F_SHADER = " \
	uniform float u_alphaThreshold; \n \
	uniform sampler2D u_texture; \n \
	in vec3 v_normal; \n \
	in vec2 v_texcoord; \n \
	layout(location = 0) out vec4 fragColor; \n \
	out vec4 color; \n \
	void main() {  \n \
		vec4 color = texture(u_texture, v_texcoord); \n \
		if (color.a < u_alphaThreshold)  \n \
			discard;  \n \
		fragColor = color; \n \
	}";

	_materials = new veMaterialArray;
	auto material = new veMaterial;
	auto tech = new veTechnique;
	auto pass = new vePass;
	material->addTechnique(tech);
	tech->addPass(pass);
	pass->addTexture(texture);

	pass->depthTest() = false;
	pass->depthWrite() = false;
	pass->cullFace() = true;

	auto vShader = new veShader(veShader::VERTEX_SHADER, V_SHADER);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, F_SHADER);
	pass->setShader(vShader);
	pass->setShader(fShader);

	pass->addUniform(new veUniform("u_ModelMat", M_MATRIX));
	pass->addUniform(new veUniform("u_texture", 0));
	_alphaThreshold = new veUniform("u_alphaThreshold", 1.0f);
	pass->addUniform(_alphaThreshold.get());

	_materials->addMaterial(material);
}
