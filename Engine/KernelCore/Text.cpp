#include "Text.h"
#include "OverlayRenderer.h"
#include "SurfaceRenderer.h"
#include "Constants.h"
#include "SceneManager.h"
#include "Application.h"
#include "Configuration.h"
#include "FileCore/File.h"

veText::veText(veSceneManager *sm, veFont *font, const std::string &content)
	: veSurface(sm)
	, _font(font)
	, _content(content)
	, _charSpace(0)
	, _width(0)
	, _height(0)
	, _needRefresh(true)
{
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

	if (_needRefreshMaterial) {
		initDefaultMaterial();
		_needRefreshMaterial = false;
	}
	if (_needRefresh) {
		rebuildContentBitmap(veApplication::instance()->width(), veApplication::instance()->height());
		_needRefresh = false;
	}
	veRenderableObject::update(node, sm);
}

void veText::setFont(veFont *font)
{
	_font = font;
	_needRefresh = true;
}

void veText::setContent(const std::string &content)
{
	_content = content;
	_needRefresh = true;
}

void veText::appendMaterial(veMaterial *material)
{
	auto pass = material->getTechnique(0)->getPass(0);
	pass->blendFunc().src = GL_ONE;
	pass->blendFunc().dst = GL_ONE_MINUS_SRC_ALPHA;

	if (!_texture.valid())
		_texture = _sceneManager->createTexture(_name + std::string("-TextTexture"));

	veSurface::appendMaterial(material);
}

veShader* veText::getFragmentShader()
{
	static const char *F_SHADER = " \
	uniform sampler2D u_texture; \n \
	uniform vec4 u_Color; \n \
	in vec3 v_normal; \n \
	in vec2 v_texcoord; \n \
	layout(location=0) out vec4 fragColor;\n \
	void main() {  \n \
		float glyph = texture(u_texture, v_texcoord).r; \n \
		if (glyph <= 0.1)  \n \
			discard;  \n \
		fragColor = u_Color * glyph; \n \
	}";

	return new veShader(veShader::FRAGMENT_SHADER, F_SHADER);
}

void veText::rebuildContentBitmap(int divWidth, int divHeight)
{
	if (_content.empty()) return;
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
            unsigned char *dst = &buf[(h + baseline - charBaseLine  + heightOffset) * width + widthOffset + charBitmap->bitmap_left];
            unsigned char *src = &charBitmap->bitmap.buffer[(charBitmap->bitmap.rows - h - 1) * charBitmap->bitmap.width];
            size_t sz = charBitmap->bitmap.width * sizeof(unsigned char);
			memcpy(dst, src, sz);
		}
		widthOffset += (charBitmap->advance.x >> 6);
	}

	_texture->storage(width, height, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, buf);
	if (_type == OVERLAY)
		_scaleMat->setValue(veMat4::scale(veVec3((float)width / (float)(divWidth), (float)height / (float)(divHeight), 0.0f)));
	else if (_type == SURFACE || _type == veSurface::BILLBOARD) {
		_scaleMat->setValue(veMat4::scale(veVec3(width * 0.5f, height * 0.5f, 0.0f)));
		_boundingBox.min() = veVec3(-(width * 0.5f), -(height * 0.5f), -0.5f);
		_boundingBox.max() = veVec3(width * 0.5f, height * 0.5f, 0.5f);
	}

	_width = width;
	_height = height;
	//delete[] temp;
	delete[] buf;
}
