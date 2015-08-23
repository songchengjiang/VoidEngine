#ifndef _VE_MESH_
#define _VE_MESH_
#include <unordered_map>
#include "Prerequisites.h"
#include "BaseCore/Array.h"
#include "RenderableObject.h"
#include "VE_Ptr.h"

class VE_EXPORT veMesh : public veRenderableObject
{
public:
	static const unsigned int MAX_ATTRIBUTE_NUM;

	struct VertexAtrribute
	{
		enum AtrributeType
		{
			VERTEX_ATTRIB_POSITION,
			VERTEX_ATTRIB_NORMAL,
			VERTEX_ATTRIB_TANGENT,
			VERTEX_ATTRIB_BITANGENT,
			VERTEX_ATTRIB_TEX_COORD0,
			VERTEX_ATTRIB_TEX_COORD1,
			VERTEX_ATTRIB_TEX_COORD2,
			VERTEX_ATTRIB_TEX_COORD3,
			VERTEX_ATTRIB_COLOR0,
			VERTEX_ATTRIB_COLOR1,
			VERTEX_ATTRIB_COLOR2,
			VERTEX_ATTRIB_COLOR3,
			VERTEX_ATTRIB_USER,
		};

		enum ValueType
		{
			USHORT = GL_UNSIGNED_SHORT,
			UINT = GL_UNSIGNED_INT,
			FLOAT = GL_FLOAT,
		};

		AtrributeType attributeType;
		unsigned short valueNum;
		ValueType valueType;
	};

	struct Primitive
	{
		enum PrimitiveType
		{
			POINTS = GL_POINTS,
			LINES = GL_LINES,
			TRIANGLES = GL_TRIANGLES,
		};

		PrimitiveType primitiveType;
		VE_Ptr<veUintArray> indices;
	};

	veMesh();
	virtual ~veMesh();

	void setVertexArray(veRealArray *vAry) { _vertices = vAry; }
	veRealArray* getVertexArray() { return _vertices.get(); }

	void setVertexAtrribute(unsigned int attrIndex, const VertexAtrribute &attri) { veAssert(attrIndex < _attributes.size());  _attributes[attrIndex] = attri; }
	void addVertexAtrribute(const VertexAtrribute &attri);
	const VertexAtrribute& getVertexAtrribute(unsigned int attrIndex) const { return _attributes[attrIndex]; }
	unsigned int getVertexAtrributeNum() const { return _attributes.size();  };

	void addPrimitive(const Primitive &primitive);
	const Primitive& getPrimitive(unsigned int idx) const;
	unsigned int getPrimitiveNum() const { return _primitives.size(); };

	bool& needRefresh();

protected:

	virtual void updateImp(veNode *node, veVisualiser *vs) override;

protected:

	VE_Ptr<veRealArray>                _vertices;
	std::vector<VertexAtrribute>       _attributes;
	std::vector<Primitive>             _primitives;
	bool                               _needRefresh;
};

#endif