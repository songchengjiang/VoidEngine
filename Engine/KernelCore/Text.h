#ifndef _VE_TEXT_
#define _VE_TEXT_
#include "Prerequisites.h"
#include "Surface.h"
#include "VE_Ptr.h"
#include "Font.h"

class VE_EXPORT veText : public veSurface
{
	friend class veSceneManager;
public:

	~veText();

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) override;
	virtual void update(veNode *node, veSceneManager *sm) override;

	void setFont(veFont *font);
	const veFont* getFont() const { return _font.get(); }

	void setContent(const std::string &content);
	const std::string& getContent() const { return _content; }

	void setCharSpace(float space) { _charSpace = space; }

protected:

	veText(veSceneManager *sm, veFont *font, const std::string &content = "");

	virtual void appendMaterial(veMaterial *material) override;
	virtual veShader* getFragmentShader() override;

	void rebuildContentBitmap(int divWidth, int divHeight);

protected:

	VE_Ptr<veFont>    _font;
	std::string       _content;
	float             _charSpace;

	int               _width;
	int               _height;

	bool              _needRefresh;
};

#endif