#include "Overlay.h"
#include "OverlayRenderer.h"
#include "Material.h"
#include "Shader.h"
#include "Constants.h"

const char *V_SHADER = " #version 430 \n \
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

const char *F_SHADER = " #version 430 \n \
uniform sampler2D u_texture; \n \
in vec3 v_normal; \n \
in vec2 v_texcoord; \n \
layout(location = 0) out vec4 fragColor; \n \
out vec4 color; \n \
void main() {  \n \
	fragColor = texture(u_texture, v_texcoord);  \n \
}";

veOverlay::veOverlay()
{
	_renderer = new veOverlayRenderer;
	createMaterial();
}

veOverlay::~veOverlay()
{

}

void veOverlay::setImage(veImage *image)
{
	_material->getTechnique(0)->getPass(0)->getTexture(0)->setImage(image);
}

veImage* veOverlay::getImage()
{
	return _material->getTechnique(0)->getPass(0)->getTexture(0)->getImage();
}

void veOverlay::createMaterial()
{
	_material = new veMaterial;
	auto tech = new veTechnique;
	auto pass = new vePass;
	auto texture = new veTexture2D;
	_material->addTechnique(tech);
	tech->addPass(pass);
	pass->addTexture(texture);

	pass->depthTest() = false;
	pass->depthWrite() = true;
	pass->cullFace() = true;

	auto vShader = new veShader(veShader::VERTEX_SHADER, V_SHADER);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, F_SHADER);
	pass->setShader(vShader);
	pass->setShader(fShader);

	vShader->addUniform(new veUniform("u_ModelMat", M_MATRIX));
	fShader->addUniform(new veUniform("u_texture", 0));
}
