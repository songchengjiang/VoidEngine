#include "Text.h"
#include "OverlayRenderer.h"
#include "SurfaceRenderer.h"
#include "Constants.h"
#include "SceneManager.h"
#include "Visualiser.h"
#include "FileCore/File.h"

veText::veText(veFont *font, const std::string &content)
	: _type(HUD)
	, _font(font)
	, _content(content)
	, _color(veVec4::WHITE)
	, _charSpace(0)
	, _width(0)
	, _height(0)
	, _needRefresh(true)
	, _needReInitMaterials(true)
{
	_renderer = new veOverlayRenderer;
}

veText::~veText()
{

}

bool veText::handle(veNode *node, veSceneManager *sm, const veEvent &event)
{
	if (event.getEventType() == veEvent::VE_WIN_RESIZE) {
		_needRefresh = true;
	}

	return false;
}

void veText::update(veNode *node, veSceneManager *sm)
{
	if (!_isVisible) return;
	if (_needReInitMaterials) {
		initMaterial();
		_needReInitMaterials = false;
	}
	if (_needRefresh) {
		rebuildContentBitmap(sm->getVisualiser()->width(), sm->getVisualiser()->height());
		_needRefresh = false;
	}
	if (_renderer.valid())
		_renderer->visit(node, this, sm);
}

void veText::setTextType(TextType type)
{
	if (type == _type) return;
	_type = type;
	if (_type == HUD) {
		_renderer = new veOverlayRenderer;
	}
	else if (_type == PLANE) {
		_renderer = new veSurfaceRenderer;
	}
	_needReInitMaterials = true;
	_needRefresh = true;
}

void veText::setFont(veFont *font)
{
	_font = font;
	_needRefresh = true;
}

void veText::setColor(const veVec4 &color)
{
	_color = color;
	_needRefresh = true;
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

	_materials = new veMaterialArray;
	auto material = new veMaterial;
	auto tech = new veTechnique;
	auto pass = new vePass;
	_texture = _sceneManager->createTexture(_name + std::string("-Texture"));
	//_texture->setFilterMode(veTexture::LINEAR);
	_texture->setWrapMode(veTexture::CLAMP);
	material->addTechnique(tech);
	tech->addPass(pass);
	pass->addTexture(_texture.get());

	pass->depthTest() = false;
	pass->depthWrite() = false;
	pass->cullFace() = true;
	pass->blendFunc().src = GL_ONE;
	pass->blendFunc().dst = GL_ONE_MINUS_SRC_ALPHA;

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

	_materials->addMaterial(material);
}


void veText::rebuildContentBitmap(int divWidth, int divHeight)
{
#define FOUR_BYTES_ALIGN(BYTES)  ((((BYTES + 4))>>2)<<2)  

	unsigned int borderSpace = _font->getFontSize() * 0.5;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int baseline = 0;
	for (size_t i = 0; i < _content.size(); ++i) {
		auto charBitmap = _font->getCharBitmap(_content[i]);
		//baseline = veMath::maximum<unsigned int>(charBitmap->bitmap.rows - charBitmap->bitmap_top, baseline);
		height = veMath::maximum<unsigned int>(height, charBitmap->bitmap.rows);
		if (i != _content.size() - 1)
			width += (charBitmap->advance.x >> 6);
	}
	width += 2 * borderSpace;
	height += 2 * borderSpace;
	width = FOUR_BYTES_ALIGN(width);

	unsigned char *buf = new unsigned char[width * height];
	memset(buf, 0, width * height * sizeof(unsigned char));
	unsigned int widthOffset = borderSpace * 0.5;
	unsigned int heightOffset = borderSpace;
	for (size_t i = 0; i < _content.size(); ++i) {
		auto charBitmap = _font->getCharBitmap(_content[i]);
		unsigned int charBaseLine = charBitmap->bitmap.rows - charBitmap->bitmap_top;
		for (unsigned int h = 0; h < charBitmap->bitmap.rows; ++h) {
			memcpy(&buf[(h + baseline - charBaseLine  + heightOffset) * width + widthOffset + charBitmap->bitmap_left], &charBitmap->bitmap.buffer[(charBitmap->bitmap.rows - h - 1) * charBitmap->bitmap.width], charBitmap->bitmap.width * sizeof(unsigned char));
		}
		widthOffset += (charBitmap->advance.x >> 6);
	}


	//unsigned char *temp = new unsigned char[width * fontSpace];
	//memset(temp, 0, width * fontSpace * sizeof(unsigned char));
	//int charStartOffset = charSpace;
	//for (size_t i = 0; i < _content.size(); ++i) {
	//	auto glyph = _font->getGlyphOfCharCode(_content[i]);
	//	for (unsigned int h = 0; h < glyph->bitmap.rows; ++h) {
	//		memcpy(&temp[(h + horiSpace - (glyph->bitmap.rows - glyph->bitmap_top)) * width + charStartOffset + offset], &glyph->bitmap.buffer[(glyph->bitmap.rows - h - 1) * glyph->bitmap.width], glyph->bitmap.width * sizeof(unsigned char));
	//	}
	//	charStartOffset += charSpace + (glyph->advance.x >> 6);
	//}
	//actualWidth = FOUR_BYTES_ALIGN(charStartOffset);

	//unsigned char *buf = new unsigned char[actualWidth * fontSpace];
	//for (int h = 0; h < fontSpace; ++h) {
	//	memcpy(&buf[h * actualWidth], &temp[h * width], actualWidth * sizeof(unsigned char));
	//}

	//for (unsigned int h = 0; h < height; ++h) {
	//	for (unsigned int w = 0; w < width; ++w) {
	//		std::cout << (unsigned int)buf[h * width + w] << " ";
	//	}
	//	veLog(" ");
	//}

	_texture->storage(width, height, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, buf);
	if (_type == HUD)
		_scaleMatUniform->setValue(veMat4::scale(veVec3((float)width / (float)divWidth, (float)height / (float)divHeight, 0.0f)));
	else if (_type == PLANE)
		_scaleMatUniform->setValue(veMat4::scale(veVec3(width / (float)divWidth, height / (float)divWidth, 0.0f)));

	_width = width;
	_height = height;
	//delete[] temp;
	delete[] buf;
}
