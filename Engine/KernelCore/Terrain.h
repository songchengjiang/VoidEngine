#ifndef _VE_TERRAIN_
#define _VE_TERRAIN_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "VE_Ptr.h"
#include "Component.h"
#include "Texture.h"
#include <unordered_map>

class VE_EXPORT veTerrainGrid : public veRenderableObject
{
public:
    veTerrainGrid(veSceneManager *sm);
    virtual ~veTerrainGrid();
    
    void build(veReal x, veReal y, unsigned short size, veTexture2D *heightTexture);
    
   	veRealArray* getVertexArray() { return _vertices.get(); }
    unsigned int getVertexStride();
    veUint16Array* getIndexArray() { return _indices.get(); }
    unsigned int getVertexAtrributeSize(unsigned int idx) const;
    unsigned int getVertexAtrributeNum() const;
    
    
private:
    
    void fillVertex(const veVec3 &position, const veVec3 &normal, const veVec3 &tangent, const veVec3 &bitangent, const veVec2 &uv);
    void initMaterial();
    
private:
    
    VE_Ptr<veRealArray>   _vertices;
    VE_Ptr<veUint16Array> _indices;
};
typedef std::vector< VE_Ptr<veTerrainGrid> > veTerrainGridList;

class VE_EXPORT veTerrain : public veComponent
{
public:
    
    veTerrain();
    virtual ~veTerrain();
    
    virtual void update(veSceneManager *sm) override;
    
    void setGridSize(unsigned short size) { _gridSize = size; }
    void setGridNumInWidth(unsigned short num) { _gridNumInWidth = num; }
    void setGridNumInHeight(unsigned short num) { _gridNumInHeight = num; }
    void setHeightTexture(veTexture2D *texture) { _heightTexture = texture; }
    
    void build() { _needBuild = true; }
    
private:
    
    unsigned short    _gridSize;
    unsigned short    _gridNumInWidth;
    unsigned short    _gridNumInHeight;
    bool              _needBuild;
    
    VE_Ptr<veTexture2D> _heightTexture;
    
    veTerrainGridList _terrainGridList;
};

#endif
