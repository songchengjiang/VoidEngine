#ifndef _VE_SURFACE_
#define _VE_SURFACE_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "BaseCore/Array.h"
#include "VE_Ptr.h"
#include "Shader.h"

class VE_EXPORT veSurface : public veRenderableObject
{
	friend class veSceneManager;
public:
	enum Type
	{
		SURFACE,
		OVERLAY,
		BILLBOARD,
	};
	~veSurface();

	void setAlphaThreshold(veReal threshold);
	veReal getAlphaThreshold() const;

	void setColor(veVec4 &color);
	veVec4 getColor() const;

	void setType(Type type);
	Type getType() const { return _type; }

protected:

	veSurface();

	void initDefaultMaterial();
	virtual void appendMaterial(veMaterial *material);
	virtual veShader* getVertexShader();
	virtual veShader* getFragmentShader();

protected:

	VE_Ptr<veUniform> _alphaThreshold;
	VE_Ptr<veUniform> _scaleMat;
	VE_Ptr<veUniform> _color;
	VE_Ptr<veTexture> _texture;
	Type              _type;
	bool              _needRefreshMaterial;

	static const char* SURFACE_BILLBOARD_V_SHADER;
	static const char* OVERLAY_V_SHADER;
	static const char* COMMON_F_SHADER;
};

#endif