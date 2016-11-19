#include "Terrain.h"
#include "SceneManager.h"
#include "TerrainRenderer.h"
#include "FileCore/File.h"
#include "Constants.h"
#include "Shaders/Terrain.glsl"

veTerrainGrid::veTerrainGrid(veSceneManager *sm)
    : veRenderableObject(sm)
{
    _renderer = new veTerrainRenderer;
    _vertices = new veRealArray;
    _indices  = new veUint16Array;
    initMaterial();
}

veTerrainGrid::~veTerrainGrid()
{

}

void veTerrainGrid::initMaterial()
{
    _material = new veMaterial;
    auto technique = new veTechnique;
    _material->addTechnique(technique);
    auto pass = new vePass;
    technique->addPass(pass);
    
    pass->castShadow() = false;
    
    auto vShader = new veShader(veShader::VERTEX_SHADER, TERRAIN_VERTEX_SHADER);
    vShader->setShaderHeader("#define ATTR_POSITION 0\n#define ATTR_NORMAL 1\n#define ATTR_TANGENT 2\n#define ATTR_BITANGENT 3\n#define ATTR_TEXTURE_COORD0 4\n");
    pass->setShader(vShader);
    
    auto fShader = new veShader(veShader::FRAGMENT_SHADER, TERRAIN_FRAGMENT_SHADER);
    pass->setShader(fShader);
    
    pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass->addUniform(new veUniform("u_NormalMat", NORMAL_MATRIX));
    
    
    pass->addUniform(new veUniform("u_diffuse", veVec3::UNIT_SCALE));
    pass->addUniform(new veUniform("u_specular", veVec3::UNIT_SCALE));
    pass->addUniform(new veUniform("u_lightMask", 1.0f));
    pass->addUniform(new veUniform("u_roughness", 1.0f));
    pass->addUniform(new veUniform("u_fresnel", 0.0f));
}

void veTerrainGrid::build(veReal x, veReal y, unsigned short size, veReal *heightMap, unsigned short width, unsigned int height)
{
    _vertices->clear();
    _indices->clear();
    _boundingBox.dirty();
    for (unsigned int yStep = 0; yStep < size; ++yStep) {
        for (unsigned int xStep = 0; xStep < size; ++xStep) {
            fillVertex(veVec3(x + xStep, 0.0f, y + yStep)
                     , veVec3::UNIT_Y, veVec3::UNIT_X, veVec3::UNIT_Z
                     , veVec2((veReal)xStep / (veReal)(size - 1), (veReal)yStep / (veReal)(size - 1)));
            _boundingBox.expandBy(veVec3(x + xStep, 0.0f, y + yStep));
        }
    }
    
    for (unsigned int yStep = 0; yStep < (size - 1); ++yStep) {
        if (yStep % 2 == 0) {
            for (int xStep = 0; xStep < size; ++xStep) {
                _indices->push_back( yStep      * size + xStep);
                _indices->push_back((yStep + 1) * size + xStep);
            }
        }else{
            for (int xStep = size - 1; 0 <= xStep; --xStep) {
                _indices->push_back((yStep + 1) * size + xStep);
                _indices->push_back( yStep      * size + xStep);
            }
        }
    }
}

void veTerrainGrid::fillVertex(const veVec3 &position, const veVec3 &normal, const veVec3 &tangent, const veVec3 &bitangent, const veVec2 &uv)
{
    _vertices->push_back(position.x());  _vertices->push_back(position.y());  _vertices->push_back(position.z());
    _vertices->push_back(normal.x());    _vertices->push_back(normal.y());    _vertices->push_back(normal.z());
    _vertices->push_back(tangent.x());   _vertices->push_back(tangent.y());   _vertices->push_back(tangent.z());
    _vertices->push_back(bitangent.x()); _vertices->push_back(bitangent.y()); _vertices->push_back(bitangent.z());
    _vertices->push_back(uv.x());        _vertices->push_back(uv.y());
}

unsigned int veTerrainGrid::getVertexStride()
{
    return sizeof(veReal) * (3 + 3 + 3 + 3 + 2);
}

unsigned int veTerrainGrid::getVertexAtrributeSize(unsigned int idx) const
{
    switch (idx) {
        case 0:
        case 1:
        case 2:
        case 3:
            return 3;
        case 4:
            return 2;
        default:
            break;
    }
    return 0;
}

unsigned int veTerrainGrid::getVertexAtrributeNum() const
{
    return 5;
}

veTerrain::veTerrain()
    : _gridSize(64)
    , _gridNumInWidth(16)
    , _gridNumInHeight(16)
    , _heightMap(nullptr)
    , _heightMapWidth(0)
    , _heightMapHeight(0)
    , _needBuild(false)
{

}

veTerrain::~veTerrain()
{

}

void veTerrain::beforeUpdate(veSceneManager *sm)
{
    if (_needBuild && !_attachedNodeList.empty()) {
        for (auto grid : _terrainGridList) {
            for (auto attachedNode : _attachedNodeList) {
                attachedNode->removeRenderableObject(grid.get());
            }
        }
        _terrainGridList.clear();
        for (unsigned short h = 0; h < _gridNumInHeight; ++h) {
            for (unsigned short w = 0; w < _gridNumInWidth; ++w) {
                auto grid = new veTerrainGrid(sm);
                veReal x = (w - 0.5f * _gridNumInWidth) * (_gridSize - 1);
                veReal y = (h - 0.5f * _gridNumInHeight) * (_gridSize - 1);
                grid->build(x, y, _gridSize, _heightMap, _heightMapWidth, _heightMapHeight);
                _terrainGridList.push_back(grid);
                for (auto attachedNode : _attachedNodeList) {
                    attachedNode->addRenderableObject(grid);
                }
            }
        }
        _needBuild = false;
    }
}

void veTerrain::setHeightMap(veReal *heightMap, unsigned short width, unsigned int height)
{
    _heightMap = heightMap;
    _heightMapWidth = width;
    _heightMapHeight = height;
}