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

void veTerrainGrid::build(veReal x, veReal y, unsigned short size, veTexture2D *heightTexture)
{
    _vertices->clear();
    _indices->clear();
    _boundingBox.dirty();
    veArray<veVec3> vertices;
    veArray<veVec3> normals;
    veArray<veVec3> tangents;
    veArray<veVec3> bitangents;
    veArray<veVec2> uvs;
    for (unsigned int yStep = 0; yStep < size; ++yStep) {
        for (unsigned int xStep = 0; xStep < size; ++xStep) {
            veReal xPos = x + xStep;
            veReal yPos = 0.0f;
            veReal zPos = y + yStep;
            if (heightTexture != nullptr) {
                heightTexture->getData((int)xPos, (int)zPos, 0);
            }
            vertices.push_back(veVec3(xPos, yPos, zPos));
            uvs.push_back(veVec2((veReal)xStep / (veReal)(size - 1), (veReal)yStep / (veReal)(size - 1)));
            _boundingBox.expandBy(veVec3(x + xStep, 0.0f, y + yStep));
        }
    }
    normals.resize(vertices.size());
    tangents.resize(vertices.size());
    bitangents.resize(vertices.size());
    
    
    auto normalCalcFunc = [](size_t idx0, size_t idx1, size_t idx2, size_t idx3, const veArray<veVec3> &vertices, veArray<veVec3> &normals){
        veVec3 point0 = vertices[idx0];
        veVec3 point1 = vertices[idx1];
        veVec3 point2 = vertices[idx2];
        veVec3 point3 = vertices[idx3];
        
        veVec3 dir0 = point2 - point0;
        veVec3 dir1 = point1 - point0;
        dir0.normalize();
        dir1.normalize();
        veVec3 norm = dir1.crossProduct(dir0);
        normals[idx0] += norm;
        normals[idx1] += norm;
        normals[idx2] += norm;
        
        dir0 = point3 - point2;
        dir1 = point1 - point2;
        dir0.normalize();
        dir1.normalize();
        norm = dir1.crossProduct(dir0);
        normals[idx1] += norm;
        normals[idx2] += norm;
        normals[idx3] += norm;
    };
    for (unsigned int yStep = 0; yStep < (size - 1); ++yStep) {
        if (yStep % 2 == 0) {
            for (int xStep = 0; xStep < (size - 1); ++xStep) {
                size_t idx0 = yStep * size + xStep;
                size_t idx1 = (yStep + 1) * size + xStep;
                size_t idx2 = yStep * size + xStep + 1;
                size_t idx3 = (yStep + 1) * size + xStep + 1;
                _indices->push_back(idx0);
                _indices->push_back(idx1);
                _indices->push_back(idx2);
                _indices->push_back(idx3);
                normalCalcFunc(idx0, idx1, idx2, idx3, vertices, normals);
            }
        }else{
            for (int xStep = size - 1; 0 < xStep; --xStep) {
                size_t idx0 = (yStep + 1) * size + xStep;
                size_t idx1 = yStep * size + xStep;
                size_t idx2 = (yStep + 1) * size + xStep - 1;
                size_t idx3 = yStep * size + xStep - 1;
                _indices->push_back(idx0);
                _indices->push_back(idx1);
                _indices->push_back(idx2);
                _indices->push_back(idx3);
                
                normalCalcFunc(idx0, idx1, idx2, idx3, vertices, normals);
            }
        }
    }

    for (unsigned int i = 0; i < normals.size(); ++i) {
        normals[i].normalize();
        veVec3 bitangent = normals[i].crossProduct(veVec3::UNIT_X);
        veVec3 tangent   = bitangent.crossProduct(normals[i]);
        tangents[i]      = tangent;
        bitangents[i]    = bitangent;
    }
    
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        fillVertex(vertices[i]
                   , normals[i], tangents[i], bitangents[i]
                   , uvs[i]);
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
        if (_heightTexture.valid()) {
            _gridNumInWidth  = _heightTexture->getWidth() / _gridSize;
            _gridNumInHeight = _heightTexture->getHeight() / _gridSize;
        }
        for (unsigned short h = 0; h < _gridNumInHeight; ++h) {
            for (unsigned short w = 0; w < _gridNumInWidth; ++w) {
                auto grid = new veTerrainGrid(sm);
                veReal x = w * (_gridSize - 1);
                veReal y = h * (_gridSize - 1);
                grid->build(x, y, _gridSize, _heightTexture.get());
                _terrainGridList.push_back(grid);
                for (auto attachedNode : _attachedNodeList) {
                    attachedNode->addRenderableObject(grid);
                }
            }
        }
        _needBuild = false;
    }
}
