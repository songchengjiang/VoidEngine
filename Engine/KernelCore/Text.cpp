#include "Text.h"
#include "OverlayRenderer.h"
#include "Constants.h"
#include "Visualiser.h"
#include "FileCore/File.h"

veText::veText(veFont *font, const std::string &content)
	: USE_VE_PTR_INIT
	, _type(HUD)
	, _font(font)
	, _content(content)
	, _color(veVec4::WHITE)
	, _charSpace(0.125)
	, _needRefresh(true)
{
	_renderer = new veOverlayRenderer;
	_material = new veMaterial;
	initMaterial();
}

veText::~veText()
{

}

void veText::update(veNode *node, veVisualiser *vs)
{
	if (!_isVisible) return;
	if (_needRefresh) {
		rebuildContentBitmap(vs->width(), vs->height());
		_needRefresh = false;
	}
	if (_renderer.valid())
		_renderer->visit(node, this, vs);
}

void veText::setTextType(TextType type)
{
	if (type == _type) return;
	initMaterial();
}

void veText::setFont(veFont *font)
{
	_font = font;
	_needRefresh = true;
}

void veText::setColor(const veVec4 &color)
{
	_color = color;
	_colorUniform->setValue(_color);
}

void veText::setContent(const std::string &content)
{
	_content = content;
	_needRefresh = true;
}

void veText::initMaterial()
{
	static const char *PLANE_V_SHADER = " \
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

	static const char *HUD_V_SHADER = " \
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

	static const char *F_SHADER = " \
	uniform sampler2D u_texture; \n \
	uniform vec4 u_Color; \n \
	in vec3 v_normal; \n \
	in vec2 v_texcoord; \n \
	layout(location = 0) out vec4 fragColor; \n \
	void main() {  \n \
		float glyph = texture(u_texture, v_texcoord).r; \n \
		//if (glyph <= 0.1)  \n \
		//	discard;  \n \
		fragColor = u_Color * glyph; \n \
	}";

	auto tech = new veTechnique;
	auto pass = new vePass;
	_texture = new veTexture2D;
	//_texture->setFilterMode(veTexture::LINEAR);
	_texture->setWrapMode(veTexture::CLAMP);
	_material->addTechnique(tech);
	tech->addPass(pass);
	pass->addTexture(_texture.get());

	pass->depthTest() = false;
	pass->depthWrite() = false;
	pass->cullFace() = true;

	veShader *vShader = nullptr;
	if (_type == HUD) {
		vShader = new veShader(veShader::VERTEX_SHADER, HUD_V_SHADER);
		pass->addUniform(new veUniform("u_ModelMat", M_MATRIX));
	}
	else if (_type == PLANE) {
		vShader = new veShader(veShader::VERTEX_SHADER, PLANE_V_SHADER);
		pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	}	

	auto fShader = new veShader(veShader::FRAGMENT_SHADER, F_SHADER);

	pass->setShader(vShader);
	pass->setShader(fShader);

	pass->addUniform(new veUniform("u_texture", 0));
	_colorUniform = new veUniform("u_Color", _color);
	pass->addUniform(_colorUniform.get());
	_scaleMatUniform = new veUniform("u_ScaleMat", veMat4::IDENTITY);
	pass->addUniform(_scaleMatUniform.get());
}


void veText::rebuildContentBitmap(int divWidth, int divHeight)
{
#define FOUR_BYTES_ALIGN(BYTES)  ((((BYTES + 4))>>2)<<2)  

	int fontSize = _font->getFontSize();
	int charSpace = _charSpace * fontSize;
	int horiSpace = 0.125f * fontSize;
	int width = (fontSize + charSpace) * _content.size();
	int actualWidth = 0;
	unsigned char *temp = new unsigned char[width * fontSize];
	memset(temp, 0, width * fontSize * sizeof(unsigned char));
	int charStartOffset = charSpace;
	for (size_t i = 0; i < _content.size(); ++i) {
		auto glyph = _font->getGlyphOfCharCode(_content[i]);
		for (unsigned int h = 0; h < glyph->bitmap.rows; ++h) {
			memcpy(&temp[(h + horiSpace) * width + charStartOffset], &glyph->bitmap.buffer[(glyph->bitmap.rows - h - 1) * glyph->bitmap.width], glyph->bitmap.width * sizeof(unsigned char));
		}
		charStartOffset += charSpace + (glyph->advance.x >> 6);
	}
	actualWidth = FOUR_BYTES_ALIGN(charStartOffset);

	unsigned char *buf = new unsigned char[actualWidth * fontSize];
	for (int h = 0; h < fontSize; ++h) {
		memcpy(&buf[h * actualWidth], &temp[h * width], actualWidth * sizeof(unsigned char));
	}

	//for (unsigned int h = 0; h < fontSize; ++h) {
	//	for (unsigned int w = 0; w < width; ++w) {
	//		std::cout << (unsigned int)buf[h * width + w] << " ";
	//	}
	//	veLog(" ");
	//}

	auto image = new veImage;
	image->set(actualWidth, fontSize, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, buf);
	_texture->setImage(image);
	if (_type == HUD)
		_scaleMatUniform->setValue(veMat4::scale(veVec3((float)actualWidth / (float)divWidth, (float)fontSize / (float)divHeight, 0.0f)));
	else if (_type == PLANE)
		_scaleMatUniform->setValue(veMat4::scale(veVec3(actualWidth / 2.0f, fontSize / 2.0f, 0.0f)));

	delete[] temp;
	delete[] buf;
}
