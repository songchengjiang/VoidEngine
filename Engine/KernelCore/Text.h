#ifndef _VE_TEXT_
#define _VE_TEXT_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "Image.h"
#include "VE_Ptr.h"
#include "Font.h"

class VE_EXPORT veText : public veRenderableObject
{
public:

	enum TextType
	{
		HUD,
		PLANE,
	};

	veText(veFont *font, const std::string &content = "");
	~veText();

	USE_VE_PTR;

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) override;
	virtual void update(veNode *node, veSceneManager *sm) override;

	void setTextType(TextType type);
	TextType getTextType() const { return _type; }

	void setFont(veFont *font);
	const veFont* getFont() const { return _font.get(); }

	void setColor(const veVec4 &color);
	const veVec4& getColor() const { return _color; }

	void setContent(const std::string &content);
	const std::string& getContent() const { return _content; }

	void setCharSpace(float space) { _charSpace = space; }

protected:

	void initMaterial();
	void rebuildContentBitmap(int divWidth, int divHeight);

protected:

	TextType          _type;
	VE_Ptr<veTexture> _texture;
	VE_Ptr<veFont>    _font;
	std::string       _content;
	veVec4            _color;
	VE_Ptr<veUniform> _colorUniform;
	VE_Ptr<veUniform> _scaleMatUniform;
	float             _charSpace;

	int               _width;
	int               _height;

	bool              _needRefresh;
};

#endif