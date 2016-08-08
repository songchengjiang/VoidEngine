#ifndef _VE_MESH_
#define _VE_MESH_
#include <unordered_map>
#include "Prerequisites.h"
#include "BaseCore/Array.h"
#include "Material.h"
#include "RenderableObject.h"
#include "Bone.h"
#include "VE_Ptr.h"

class veRay;
class VE_EXPORT veMesh : public veRenderableObject
{
    friend class veMeshManager;
	friend class veMeshRenderer;
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
			VERTEX_ATTRIB_BONE_INDICES,
			VERTEX_ATTRIB_BONE_WEIGHTS,
			VERTEX_ATTRIB_USER0,
			VERTEX_ATTRIB_USER1,
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
            LINE_LOOP = GL_LINE_LOOP,
            LINE_STRIP = GL_LINE_STRIP,
			TRIANGLES = GL_TRIANGLES,
            TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
            TRIANGLE_FAN = GL_TRIANGLE_FAN,
		};

		PrimitiveType primitiveType;
		VE_Ptr<veUint16Array> indices;
	};

	virtual ~veMesh();

	//virtual void update(veNode *node, veSceneManager *sm) override;

    virtual void render(veNode *node, veCamera *camera, unsigned int contextID) override;
    virtual void update(veNode *node, veSceneManager *sm) override;
    virtual bool intersectWith(veRay *ray, veNode *node) override;
    
	void setVertexArray(veRealArray *vAry) { _vertices = vAry; _needRefresh = true; }
	veRealArray* getVertexArray() { return _vertices.get(); }
	unsigned int getVertexStride();
	unsigned int getVertexCount();

	void setVertexAtrribute(unsigned int attrIndex, const VertexAtrribute &attri) { veAssert(attrIndex < _attributes.size());  _attributes[attrIndex] = attri; _needRefresh = true; }
	void addVertexAtrribute(const VertexAtrribute &attri);
	const VertexAtrribute& getVertexAtrribute(size_t attrIndex) const { return _attributes[attrIndex]; }
	bool getVertexAtrribute(VertexAtrribute::AtrributeType aType, VertexAtrribute &attri) const;
	bool getVertexAtrributeSizeInByte(VertexAtrribute::AtrributeType aType, unsigned int &sizeInByte) const;
	size_t getVertexAtrributeNum() const { return _attributes.size();  };

	void addPrimitive(const Primitive &primitive);
	const Primitive& getPrimitive(size_t idx) const;
	size_t getPrimitiveNum() const { return _primitives.size(); };

	void addBone(veBone *bone);
	veBone* getBone(unsigned int idx);
	const veBone* getBone(unsigned int idx) const;
	size_t getBoneNum() const { return _bones.size(); };

	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }

	typedef std::function<bool(const veReal* /*p1*/, const veReal* /*p2*/, const veReal* /*p3*/
		                     , const veReal* /*n1*/, const veReal* /*n2*/, const veReal* /*n3*/)> PrimitiveCallback;
	void traversePrimitives(const PrimitiveCallback &callback);

protected:
    
    veMesh(veSceneManager *sm);
    
protected:

    void caculateBoundingBox();
    void updateBoundingBox();
    bool intersectWith(veRay *ray, veVec3 &position, veVec3 &normal);
	void generateTransformFeedbackBuffer(unsigned int contextID);
	GLuint getTransformFeedbackBuffer(unsigned int contextID) { return _transformFeedbackBuffer->getData(contextID); }
	GLsizeiptr getTransformFeedbackBufferSize() { return _transformFeedbackBufferSize; }

protected:

	VE_Ptr<veRealArray>                _vertices;
	std::vector<VertexAtrribute>       _attributes;
	std::vector<Primitive>             _primitives;
	std::vector< VE_Ptr<veBone> >      _bones;
	veReal                            *_transformFeedbackVertices;
    VE_Ptr<veGLDataBuffer>             _transformFeedbackBuffer;
	GLsizeiptr                         _transformFeedbackBufferSize;
	unsigned int                       _vertexStride;
	bool                               _needRefresh;
};

#endif